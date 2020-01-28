#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <applibs/log.h>
#include <applibs/gpio.h>

#include "../../MT3620_Grove_Shield_Library/Grove.h"
#include "../../MT3620_Grove_Shield_Library/Sensors/GroveLightSensor.h"
#include "../../MT3620_Grove_Shield_Library/Sensors/GroveAD7992.h"


// This C application for the MT3620 Reference Development Board (Azure Sphere)
// outputs a string every second to Visual Studio's Device Output window
//
// It uses the API for the following Azure Sphere application libraries:
// - log (messages shown in Visual Studio's Device Output window during debugging)

static volatile sig_atomic_t terminationRequested = false;

/// <summary>
///     Signal handler for termination requests. This handler must be async-signal-safe.
/// </summary>
static void TerminationHandler(int signalNumber)
{
    // Don't use Log_Debug here, as it is not guaranteed to be async signal safe
    terminationRequested = true;
}

/// <summary>
///     Main entry point for this sample.
/// </summary>
int main(int argc, char *argv[])
{
    Log_Debug("Application starting\n");

    // Register a SIGTERM handler for termination requests
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = TerminationHandler;
    sigaction(SIGTERM, &action, NULL);

	// Initialize Grove Shield
	int i2cFd;
	GroveShield_Initialize(&i2cFd, 115200);

	// Initialize Light Sensor
	void *light = GroveLightSensor_Init(i2cFd, 0);

    // Main loop
    const struct timespec sleepTime = {1, 0};
    while (!terminationRequested) {
		float value = GroveLightSensor_Read(light);
		value = GroveAD7992_ConvertToMillisVolt(value);
        Log_Debug("Light value %dmV\n", (uint16_t)value);

        nanosleep(&sleepTime, NULL);
    }

    Log_Debug("Application exiting\n");
    return 0;
}
