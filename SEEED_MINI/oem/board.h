#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../oem/Hardware/seeed_mt3620_mdb/inc/hw/azure_sphere_learning_path.h"

int ReadTelemetry(char* msgBuffer, size_t bufferLen);
bool InitializeDevKit(void);
bool CloseDevKit(void);
