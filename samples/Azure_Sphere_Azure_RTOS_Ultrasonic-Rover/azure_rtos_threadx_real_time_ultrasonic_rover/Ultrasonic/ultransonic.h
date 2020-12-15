#pragma once

#include <math.h>
#include <stdint.h>
#include <stdbool.h>
#include "os_hal_gpio.h"


static const uintptr_t GPT_BASE = 0x21030000;

float lp_get_distance(u8 pin, unsigned long timeoutMicroseconds);