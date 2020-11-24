#pragma once

typedef enum
{
	LP_IC_UNKNOWN,
	LP_IC_HEARTBEAT,
	LP_IC_ENVIRONMENT_SENSOR,
	LP_IC_SAMPLE_RATE
} LP_INTER_CORE_CMD;

typedef struct
{
	LP_INTER_CORE_CMD cmd;
	float temperature;
	float pressure;
	float humidity;
	int sample_rate;
} LP_INTER_CORE_BLOCK;
