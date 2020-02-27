#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../Hardware/mt3620_rdb/inc/hw/mt3620_rdb.h"


#define BUILTIN_LED		MT3620_GPIO19
#define LIGHT_PIN		MT3620_GPIO21
#define RELAY_PIN		MT3620_GPIO16

int ReadTelemetry(char* msgBuffer, size_t bufferLen);
bool InitializeDevKit(void);
bool CloseDevKit(void);

