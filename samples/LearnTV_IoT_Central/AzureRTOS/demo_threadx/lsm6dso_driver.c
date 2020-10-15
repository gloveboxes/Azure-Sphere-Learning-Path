/*
 * (C) 2005-2020 MediaTek Inc. All rights reserved.
 *
 * Copyright Statement:
 *
 * This MT3620 driver software/firmware and related documentation
 * ("MediaTek Software") are protected under relevant copyright laws.
 * The information contained herein is confidential and proprietary to
 * MediaTek Inc. ("MediaTek"). You may only use, reproduce, modify, or
 * distribute (as applicable) MediaTek Software if you have agreed to and been
 * bound by this Statement and the applicable license agreement with MediaTek
 * ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User"). If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE
 * PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS
 * ANY AND ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO
 * LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED
 * HEREUNDER WILL BE ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY
 * RECEIVER TO MEDIATEK DURING THE PRECEDING TWELVE (12) MONTHS FOR SUCH
 * MEDIATEK SOFTWARE AT ISSUE.
 */

#include "printf.h"
#include "mt3620.h"

#include "os_hal_uart.h"
#include "os_hal_i2c.h"

#include "lsm6dso_driver.h"
#include "lsm6dso_reg.h"

static int lsm6dso_handle;
static lsm6dso_ctx_t dev_ctx;
static axis3bit16_t data_raw_acceleration;
static axis3bit16_t data_raw_angular_rate;
static axis3bit16_t raw_angular_rate_calibration;
static axis1bit16_t data_raw_temperature;
static float acceleration_mg[3];
static float angular_rate_dps[3];
static float lsm6dsoTemperature_degC;


/******************************************************************************/
/* Functions */
/******************************************************************************/
void lsm6dso_show_result(void)
{
	uint8_t reg;

	/* Read output only if new xl value is available */
	lsm6dso_xl_flag_data_ready_get(&dev_ctx, &reg);
	if (reg) {
		/* Read acceleration field data */
		memset(data_raw_acceleration.u8bit, 0x00, 3 * sizeof(int16_t));
		lsm6dso_acceleration_raw_get(&dev_ctx, data_raw_acceleration.u8bit);

		acceleration_mg[0] = lsm6dso_from_fs4_to_mg(data_raw_acceleration.i16bit[0]);
		acceleration_mg[1] = lsm6dso_from_fs4_to_mg(data_raw_acceleration.i16bit[1]);
		acceleration_mg[2] = lsm6dso_from_fs4_to_mg(data_raw_acceleration.i16bit[2]);

		//printf("\n[LSM6DSO] Acceleration [mg]  : %.4lf, %.4lf, %.4lf\n",
		//	acceleration_mg[0], acceleration_mg[1], acceleration_mg[2]);
	}

	lsm6dso_gy_flag_data_ready_get(&dev_ctx, &reg);
	if (reg) {
		/* Read angular rate field data */
		memset(data_raw_angular_rate.u8bit, 0x00, 3 * sizeof(int16_t));
		lsm6dso_angular_rate_raw_get(&dev_ctx, data_raw_angular_rate.u8bit);

		/* Before we store the mdps values subtract the calibration data we captured at startup. */
		angular_rate_dps[0] = (lsm6dso_from_fs2000_to_mdps(data_raw_angular_rate.i16bit[0] -
					raw_angular_rate_calibration.i16bit[0])) / 1000.0;
		angular_rate_dps[1] = (lsm6dso_from_fs2000_to_mdps(data_raw_angular_rate.i16bit[1] -
					raw_angular_rate_calibration.i16bit[1])) / 1000.0;
		angular_rate_dps[2] = (lsm6dso_from_fs2000_to_mdps(data_raw_angular_rate.i16bit[2] -
					raw_angular_rate_calibration.i16bit[2])) / 1000.0;

		//printf("[LSM6DSO] Angular rate [dps] : %4.2f, %4.2f, %4.2f\n",
		//	angular_rate_dps[0], angular_rate_dps[1], angular_rate_dps[2]);
	}

	lsm6dso_temp_flag_data_ready_get(&dev_ctx, &reg);
	if (reg) {
		/* Read temperature data */
		memset(data_raw_temperature.u8bit, 0x00, sizeof(int16_t));
		lsm6dso_temperature_raw_get(&dev_ctx, data_raw_temperature.u8bit);
		lsm6dsoTemperature_degC = lsm6dso_from_lsb_to_celsius(data_raw_temperature.i16bit);

		//memset(data_raw_pressure.u8bit, 0x00, sizeof(int32_t));
		//pressure_hPa = lps22hh_from_lsb_to_hpa(data_raw_pressure.i32bit);

		//printf("[LSM6DSO] Temperature  [degC]: %.2f\n", lsm6dsoTemperature_degC);
	}
}

void update_temperature(void)
{
	uint8_t reg;

	lsm6dso_temp_flag_data_ready_get(&dev_ctx, &reg);
	if (reg)
	{
		/* Read temperature data */
		memset(data_raw_temperature.u8bit, 0x00, sizeof(int16_t));
		lsm6dso_temperature_raw_get(&dev_ctx, data_raw_temperature.u8bit);
		lsm6dsoTemperature_degC = lsm6dso_from_lsb_to_celsius(data_raw_temperature.i16bit);

		//memset(data_raw_pressure.u8bit, 0x00, sizeof(int32_t));
		//pressure_hPa = lps22hh_from_lsb_to_hpa(data_raw_pressure.i32bit);

		//printf("[LSM6DSO] Temperature  [degC]: %.2f\n", lsm6dsoTemperature_degC);
	}
}

float get_temperature(void) {
	update_temperature();
	return lsm6dsoTemperature_degC;
}


int lsm6dso_init(void *i2c_write, void *i2c_read)
{
	uint8_t reg;

	if (i2c_write == NULL || i2c_read == NULL) {
		printf("Invalid parameters!\n");
		return -1;
	}

	dev_ctx.write_reg = i2c_write;
	dev_ctx.read_reg = i2c_read;
	dev_ctx.handle = &lsm6dso_handle;

	/* Check Device ID */
	lsm6dso_device_id_get(&dev_ctx, &reg);
	if (reg == LSM6DSO_ID) {
		printf("LSM6DSO Found!\n");
	} else {
		printf("LSM6DSO not found! (0x%02X)\n", reg);
		return -1;
	}

	/* Restore Default Configuration */
	lsm6dso_reset_set(&dev_ctx, PROPERTY_ENABLE);
	do {
		lsm6dso_reset_get(&dev_ctx, &reg);
	} while (reg);

	/* Disable I3C interface */
	lsm6dso_i3c_disable_set(&dev_ctx, LSM6DSO_I3C_DISABLE);

	/* Enable Block Data Update */
	lsm6dso_block_data_update_set(&dev_ctx, PROPERTY_ENABLE);

	/* Set Output Data Rate */
	lsm6dso_xl_data_rate_set(&dev_ctx, LSM6DSO_XL_ODR_12Hz5);
	lsm6dso_gy_data_rate_set(&dev_ctx, LSM6DSO_GY_ODR_12Hz5);

	/* Set full scale */
	lsm6dso_xl_full_scale_set(&dev_ctx, LSM6DSO_4g);
	lsm6dso_gy_full_scale_set(&dev_ctx, LSM6DSO_2000dps);

	/* Configure filtering chain(No aux interface) */
	/* Accelerometer - LPF1 + LPF2 path */
	lsm6dso_xl_hp_path_on_out_set(&dev_ctx, LSM6DSO_LP_ODR_DIV_100);
	lsm6dso_xl_filter_lp2_set(&dev_ctx, PROPERTY_ENABLE);

	return 0;
}

void calibrate_lsm6dso(void) {
	//printf("LSM6DSO: Calibrating angular rate...\n");
	//printf("LSM6DSO: Please make sure the device is stationary.\n");
	//do {
	//	/* Delay and read the device until we have data! */
	//	do {
	//		/* Read the calibration values */
	//		tx_thread_sleep(500);
	//		//vTaskDelay(pdMS_TO_TICKS(5000));
	//		lsm6dso_gy_flag_data_ready_get(&dev_ctx, &reg);
	//	} while (!reg);

	//	printf("LSM6DSO: calibration on-going...\n");
	//	if (reg) {
	//		/* Read angular rate field data to use for calibration offsets */
	//		memset(data_raw_angular_rate.u8bit, 0x00, 3 * sizeof(int16_t));
	//		lsm6dso_angular_rate_raw_get(&dev_ctx, raw_angular_rate_calibration.u8bit);
	//	}

	//	/* Delay and read the device until we have data! */
	//	do {
	//		/* Read the calibration values */
	//		tx_thread_sleep(500);
	//		//vTaskDelay(pdMS_TO_TICKS(5000));
	//		lsm6dso_gy_flag_data_ready_get(&dev_ctx, &reg);
	//	} while (!reg);

	//	/* Read the angular data rate again and verify that after applying the calibration,
	//	 * we have 0 angular rate in all directions
	//	*/
	//	if (reg) {
	//		/* Read angular rate field data */
	//		memset(data_raw_angular_rate.u8bit, 0x00, 3 * sizeof(int16_t));
	//		lsm6dso_angular_rate_raw_get(&dev_ctx, data_raw_angular_rate.u8bit);

	//		/* Before we store the mdps values subtract the calibration data we captured at startup. */
	//		angular_rate_dps[0] = lsm6dso_from_fs2000_to_mdps(data_raw_angular_rate.i16bit[0] -
	//					raw_angular_rate_calibration.i16bit[0]);
	//		angular_rate_dps[1] = lsm6dso_from_fs2000_to_mdps(data_raw_angular_rate.i16bit[1] -
	//					raw_angular_rate_calibration.i16bit[1]);
	//		angular_rate_dps[2] = lsm6dso_from_fs2000_to_mdps(data_raw_angular_rate.i16bit[2] -
	//					raw_angular_rate_calibration.i16bit[2]);
	//	}
	//	/* If the angular values after applying the offset are not all 0.0s, then do it again! */
	//} while ((angular_rate_dps[0] != 0.0) || (angular_rate_dps[1] != 0.0) || (angular_rate_dps[2] != 0.0));
	//printf("LSM6DSO: Calibrating angular rate complete!\n");
}

