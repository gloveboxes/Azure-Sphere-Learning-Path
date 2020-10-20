#pragma once

//#include "epoll_timerfd_utilities.h"
#include "parson.h"
#include <applibs/gpio.h>
#include <applibs/log.h>
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "terminate.h"

typedef enum
{
	LP_DIRECTION_UNKNOWN,
	LP_INPUT,
	LP_OUTPUT
} LP_GPIO_DIRECTION;

struct _lp_gpio
{
	int fd;
	int pin;
	GPIO_Value initialState;
	bool invertPin;
	// bool (*initialise)(struct _lp_gpio* gpio);
	char* name;
	LP_GPIO_DIRECTION direction;
	bool opened;
};

typedef struct _lp_gpio LP_GPIO;

bool lp_gpioOpen(LP_GPIO* gpio);
bool lp_gpioStateGet(LP_GPIO* gpio, GPIO_Value_Type* oldState);
void lp_gpioClose(LP_GPIO* gpio);
void lp_gpioOff(LP_GPIO* gpio);
void lp_gpioOn(LP_GPIO* gpio);
void lp_gpioSetClose(LP_GPIO** gpioSet, size_t gpioSetCount);
void lp_gpioSetOpen(LP_GPIO** gpioSet, size_t gpioSetCount);
void lp_gpioStateSet(LP_GPIO* gpio, bool state);
