#ifndef peripheral_h
#define peripheral_h

#include "epoll_timerfd_utilities.h"
#include "parson.h"
#include <applibs/gpio.h>
#include <applibs/log.h>
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
	TYPE_INT = 0,
	TYPE_FLOAT = 1,
	TYPE_BOOL = 2,
	TYPE_STRING = 3
} valueType;

struct _peripheral {
	int fd;
	int pin;
	GPIO_Value initialState;
	bool invertPin;
	int (*initialise)(struct _peripheral* peripheral);
	char* name;
};

typedef struct _peripheral Peripheral;

struct _deviceTwinPeripheral {
	Peripheral peripheral;
	const char* twinProperty;
	void* twinState;
	valueType twinType;
	void (*handler)(struct _deviceTwinPeripheral* deviceTwinPeripheral);
};

typedef struct _deviceTwinPeripheral DeviceTwinPeripheral;

struct _directMethodPeripheral {
	Peripheral peripheral;
	const char* methodName;
	enum DirectMethodResponseCode(*handler)(JSON_Object* json, struct _directMethodPeripheral* peripheral);
	char* responseMessage;
};

typedef struct _directMethodPeripheral DirectMethodPeripheral;

typedef struct {
	Peripheral peripheral;
} ActuatorPeripheral;

#define OPEN_PERIPHERAL(x) if (x.peripheral.initialise != NULL) {x.peripheral.initialise(&x.peripheral);}
#define OPEN_PERIPHERAL_SET(x) 	for (int i = 0; i < NELEMS(x); i++) {if (x[i]->peripheral.initialise != NULL) { x[i]->peripheral.initialise(&x[i]->peripheral);}}
#define CLOSE_PERIPHERAL_SET(x) for (int i = 0; i < NELEMS(x); i++) { CloseFdAndPrintError(x[i]->peripheral.fd, x[i]->peripheral.name); }

#define OPEN_DEVICE_TWIN_SET(x) for (int i = 0; i < NELEMS(x); i++) { OpenDeviceTwin(x[i]); }
#define CLOSE_DEVICE_TWIN_SET(x) for (int i = 0; i < NELEMS(x); i++) { CloseDeviceTwin(x[i]); }

int OpenPeripheral(Peripheral* peripheral);
void OpenDeviceTwin(DeviceTwinPeripheral* deviceTwinPeripheral);
void CloseDeviceTwin(DeviceTwinPeripheral* deviceTwinPeripheral);

#endif