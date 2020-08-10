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

void imu_initialize(void);
void imu_close(void);
float get_temperature(void);
float get_pressure(void);
AngularRateDegreesPerSecond get_angular_rate(void);
AccelerationMilligForce get_acceleration(void);
