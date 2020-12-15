#pragma once

#include <stdint.h>
#include "lsm6dso_reg.h"
#include "os_hal_i2c.h"

/* I2C */
#define I2C_MAX_LEN 64
static const uint8_t i2c_port_num = OS_HAL_I2C_ISU2;
static const uint8_t i2c_speed = I2C_SCL_1000kHz;
static const uint8_t i2c_lsm6dso_addr = LSM6DSO_I2C_ADD_L >> 1;


int32_t i2c_write(int* fD, uint8_t reg, uint8_t* buf, uint16_t len);
int32_t i2c_read(int* fD, uint8_t reg, uint8_t* buf, uint16_t len);
void i2c_enum(void);
int i2c_init(void);