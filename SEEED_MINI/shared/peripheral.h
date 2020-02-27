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

struct _peripheral {
	int fd;
	int pin;
	GPIO_Value initialState;
	bool invertPin;
	int (*initialise)(struct _peripheral* peripheral);
	char* name;
};

typedef struct _peripheral Peripheral;


int OpenPeripheral(Peripheral* peripheral);
void OpenPeripheralSet(Peripheral** peripherals, size_t peripheralCount);
void ClosePeripheralSet(void);
void CloseFdAndPrintError(int fd, const char* fdName);
