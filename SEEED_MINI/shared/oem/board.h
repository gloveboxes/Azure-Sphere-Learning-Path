#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../Hardware/seeed_mt3620_mdb/inc/hw/seeed_mt3620_mdb.h"


#define BUILTIN_LED		AILINK_WFM620RSC1_PIN16_GPIO35
#define LIGHT_PIN		AILINK_WFM620RSC1_PIN15_GPIO33
#define RELAY_PIN		AILINK_WFM620RSC1_PIN3_GPIO43

int ReadTelemetry(char* msgBuffer, size_t bufferLen);
bool InitializeDevKit(void);
bool CloseDevKit(void);

