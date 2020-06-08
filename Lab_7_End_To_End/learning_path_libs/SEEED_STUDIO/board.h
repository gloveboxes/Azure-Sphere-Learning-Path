#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "hw/azure_sphere_learning_path.h"

int lp_readTelemetry(char* msgBuffer, size_t bufferLen);
bool lp_initializeDevKit(void);
bool lp_closeDevKit(void);
