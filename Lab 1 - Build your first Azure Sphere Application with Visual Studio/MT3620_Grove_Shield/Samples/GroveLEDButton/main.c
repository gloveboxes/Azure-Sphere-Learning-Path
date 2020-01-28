#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <applibs/log.h>
#include <applibs/gpio.h>

#include "../../MT3620_Grove_Shield_Library/Grove.h"
#include "../../MT3620_Grove_Shield_Library/Sensors/GroveLEDButton.h"


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
	static GPIO_Value_Type btn_sta, last_btn_sta;
    Log_Debug("Application starting\n");

    // Register a SIGTERM handler for termination requests
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = TerminationHandler;
    sigaction(SIGTERM, &action, NULL);

	void *btn = GroveLEDButton_Init(1, 0);
	last_btn_sta = GroveLEDButton_GetBtnState(btn);

    // Main loop
    const struct timespec sleepTime = {0, 1000};
    while (!terminationRequested) {

		btn_sta = GroveLEDButton_GetBtnState(btn);
		
		if (btn_sta != last_btn_sta) {
			if (btn_sta == 0) {
				GroveLEDButton_LedOn(btn);
				Log_Debug("Button pressed.\n");
			}
			else {
				GroveLEDButton_LedOff(btn);
				Log_Debug("Button released.\n");
			}
		}
		last_btn_sta = btn_sta;

        nanosleep(&sleepTime, NULL);
    }

    Log_Debug("Application exiting\n");
    return 0;
}
