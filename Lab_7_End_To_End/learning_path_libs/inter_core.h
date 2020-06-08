#pragma once

#include "eventloop_timer_utilities.h"
#include "terminate.h"
#include <applibs/application.h>
#include <applibs/eventloop.h>
#include <applibs/log.h>
#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include "timer.h"

enum LP_INTER_CORE_CMD
{
	LP_IC_UNKNOWN,
	LP_IC_HEARTBEAT,
	LP_IC_TEMPERATURE_PRESSURE_HUMIDITY,
	LP_IC_EVENT_BUTTON_A,
	LP_IC_EVENT_BUTTON_B,
	LP_IC_SET_DESIRED_TEMPERATURE,
	LP_IC_BLINK_RATE
};

typedef struct LP_INTER_CORE_BLOCK
{
	enum LP_INTER_CORE_CMD cmd;
	float	temperature;
	float	pressure;
	float	humidity;
	int		blinkRate;

} LP_INTER_CORE_BLOCK;


bool lp_sendInterCoreMessage(LP_INTER_CORE_BLOCK* control_block, size_t len);
int lp_enableInterCoreCommunications(const char* rtAppComponentId, void (*interCoreCallback)(LP_INTER_CORE_BLOCK*));
