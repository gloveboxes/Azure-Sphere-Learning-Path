#ifndef models_h
#define models_h

#include "epoll_timerfd_utilities.h"
#include <applibs/gpio.h>
#include <signal.h>
#include <stdbool.h>

#define SCOPEID_LENGTH 20
#define RT_APP_COMPONENT_LENGTH 36 + 1  // GUID 36 Char + 1 NULL terminate)

#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))
#define START_TIMER_SET(x) for (int i = 0; i < NELEMS(x); i++) { StartTimer(x[i]); }
#define STOP_TIMER_SET(x) for (int i = 0; i < NELEMS(x); i++) { CloseFdAndPrintError(x[i]->fd, x[i]->name); }
#define GPIO_ON(x) GPIO_SetValue(x.fd, x.invertPin ? GPIO_Value_Low : GPIO_Value_High)
#define GPIO_OFF(x) GPIO_SetValue(x.fd, x.invertPin ? GPIO_Value_High : GPIO_Value_Low)

extern char scopeId[SCOPEID_LENGTH]; // ScopeId for the Azure IoT Central application, set in app_manifest.json, CmdArgs
extern char rtAppComponentId[RT_APP_COMPONENT_LENGTH];  //initialized from cmdline argument

extern volatile sig_atomic_t terminationRequired;
extern bool realTelemetry;		// Generate fake telemetry
extern enum DevKit deviceKit;

typedef struct {
	EventData eventData;
	struct timespec period;
	int fd;
	const char* name;
} Timer;

#endif