#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "hw/azure_sphere_learning_path.h"

typedef struct
{
	float temperature;
	float humidity;
	float pressure;
	int light;
} LP_ENVIRONMENT;

bool lp_readTelemetry(LP_ENVIRONMENT* environment);
bool lp_initializeDevKit(void);
bool lp_closeDevKit(void);
