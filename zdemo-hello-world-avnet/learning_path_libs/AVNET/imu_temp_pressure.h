#pragma once

#include "hw/azure_sphere_learning_path.h"
#include "lps22hh_reg.h"
#include "lsm6dso_reg.h"
#include <applibs/gpio.h>
#include <applibs/i2c.h>
#include <applibs/log.h>
#include <errno.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

// applibs_versions.h defines the API struct versions to use for applibs APIs.
//#include "applibs_versions.h"

#define LSM6DSO_ID         0x6C   // register value
#define LSM6DSO_ADDRESS	   0x6A	  // I2C Address

typedef struct{
	float x;
	float y;
	float z;
} AngularRateDegreesPerSecond;

typedef struct {
	float x;
	float y;
	float z;
} AccelerationMilligForce;

bool AvnetSkSensorUpdate(void);
float GetTemperature(void);
float GetPressure(void);
int initI2c(void);
void closeI2c(void);
AngularRateDegreesPerSecond GetAngularRate(void);
AccelerationMilligForce GetAcceleration(void);