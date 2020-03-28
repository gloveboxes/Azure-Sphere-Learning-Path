#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "imu_temp_pressure.h"
#include "light_sensor.h"

#include "Hardware/avnet_mt3620_sk/inc/hw/azure_sphere_learning_path.h"

int ReadTelemetry(char* msgBuffer, size_t bufferLen);
bool InitializeDevKit(void);
bool CloseDevKit(void);

