#pragma once

typedef enum 
{
	LP_IC_UNKNOWN,
	LP_IC_HEARTBEAT,
	LP_IC_TEMPERATURE_PRESSURE_HUMIDITY,
	LP_IC_EVENT_BUTTON_A,
	LP_IC_EVENT_BUTTON_B,
	LP_IC_SET_DESIRED_TEMPERATURE,
	LP_IC_BLINK_RATE
} LP_INTER_CORE_CMD;

typedef struct
{
	LP_INTER_CORE_CMD cmd;
	float temperature;
	float pressure;
	float humidity;
	int blinkRate;
} LP_INTER_CORE_BLOCK;