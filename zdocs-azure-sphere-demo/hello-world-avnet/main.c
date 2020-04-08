#include "../libs/azure_iot.h"
#include "../libs/globals.h"
#include "../libs/peripheral.h"
#include "../libs/terminate.h"
#include "../libs/timer.h"
#include "../oem/board.h"
#include "applibs_versions.h"
#include "exit_codes.h"
#include <applibs/gpio.h>
#include <applibs/log.h>
#include <applibs/powermanagement.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

// Number of bytes to allocate for the JSON telemetry message for IoT Central
#define JSON_MESSAGE_BYTES 128  

// Forward signatures
static void ReadSensorHandler(EventLoopTimer* eventLoopTimer);
static void led1ControlHandler(DeviceTwinBinding* deviceTwinBinding);

// Variables
static char msgBuffer[JSON_MESSAGE_BYTES] = { 0 };



// Sets
static Timer* timerSet[] = {  };
static Peripheral* peripheralSet[] = { };
static DeviceTwinBinding* deviceTwinBindingSet[] = {  };





static int InitPeripheralsAndHandlers(void) {
	InitializeDevKit();

	OpenPeripheralSet(peripheralSet, NELEMS(peripheralSet));
	OpenDeviceTwinSet(deviceTwinBindingSet, NELEMS(deviceTwinBindingSet));
	StartTimerSet(timerSet, NELEMS(timerSet));
	StartCloudToDevice();

	return 0;
}

static void ClosePeripheralsAndHandlers(void) {
	Log_Debug("Closing file descriptors\n");

	StopTimerSet();
	StopCloudToDevice();

	ClosePeripheralSet();
	CloseDeviceTwinSet();

	CloseDevKit();

	StopTimerEventLoop();
}

int main(int argc, char* argv[]) {
	RegisterTerminationHandler();
	ProcessCmdArgs(argc, argv);

	if (strlen(scopeId) == 0) {
		Log_Debug("ScopeId needs to be set in the app_manifest CmdArgs\n");
		return ExitCode_Missing_ID_Scope;
	}

	if (InitPeripheralsAndHandlers() != 0) {
		return ExitCode_Init_Failed;
	}

	// Main loop
	while (!IsTerminationRequired()) {

		int result = EventLoop_Run(GetTimerEventLoop(), -1, true);

		// Continue if interrupted by signal, e.g. due to breakpoint being set.
		if (result == -1 && errno != EINTR) {
			Terminate();
		}
	}

	ClosePeripheralsAndHandlers();

	Log_Debug("Application exiting.\n");
	return GetTerminationExitCode();
}