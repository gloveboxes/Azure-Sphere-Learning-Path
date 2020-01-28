#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <applibs/log.h>
#include <applibs/gpio.h>

#include "../../MT3620_Grove_Shield_Library/Grove.h"
#include "../../MT3620_Grove_Shield_Library/Sensors/GroveRelay.h"

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
    static bool state = true;

    Log_Debug("Application starting\n");

    // Register a SIGTERM handler for termination requests
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = TerminationHandler;
    sigaction(SIGTERM, &action, NULL);

    void *relay = GroveRelay_Open(0);

    // Main loop
    const struct timespec sleepTime = {2, 0};
    while (!terminationRequested) {       
        if(state) {
            GroveRelay_On(relay);
			Log_Debug("Relay on\n");
            
        } else {
            GroveRelay_Off(relay);
			Log_Debug("Relay off\n");
        }        
        state = !state;
        
        nanosleep(&sleepTime, NULL);
    }

    Log_Debug("Application exiting\n");
    return 0;
}
