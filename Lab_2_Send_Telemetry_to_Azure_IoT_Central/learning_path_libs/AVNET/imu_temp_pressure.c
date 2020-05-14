/*
 *  Some of the code in this file was copied from ST Micro.  Below is their required information.
 *
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2018 STMicroelectronics</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its
 *      contributors may be used to endorse or promote products derived from
 *      this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */


#include "imu_temp_pressure.h"


/* Private variables ---------------------------------------------------------*/
static axis3bit16_t data_raw_acceleration;
static axis3bit16_t data_raw_angular_rate;
static axis3bit16_t raw_angular_rate_calibration;
static axis1bit32_t data_raw_pressure;
static axis1bit16_t data_raw_temperature;
//static float acceleration_mg[3];
//static float angular_rate_dps[3];
//static float lsm6dsoTemperature_degC;
static float pressure_hPa;
static float lps22hhTemperature_degC;

static uint8_t whoamI, rst;
int accelTimerFd;
const uint8_t lsm6dsOAddress = LSM6DSO_ADDRESS;     // Addr = 0x6A
lsm6dso_ctx_t dev_ctx;
lps22hh_ctx_t pressure_ctx;
bool lps22hhDetected;


//Extern variables
int i2cFd = -1;

AngularRateDegreesPerSecond angularRateDps;
AccelerationMilligForce accelerationMilligForce;


//Private functions

// Routines to read/write to the LSM6DSO device
static int32_t platform_write(int *fD, uint8_t reg, uint8_t *bufp, uint16_t len);
static int32_t platform_read(int *fD, uint8_t reg, uint8_t *bufp, uint16_t len);

// Routines to read/write to the LPS22HH device connected to the LSM6DSO sensor hub
static int32_t lsm6dso_write_lps22hh_cx(void* ctx, uint8_t reg, uint8_t* data, uint16_t len);
static int32_t lsm6dso_read_lps22hh_cx(void* ctx, uint8_t reg, uint8_t* data, uint16_t len);

/// <summary>
///     Sleep for delayTime ms
/// </summary>
void HAL_Delay(int delayTime) {
	struct timespec ts;
	ts.tv_sec = 0;
	ts.tv_nsec = delayTime * 10000;
	nanosleep(&ts, NULL);
}

/// <summary>
///     Print latest data from on-board sensors.
/// </summary>
bool AvnetSkSensorUpdate(void)
{

	uint8_t reg;
	lps22hh_reg_t lps22hhReg;

	// Read the sensors on the lsm6dso device

	//Read output only if new xl value is available
	lsm6dso_xl_flag_data_ready_get(&dev_ctx, &reg);
	if (reg)
	{
		// Read acceleration field data
		memset(data_raw_acceleration.u8bit, 0x00, 3 * sizeof(int16_t));
		lsm6dso_acceleration_raw_get(&dev_ctx, data_raw_acceleration.u8bit);
		
		accelerationMilligForce.x = lsm6dso_from_fs4_to_mg(data_raw_acceleration.i16bit[0]);
		accelerationMilligForce.y = lsm6dso_from_fs4_to_mg(data_raw_acceleration.i16bit[1]);
		accelerationMilligForce.z = lsm6dso_from_fs4_to_mg(data_raw_acceleration.i16bit[2]);

		//Log_Debug("\nLSM6DSO: Acceleration [mg]  : %.4lf, %.4lf, %.4lf\n",
		//	accelerationMilligForce.x, accelerationMilligForce.y, accelerationMilligForce.z);
	}

	lsm6dso_gy_flag_data_ready_get(&dev_ctx, &reg);
	if (reg)
	{
		// Read angular rate field data
		memset(data_raw_angular_rate.u8bit, 0x00, 3 * sizeof(int16_t));
		lsm6dso_angular_rate_raw_get(&dev_ctx, data_raw_angular_rate.u8bit);

		// Before we store the mdps values subtract the calibration data we captured at startup.
		angularRateDps.x = (lsm6dso_from_fs2000_to_mdps(data_raw_angular_rate.i16bit[0] - raw_angular_rate_calibration.i16bit[0])) / 1000.0;
		angularRateDps.y = (lsm6dso_from_fs2000_to_mdps(data_raw_angular_rate.i16bit[1] - raw_angular_rate_calibration.i16bit[1])) / 1000.0;
		angularRateDps.z = (lsm6dso_from_fs2000_to_mdps(data_raw_angular_rate.i16bit[2] - raw_angular_rate_calibration.i16bit[2])) / 1000.0;

		//Log_Debug("LSM6DSO: Angular rate [dps] : %4.2f, %4.2f, %4.2f\r\n",
		//	angularRateDps.x, angularRateDps.y, angularRateDps.z);


	}

	//lsm6dso_temp_flag_data_ready_get(&dev_ctx, &reg);
	//if (reg)
	//{
	//	// Read temperature data
	//	memset(data_raw_temperature.u8bit, 0x00, sizeof(int16_t));
	//	lsm6dso_temperature_raw_get(&dev_ctx, data_raw_temperature.u8bit);
	//	lsm6dsoTemperature_degC = lsm6dso_from_lsb_to_celsius(data_raw_temperature.i16bit);

	//	Log_Debug("LSM6DSO: Temperature  [degC]: %.2f\r\n", lsm6dsoTemperature_degC);
	//}

	// Read the lps22hh sensor on the lsm6dso device

	// Initialize the data structures to 0s.
	memset(data_raw_pressure.u8bit, 0x00, sizeof(int32_t));
	memset(data_raw_temperature.u8bit, 0x00, sizeof(int16_t));

	if (lps22hhDetected) {
		lps22hh_read_reg(&pressure_ctx, LPS22HH_STATUS, (uint8_t *)&lps22hhReg, 1);

		//Read output only if new value is available

		if ((lps22hhReg.status.p_da == 1) && (lps22hhReg.status.t_da == 1))
		{
			lps22hh_pressure_raw_get(&pressure_ctx, data_raw_pressure.u8bit);

			pressure_hPa = lps22hh_from_lsb_to_hpa(data_raw_pressure.i32bit);

			lps22hh_temperature_raw_get(&pressure_ctx, data_raw_temperature.u8bit);
			lps22hhTemperature_degC = lps22hh_from_lsb_to_celsius(data_raw_temperature.i16bit);

			//Log_Debug("LPS22HH: Pressure     [hPa] : %.2f\r\n", pressure_hPa);
			//Log_Debug("LPS22HH: Temperature  [degC]: %.2f\r\n", lps22hhTemperature_degC);

			return true;
		}
	}
	// LPS22HH was not detected
	else {
		Log_Debug("LPS22HH: Pressure     [hPa] : Not read!\r\n");
		Log_Debug("LPS22HH: Temperature  [degC]: Not read!\r\n");
		return false;
	}
	return false;
}

float GetPressure(void) {
	return pressure_hPa;
}

float GetTemperature(void) {
	return lps22hhTemperature_degC;
}

AngularRateDegreesPerSecond GetAngularRate(void) {
	return angularRateDps;
}

AccelerationMilligForce GetAcceleration(void) {
	return accelerationMilligForce;
}

/// <summary>
///     Initializes the I2C interface.
/// </summary>
/// <returns>0 on success, or -1 on failure</returns>
int initI2c(void) {

	i2cFd = I2CMaster_Open(AVNET_MT3620_SK_ISU2_I2C);
	if (i2cFd < 0) {
		Log_Debug("ERROR: I2CMaster_Open: errno=%d (%s)\n", errno, strerror(errno));
		return -1;
	}

	int result = I2CMaster_SetBusSpeed(i2cFd, I2C_BUS_SPEED_STANDARD);
	if (result != 0) {
		Log_Debug("ERROR: I2CMaster_SetBusSpeed: errno=%d (%s)\n", errno, strerror(errno));
		return -1;
	}

	result = I2CMaster_SetTimeout(i2cFd, 100);
	if (result != 0) {
		Log_Debug("ERROR: I2CMaster_SetTimeout: errno=%d (%s)\n", errno, strerror(errno));
		return -1;
	}

	// Start lsm6dso specific init

	// Initialize lsm6dso mems driver interface
	dev_ctx.write_reg = platform_write;
	dev_ctx.read_reg = platform_read;
	dev_ctx.handle = &i2cFd;

	// Check device ID
	lsm6dso_device_id_get(&dev_ctx, &whoamI);
	if (whoamI != LSM6DSO_ID) {
		Log_Debug("LSM6DSO not found!\n");
		return -1;
	}
	else {
		Log_Debug("LSM6DSO Found!\n");
	}
		
	 // Restore default configuration
	lsm6dso_reset_set(&dev_ctx, PROPERTY_ENABLE);
	do {
		lsm6dso_reset_get(&dev_ctx, &rst);
	} while (rst);

	 // Disable I3C interface
	lsm6dso_i3c_disable_set(&dev_ctx, LSM6DSO_I3C_DISABLE);

	// Enable Block Data Update
	lsm6dso_block_data_update_set(&dev_ctx, PROPERTY_ENABLE);

	 // Set Output Data Rate
	lsm6dso_xl_data_rate_set(&dev_ctx, LSM6DSO_XL_ODR_12Hz5);
	lsm6dso_gy_data_rate_set(&dev_ctx, LSM6DSO_GY_ODR_12Hz5);

	 // Set full scale
	lsm6dso_xl_full_scale_set(&dev_ctx, LSM6DSO_4g);
	lsm6dso_gy_full_scale_set(&dev_ctx, LSM6DSO_2000dps);

	 // Configure filtering chain(No aux interface)
	// Accelerometer - LPF1 + LPF2 path	
	lsm6dso_xl_hp_path_on_out_set(&dev_ctx, LSM6DSO_LP_ODR_DIV_100);
	lsm6dso_xl_filter_lp2_set(&dev_ctx, PROPERTY_ENABLE);

	// lps22hh specific init

	// Default the flag to false.  If we fail to communicate with the LPS22HH device, this flag
	// will cause application execution to skip over LPS22HH specific code.
	lps22hhDetected = false;

	// Initialize lps22hh mems driver interface
	pressure_ctx.read_reg = lsm6dso_read_lps22hh_cx;
	pressure_ctx.write_reg = lsm6dso_write_lps22hh_cx;
	pressure_ctx.handle = &i2cFd;

	int failCount = 10;

	while (!lps22hhDetected) {
		
		// Enable pull up on master I2C interface.
		lsm6dso_sh_pin_mode_set(&dev_ctx, LSM6DSO_INTERNAL_PULL_UP);

		// Check if LPS22HH is connected to Sensor Hub
		lps22hh_device_id_get(&pressure_ctx, &whoamI);
		if (whoamI != LPS22HH_ID) {
			Log_Debug("LPS22HH not found!\n");
		}
		else {
			lps22hhDetected = true;
			Log_Debug("LPS22HH Found!\n");
		}

		// Restore the default configuration
		lps22hh_reset_set(&pressure_ctx, PROPERTY_ENABLE);
		do {
			lps22hh_reset_get(&pressure_ctx, &rst);
		} while (rst);

		// Enable Block Data Update
		lps22hh_block_data_update_set(&pressure_ctx, PROPERTY_ENABLE);

		//Set Output Data Rate
		lps22hh_data_rate_set(&pressure_ctx, LPS22HH_10_Hz_LOW_NOISE);

		// If we failed to detect the lps22hh device, then pause before trying again.
		if (!lps22hhDetected) {
			HAL_Delay(100);
		}

		if (failCount-- == 0) {
			lps22hhDetected = false;
			Log_Debug("Failed to read LPS22HH device ID, disabling all access to LPS22HH device!\n");
			Log_Debug("Usually a power cycle will correct this issue\n");
			break;
		}
	}

	// Read the raw angular rate data from the device to use as offsets.  We're making the assumption that the device
	// is stationary.

	uint8_t reg;
	
	Log_Debug("LSM6DSO: Calibrating angular rate . . .\n"); 
	Log_Debug("LSM6DSO: Please make sure the device is stationary.\n");

	do {

		// Delay and read the device until we have data!
		do {
			// Read the calibration values
			HAL_Delay(5000);
			lsm6dso_gy_flag_data_ready_get(&dev_ctx, &reg);
		} while (!reg);

		if (reg)
		{
			// Read angular rate field data to use for calibration offsets
			memset(data_raw_angular_rate.u8bit, 0x00, 3 * sizeof(int16_t));
			lsm6dso_angular_rate_raw_get(&dev_ctx, raw_angular_rate_calibration.u8bit);
		}

		// Delay and read the device until we have data!
		do {
			// Read the calibration values
			HAL_Delay(5000);
			lsm6dso_gy_flag_data_ready_get(&dev_ctx, &reg);
		} while (!reg);

		// Read the angular data rate again and verify that after applying the calibration, we have 0 angular rate in all directions
		if (reg)
		{

			// Read angular rate field data
			memset(data_raw_angular_rate.u8bit, 0x00, 3 * sizeof(int16_t));
			lsm6dso_angular_rate_raw_get(&dev_ctx, data_raw_angular_rate.u8bit);

			// Before we store the mdps values subtract the calibration data we captured at startup.
			angularRateDps.x = lsm6dso_from_fs2000_to_mdps(data_raw_angular_rate.i16bit[0] - raw_angular_rate_calibration.i16bit[0]);
			angularRateDps.y = lsm6dso_from_fs2000_to_mdps(data_raw_angular_rate.i16bit[1] - raw_angular_rate_calibration.i16bit[1]);
			angularRateDps.z = lsm6dso_from_fs2000_to_mdps(data_raw_angular_rate.i16bit[2] - raw_angular_rate_calibration.i16bit[2]);

		}

	// If the angular values after applying the offset are not all 0.0s, then do it again!
	} while ((angularRateDps.x != 0.0) || (angularRateDps.y != 0.0) || (angularRateDps.z != 0.0));

	Log_Debug("LSM6DSO: Calibrating angular rate complete!\n");

	// Init the epoll interface to periodically run the AccelTimerEventHandler routine where we read the sensors

	//// Define the period in the build_options.h file
	//struct timespec accelReadPeriod = { .tv_sec = ACCEL_READ_PERIOD_SECONDS,.tv_nsec = ACCEL_READ_PERIOD_NANO_SECONDS };
	//// event handler data structures. Only the event handler field needs to be populated.
	//static EventData accelEventData = { .eventHandler = &AccelTimerEventHandler };
	//accelTimerFd = -1;
	//accelTimerFd = CreateTimerFdAndAddToEpoll(epollFd, &accelReadPeriod, &accelEventData, EPOLLIN);
	//if (accelTimerFd < 0) {
	//	return -1;
	//}
	
	return 0;
}

/// <summary>
///     Closes a file descriptor and prints an error on failure.
/// </summary>
/// <param name="fd">File descriptor to close</param>
/// <param name="fdName">File descriptor name to use in error message</param>
void CloseFdPrintError(int fd, const char* fdName)
{
	if (fd >= 0) {
		int result = close(fd);
		if (result != 0) {
			Log_Debug("ERROR: Could not close fd %s: %s (%d).\n", fdName, strerror(errno), errno);
		}
	}
}

/// <summary>
///     Closes the I2C interface File Descriptors.
/// </summary>
void closeI2c(void) {
	CloseFdPrintError(i2cFd, "i2c");
}

/// <summary>
///     Writes data to the lsm6dso i2c device
/// </summary>
/// <returns>0</returns>

static int32_t platform_write(int *fD, uint8_t reg, uint8_t *bufp,
	uint16_t len)
{

#ifdef ENABLE_READ_WRITE_DEBUG
	Log_Debug("platform_write()\n");
	Log_Debug("reg: %0x\n", reg);
	Log_Debug("len: %0x\n", len);
	Log_Debug("bufp contents: ");
	for (int i = 0; i < len; i++) {

		Log_Debug("%0x: ", bufp[i]);
	}
	Log_Debug("\n");
#endif 

	// Construct a new command buffer that contains the register to write to, then the data to write
	uint8_t cmdBuffer[len + 1];
	cmdBuffer[0] = reg;
	for (int i = 0; i < len; i++) {
		cmdBuffer[i + 1] = bufp[i];
	}

#ifdef ENABLE_READ_WRITE_DEBUG
	Log_Debug("cmdBuffer contents: ");
	for (int i = 0; i < len + 1; i++) {

		Log_Debug("%0x: ", cmdBuffer[i]);
	}
	Log_Debug("\n");
#endif

	// Write the data to the device
	int32_t retVal = I2CMaster_Write(*fD, lsm6dsOAddress, cmdBuffer, (size_t)len + 1);
	if (retVal < 0) {
		Log_Debug("ERROR: platform_write: errno=%d (%s)\n", errno, strerror(errno));
		return -1;
	}
#ifdef ENABLE_READ_WRITE_DEBUG
	Log_Debug("Wrote %d bytes to device.\n\n", retVal);
#endif
	return 0;
}

/// <summary>
///     Reads generic device register from the i2c interface
/// </summary>
/// <returns>0</returns>

/*
 * @brief  Read generic device register (platform dependent)
 *
 * @param  handle    customizable argument. In this examples is used in
 *                   order to select the correct sensor bus handler.
 * @param  reg       register to read
 * @param  bufp      pointer to buffer that store the data read
 * @param  len       number of consecutive register to read
 *
 */
static int32_t platform_read(int *fD, uint8_t reg, uint8_t *bufp,
	uint16_t len)
{

#ifdef ENABLE_READ_WRITE_DEBUG
	Log_Debug("platform_read()\n");
	Log_Debug("reg: %0x\n", reg);
	Log_Debug("len: %d\n", len);
;
#endif

	// Set the register address to read
	int32_t retVal = I2CMaster_Write(i2cFd, lsm6dsOAddress, &reg, 1);
	if (retVal < 0) {
		Log_Debug("ERROR: platform_read(write step): errno=%d (%s)\n", errno, strerror(errno));
		return -1;
	}

	// Read the data into the provided buffer
	retVal = I2CMaster_Read(i2cFd, lsm6dsOAddress, bufp, len);
	if (retVal < 0) {
		Log_Debug("ERROR: platform_read(read step): errno=%d (%s)\n", errno, strerror(errno));
		return -1;
	}

#ifdef ENABLE_READ_WRITE_DEBUG
	Log_Debug("Read returned: ");
	for (int i = 0; i < len; i++) {
		Log_Debug("%0x: ", bufp[i]);
	}
	Log_Debug("\n\n");
#endif 	   

	return 0;
}
/*
 * @brief  Write lsm2mdl device register (used by configuration functions)
 *
 * @param  handle    customizable argument. In this examples is used in
 *                   order to select the correct sensor bus handler.
 * @param  reg       register to write
 * @param  bufp      pointer to data to write in register reg
 * @param  len       number of consecutive register to write
 *
 */
static int32_t lsm6dso_write_lps22hh_cx(void* ctx, uint8_t reg, uint8_t* data,
	uint16_t len)
{
	axis3bit16_t data_raw_acceleration;
	int32_t ret;
	uint8_t drdy;
	lsm6dso_status_master_t master_status;
	lsm6dso_sh_cfg_write_t sh_cfg_write;

	// Configure Sensor Hub to write to the LPS22HH, and send the write data
	sh_cfg_write.slv0_add = (LPS22HH_I2C_ADD_L & 0xFEU) >> 1; // 7bit I2C address
	sh_cfg_write.slv0_subadd = reg,
	sh_cfg_write.slv0_data = *data,
	ret = lsm6dso_sh_cfg_write(&dev_ctx, &sh_cfg_write);

	/* Disable accelerometer. */
	lsm6dso_xl_data_rate_set(&dev_ctx, LSM6DSO_XL_ODR_OFF);

	/* Enable I2C Master. */
	lsm6dso_sh_master_set(&dev_ctx, PROPERTY_ENABLE);

	/* Enable accelerometer to trigger Sensor Hub operation. */
	lsm6dso_xl_data_rate_set(&dev_ctx, LSM6DSO_XL_ODR_104Hz);

	/* Wait Sensor Hub operation flag set. */
	lsm6dso_acceleration_raw_get(&dev_ctx, data_raw_acceleration.u8bit);
	do
	{
		HAL_Delay(20);
		lsm6dso_xl_flag_data_ready_get(&dev_ctx, &drdy);
	} while (!drdy);

	do
	{
		HAL_Delay(20);
		lsm6dso_sh_status_get(&dev_ctx, &master_status);
	} while (!master_status.sens_hub_endop);

	/* Disable I2C master and XL (trigger). */
	lsm6dso_sh_master_set(&dev_ctx, PROPERTY_DISABLE);
	lsm6dso_xl_data_rate_set(&dev_ctx, LSM6DSO_XL_ODR_OFF);

	return ret;
}

/*
 * @brief  Read lsm2mdl device register (used by configuration functions)
 *
 * @param  handle    customizable argument. In this examples is used in
 *                   order to select the correct sensor bus handler.
 * @param  reg       register to read
 * @param  bufp      pointer to buffer that store the data read
 * @param  len       number of consecutive register to read
 *
 */
static int32_t lsm6dso_read_lps22hh_cx(void* ctx, uint8_t reg, uint8_t* data, uint16_t len)
{
	lsm6dso_sh_cfg_read_t sh_cfg_read;
	uint8_t buf_raw[6];
	int32_t ret;
	uint8_t drdy;
	lsm6dso_status_master_t master_status;

	/* Disable accelerometer. */
	lsm6dso_xl_data_rate_set(&dev_ctx, LSM6DSO_XL_ODR_OFF);

	/* Configure Sensor Hub to read LPS22HH. */
	sh_cfg_read.slv_add = (LPS22HH_I2C_ADD_L & 0xFEU) >> 1; /* 7bit I2C address */
	sh_cfg_read.slv_subadd = reg;
	sh_cfg_read.slv_len = (uint8_t)len;

	// Call the command to read the data from the sensor hub.
	// This data will be read from the device connected to the 
	// sensor hub, and saved into a register for us to read.
	ret = lsm6dso_sh_slv0_cfg_read(&dev_ctx, &sh_cfg_read);

	// Using slave 0 only
	lsm6dso_sh_slave_connected_set(&dev_ctx, LSM6DSO_SLV_0);

	/* Enable I2C Master */
	lsm6dso_sh_master_set(&dev_ctx, PROPERTY_ENABLE);

	/* Enable accelerometer to trigger Sensor Hub operation. */
	lsm6dso_xl_data_rate_set(&dev_ctx, LSM6DSO_XL_ODR_104Hz);

	/* Wait Sensor Hub operation flag set. */
	lsm6dso_acceleration_raw_get(&dev_ctx, buf_raw);
	do {
		HAL_Delay(20);
		lsm6dso_xl_flag_data_ready_get(&dev_ctx, &drdy);
	} while (!drdy);

	do {
		HAL_Delay(20);
		lsm6dso_sh_status_get(&dev_ctx, &master_status);
	} while (!master_status.sens_hub_endop);

	/* Disable I2C master and XL(trigger). */
	lsm6dso_sh_master_set(&dev_ctx, PROPERTY_DISABLE);
	lsm6dso_xl_data_rate_set(&dev_ctx, LSM6DSO_XL_ODR_OFF);

	// Read the data from the device
	lsm6dso_sh_read_data_raw_get(&dev_ctx, data, (uint8_t)len);

#ifdef ENABLE_READ_WRITE_DEBUG
	Log_Debug("Read %d bytes: ", len);
	for (int i = 0; i < len; i++) {
		Log_Debug("[%0x] ", data[i]);
	}
	Log_Debug("\n", len);
#endif 

	/* Re-enable accelerometer */
	lsm6dso_xl_data_rate_set(&dev_ctx, LSM6DSO_XL_ODR_104Hz);

	return ret;
}

