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

#ifndef __OS_HAL_I2C_H__
#define __OS_HAL_I2C_H__

#include "mhal_i2c.h"

/**
 * @addtogroup OS-HAL
 * @{
 * @addtogroup i2c
 * @{
 * This section introduces the Inter-Integrated Circuit (I2C) APIs
 * including terms and acronyms, supported features,
 * details on how to use this driver, enums and functions.
 *
 * @section OS_HAL_I2C_Terms_Chapter Terms and Acronyms
 *
 * |Terms                   |Details                             |
 * |------------------------------|--|
 * |\b DMA                        | Direct Memory Access.|
 * |\b FIFO                       | First In, First Out.|
 * |\b I2C                        | Inter-Integrated Circuit. |
 *
 * @section OS_HAL_I2C_Features_Chapter Supported Features
 * See @ref MHAL_I2C_Features_Chapter for the details of  Supported Features.
 *
 * @}
 * @}
 */

/**
 * @addtogroup OS-HAL
 * @{
 * @addtogroup i2c
 * @{
 * @section OS_HAL_I2C_Driver_Usage_Chapter How to use this driver
 *
 * - \b Device \b driver \b sample \b code \b is \b as \b follows: \n
 *  - sample code (this is the user application sample code on freeRTos):
 *    @code
 *	- init I2C
 *	 -Call mtk_os_hal_i2c_ctrl_init(i2c_num bus_num)
 *	   to init i2c bus.
 *
 *	- set I2C master transfer speed
 *	  -Call mtk_os_hal_i2c_speed_init(i2c_num bus_num,
			     enum i2c_speed_kHz speed)
 *
 *	- I2C master read data from slave device
 *	 -Call mtk_os_hal_i2c_read(i2c_num bus_num,
			    u8 device_addr, u8 *buffer, u16 len)
 *
 *	- I2C master write data to slave device
 *	 -Call mtk_os_hal_i2c_write(i2c_num bus_num,
			    u8 device_addr, u8 *buffer, u16 len)
 *
 *	-I2C master write data to slave device then read data from device
 *	 -Call mtk_os_hal_i2c_write_read(i2c_num bus_num,
			    u8 device_addr, u8 *wr_buf, u8 *rd_buf,
			    u16 wr_len, u16 rd_len)
 *
 *	-Set I2C controler slave address
 *	 -Call mtk_os_hal_i2c_set_slave_addr(i2c_num bus_num,
			    u8 slv_addr)
 *
 *	-Set I2C slave send data to the master
 *	 -Call mtk_os_hal_i2c_slave_tx(i2c_num bus_num,
			    u8 *buffer, u16 len, u32 time_out)
 *
 *	-Set I2C slave receive data from the master
 *	 -Call mtk_os_hal_i2c_slave_rx(i2c_num bus_num,
			    u8 *buffer, u16 len, u32 time_out)
 *
 *	- uninit I2C
 *	 - Call  mtk_os_hal_i2c_ctrl_deinit(i2c_num bus_num) to uninit
 *	    i2c and release resource.
 *
 *    @endcode
 *
 * @}
 * @}
 */

/**
* @addtogroup OS-HAL
* @{
* @addtogroup i2c
* @{
*/

/** @defgroup os_hal_i2c_enum Enum
  * @{
  * This section introduces the enumerations
  * that I2C uses while performing transfer.
  */

typedef enum {
	/** Use ISU0 as I2C port */
	OS_HAL_I2C_ISU0 = 0,
	/** Use ISU1 as I2C port */
	OS_HAL_I2C_ISU1 = 1,
	/** Use ISU2 as I2C port */
	OS_HAL_I2C_ISU2 = 2,
	/** Use ISU3 as I2C port */
	OS_HAL_I2C_ISU3 = 3,
	/** Use ISU4 as I2C port */
	OS_HAL_I2C_ISU4 = 4,
	/** The maximum ISU number (invalid) */
	OS_HAL_I2C_ISU_MAX
} i2c_num;

/**
  * @}
  */


/** @defgroup os_hal_i2c_function Function
  * @{
   * This section provides high level APIs to upper layer.
  */

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @brief  Init I2C controller.
 *
 *  @param [in] bus_num : I2C ISU Port number,
 *  it can be OS_HAL_I2C_ISU0~OS_HAL_I2C_ISU4
 *
 *  @return negative value means fail.
 *  @return 0 means success.
 */
int mtk_os_hal_i2c_ctrl_init(i2c_num bus_num);

/**
 *  @brief  Deinit I2C controller.
 *
 *  @param [in] bus_num : I2C ISU Port number,
 *  it can be OS_HAL_I2C_ISU0~OS_HAL_I2C_ISU4
 *
 *  @return negative value means fail.
 *  @return 0 means success.
 */
int mtk_os_hal_i2c_ctrl_deinit(i2c_num bus_num);

/**
 *  @brief Initialize i2c master transfer speed
 *
 *  @param [in] bus_num : I2C ISU Port number,
 *  it can be OS_HAL_I2C_ISU0~OS_HAL_I2C_ISU4
 *
 *  @param [in] i2c_speed_kHz : It can be set
 *  to one of the enum #i2c_speed_kHz.
 *
 *  @return "0" if I2C sets speed successfully.\n
 *  @return -#I2C_EPTR if i2c is NULL.\n
 *  @return -#I2C_EINVAL if a parameter is invalid.
 */
int mtk_os_hal_i2c_speed_init(i2c_num bus_num, enum i2c_speed_kHz speed);

/**
 *  @brief I2C master read data from slave device.
 *
 *  @param [in] bus_num : I2C ISU Port number,
 *  it can be OS_HAL_I2C_ISU0~OS_HAL_I2C_ISU4.
 *
 *  @param [in] device_addr : slave device address.
 *
 *  @param [in] buffer : read data buffer.
 *
 *  @param [in] len : read data length.
 *
 *  @return negative value means fail.
 *
 *  @return "0" if I2C read data from slave device successfully.
 */
int mtk_os_hal_i2c_read(i2c_num bus_num, u8 device_addr, u8 *buffer, u16 len);

/**
 *  @brief I2C master write data to slave device.
 *
 *  @param [in] bus_num : I2C ISU Port number,
 *  it can be OS_HAL_I2C_ISU0~OS_HAL_I2C_ISU4.
 *
 *  @param [in] device_addr : slave device address.
 *
 *  @param [in] buffer : write data buffer.
 *
 *  @param [in] len : write data length.
 *
 *  @return negative value means fail.
 *
 *  @return "0" if I2C write data to slave device successfully.
 */
int mtk_os_hal_i2c_write(i2c_num bus_num, u8 device_addr, u8 *buffer, u16 len);

/**
 *  @brief I2C master write data to slave device then read data from device.
 *
 *  @param [in] bus_num : I2C ISU Port number,
 *  it can be OS_HAL_I2C_ISU0~OS_HAL_I2C_ISU4.
 *
 *  @param [in] device_addr : slave device address.
 *
 *  @param [in] wr_buf : write data buffer.
 *
 *  @param [in] rd_buf : read data buffer.
 *
 *  @param [in] wr_len : write data length.
 *
 *  @param [in] rd_len : read data length.
 *
 *  @return negative value means fail.
 *
 *  @return "0" if I2C write data to slave device successfully.
 */
int mtk_os_hal_i2c_write_read(i2c_num bus_num, u8 device_addr,
			      u8 *wr_buf, u8 *rd_buf, u16 wr_len, u16 rd_len);

/**
 *  @brief Set I2C slave address before transfer when I2C hardware
 *  controller is set as a slave role, it which means does not call
 *  this function when hardware is set as an I2C master role.
 *
 *  @param [in] bus_num : I2C ISU Port number,
 *  it can be OS_HAL_I2C_ISU0~OS_HAL_I2C_ISU4.
 *
 *  @param [in] slv_addr : Set controller slave address.
 *
 *  @return "0" if I2C sets slave address successfully.\n
 *
 *  @return -#I2C_EPTR if i2c is NULL.\n
 *
 *  @rerurn -#I2C_EINVAL If a parameter is invalid then I2C slave address
 *  uses 0x20 by default.
 */
int mtk_os_hal_i2c_set_slave_addr(i2c_num bus_num, u8 slv_addr);

/**
 *  @brief I2C slave wait for master to trigger transfer and
 *  then send data to the master.
 *
 *  @param [in] bus_num : I2C ISU Port number,
 *  it can be OS_HAL_I2C_ISU0~OS_HAL_I2C_ISU4.
 *
 *  @param [in] buffer : send data buffer.
 *
 *  @param [in] len : i2c slave send data length.
 *
 *  @param [in] time_out : i2c slave waiting time.
 *
 *  @return negative value means fail.
 *
 *  @return "0" if i2c send data to master successfully.
 */
int mtk_os_hal_i2c_slave_tx(i2c_num bus_num, u8 *buffer, u16 len, u32 time_out);

/**
 *  @brief I2C slave wait for master to trigger transfer and
 *  then receive data from the master.
 *
 *  @param [in] bus_num : I2C ISU Port number,
 *  it can be OS_HAL_I2C_ISU0~OS_HAL_I2C_ISU4.
 *
 *  @param [in] buffer : receive data buffer.
 *
 *  @param [in] len : i2c slave receive data length.
 *
 *  @param [in] time_out : i2c slave waiting time.
 *
 *  @return negative value means fail.
 *
 *  @return "0" if i2c receive data from master successfully.
 */
int mtk_os_hal_i2c_slave_rx(i2c_num bus_num, u8 *buffer, u16 len, u32 time_out);

#ifdef __cplusplus
}
#endif

/**
  * @}
  */

/**
* @}
* @}
*/

#endif
