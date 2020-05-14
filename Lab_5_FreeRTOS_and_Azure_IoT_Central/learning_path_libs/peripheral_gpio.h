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

typedef enum {
	LP_DIRECTION_UNKNOWN,
	LP_INPUT,
	LP_OUTPUT
} Direction;

struct _peripheralGpio {
	int fd;
	int pin;
	GPIO_Value initialState;
	bool invertPin;
	bool (*initialise)(struct _peripheralGpio* peripheralGpio);
	char* name;
	Direction direction;
	bool opened;
};

typedef struct _peripheralGpio LP_PERIPHERAL_GPIO;


bool lp_openPeripheralGpio(LP_PERIPHERAL_GPIO* peripheral);
void lp_openPeripheralGpioSet(LP_PERIPHERAL_GPIO** peripheralSet, size_t peripheralSetCount);
void lp_closePeripheralGpioSet(void);
void lp_closePeripheralGpio(LP_PERIPHERAL_GPIO* peripheral);
void lp_gpioOn(LP_PERIPHERAL_GPIO* peripheral);
void lp_gpioOff(LP_PERIPHERAL_GPIO* peripheral);
