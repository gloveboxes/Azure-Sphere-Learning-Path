#pragma once

#include "hw/azure_sphere_learning_path.h"
#include "lsm6dso_reg.h"
#include "lps22hh_reg.h"
#include <applibs/i2c.h>
#include <applibs/log.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>

#define LSM6DSO_ADDRESS	   0x6A	  // I2C Address

typedef struct
{
	float x;
	float y;
	float z;
} AngularRateDegreesPerSecond;

typedef struct
{
	float x;
	float y;
	float z;
} AccelerationMilligForce;

void lp_imu_initialize(void);
void lp_imu_close(void);
float lp_get_temperature(void);
float lp_get_pressure(void);
float lp_get_temperature_lps22h(void);	// get_temperature() from lsm6dso is faster
void lp_calibrate_angular_rate(void);
AngularRateDegreesPerSecond lp_get_angular_rate(void);
AccelerationMilligForce lp_get_acceleration(void);
