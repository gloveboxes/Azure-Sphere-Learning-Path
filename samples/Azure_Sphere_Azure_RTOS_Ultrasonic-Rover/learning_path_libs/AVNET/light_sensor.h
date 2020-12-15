#pragma once

// applibs_versions.h defines the API struct versions to use for applibs APIs.
//#include "../../../Hardware/avnet_mt3620_sk/inc/hw/avnet_mt3620_sk.h"

#include "hw/azure_sphere_learning_path.h"
#include <applibs/adc.h>
#include <applibs/log.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>


#define ADC_CHANNEL 0

bool lp_OpenADC(void);
int lp_GetLightLevel(void);