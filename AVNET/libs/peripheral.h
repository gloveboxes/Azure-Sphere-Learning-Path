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
	DIRECTION_UNKNOWN,
	INPUT,
	OUTPUT
} Direction;

struct _peripheral {
	int fd;
	int pin;
	GPIO_Value initialState;
	bool invertPin;
	bool (*initialise)(struct _peripheral* peripheral);
	char* name;
	Direction direction;
	bool opened;
};

typedef struct _peripheral Peripheral;


bool OpenPeripheral(Peripheral* peripheral);
void OpenPeripheralSet(Peripheral** peripherals, size_t peripheralCount);
void ClosePeripheralSet(void);
void ClosePeripheral(Peripheral* peripheral);
void Gpio_On(Peripheral* peripheral);
void Gpio_Off(Peripheral* peripheral);
