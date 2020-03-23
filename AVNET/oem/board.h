#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "imu_temp_pressure.h"
#include "light_sensor.h"

#include "Hardware/avnet_mt3620_sk/inc/hw/azure_sphere_learning_path.h"

#define BUILTIN_LED		AVNET_MT3620_SK_APP_STATUS_LED_YELLOW
#define LIGHT_PIN		AVNET_MT3620_SK_WLAN_STATUS_LED_YELLOW
#define RELAY_PIN		AVNET_MT3620_SK_GPIO0

int ReadTelemetry(char* msgBuffer, size_t bufferLen);
bool InitializeDevKit(void);
bool CloseDevKit(void);

