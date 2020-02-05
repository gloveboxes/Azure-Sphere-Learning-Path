#ifndef models_h
#define models_h

#include "epoll_timerfd_utilities.h"
#include <applibs/gpio.h>
#include <signal.h>
#include <stdbool.h>
#include "parson.h"

#define SCOPEID_LENGTH 20
#define RT_APP_COMPONENT_LENGTH 36 + 1  // GUID 36 Char + 1 NULL terminate)

#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))
#define OPEN_PERIPHERAL(x) if (x.peripheral.initialise != NULL) {x.peripheral.initialise(&x.peripheral);}
#define OPEN_PERIPHERAL_SET(x) 	for (int i = 0; i < NELEMS(x); i++) {if (x[i]->peripheral.initialise != NULL) { x[i]->peripheral.initialise(&x[i]->peripheral);}}
#define CLOSE_PERIPHERAL_SET(x) for (int i = 0; i < NELEMS(x); i++) { CloseFdAndPrintError(x[i]->peripheral.fd, x[i]->peripheral.name); }
#define START_TIMER_SET(x) for (int i = 0; i < NELEMS(x); i++) { StartTimer(x[i], epollFd); }
#define STOP_TIMER_SET(x) for (int i = 0; i < NELEMS(x); i++) { CloseFdAndPrintError(x[i]->fd, x[i]->name); }
#define GPIO_ON(x) GPIO_SetValue(x.fd, x.invertPin ? GPIO_Value_Low : GPIO_Value_High)
#define GPIO_OFF(x) GPIO_SetValue(x.fd, x.invertPin ? GPIO_Value_High : GPIO_Value_Low)

extern char scopeId[SCOPEID_LENGTH]; // ScopeId for the Azure IoT Central application, set in app_manifest.json, CmdArgs
static char rtAppComponentId[RT_APP_COMPONENT_LENGTH];  //initialized from cmdline argument

extern volatile sig_atomic_t terminationRequired;
extern bool realTelemetry;		// Generate fake telemetry or use Seeed Studio Grove SHT31 Sensor

struct _peripheral {
	int fd;
	unsigned char pin;
	GPIO_Value initialState;
	bool invertPin;
	int (*initialise)(struct _peripheral* peripheral);
	char* name;
};

typedef struct _peripheral Peripheral;

struct _deviceTwinPeripheral {
	Peripheral peripheral;
	bool twinState;
	const char* twinProperty;
	void (*handler)(JSON_Object* json, struct _deviceTwinPeripheral* deviceTwinPeripheral);
};

typedef struct _deviceTwinPeripheral DeviceTwinPeripheral;

struct _directMethodPeripheral {
	Peripheral peripheral;
	const char* methodName;
	bool (*handler)(JSON_Object* json, struct _directMethodPeripheral* peripheral);
};

typedef struct _directMethodPeripheral DirectMethodPeripheral;

typedef struct {
	Peripheral peripheral;
} ActuatorPeripheral;

typedef struct {
	EventData eventData;
	struct timespec period;
	int fd;
	const char* name;
} Timer;

#endif