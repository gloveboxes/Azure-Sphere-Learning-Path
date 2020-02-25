#pragma once

#include <stdbool.h>
#include "epoll_timerfd_utilities.h"

#define LSM6DSO_ID         0x6C   // register value
#define LSM6DSO_ADDRESS	   0x6A	  // I2C Address

bool lps22hhDetected;

int initI2c(void);
void closeI2c(void);