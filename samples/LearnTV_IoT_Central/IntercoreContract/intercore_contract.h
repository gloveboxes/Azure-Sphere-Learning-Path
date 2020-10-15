#pragma once

typedef enum 
{
	LP_IC_UNKNOWN,
	LP_IC_HEARTBEAT,
	LP_IC_TEMPERATURE_PRESSURE_HUMIDITY,
} LP_INTER_CORE_CMD;

typedef struct
{
	LP_INTER_CORE_CMD cmd;
	float temperature;
	float pressure;
	float humidity;
} LP_INTER_CORE_BLOCK;