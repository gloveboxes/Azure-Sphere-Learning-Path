#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <applibs/log.h>
#include <applibs/gpio.h>

#include "../../MT3620_Grove_Shield_Library/Grove.h"
#include "../../MT3620_Grove_Shield_Library/Sensors/Grove4DigitDisplay.h"



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
	int number = 1234;

	Log_Debug("Application starting\n");

	// Register a SIGTERM handler for termination requests
	struct sigaction action;
	memset(&action, 0, sizeof(struct sigaction));
	action.sa_handler = TerminationHandler;
	sigaction(SIGTERM, &action, NULL);

	// Initialize Grove 4 Digital Display, use pin_4 as pin_clik, pin_5 as pin_dio
	void *dev = Grove4DigitDisplay_Open(4, 5);

	// Enable clock point
	Grove4DigitDisplay_DisplayClockPoint(true);

	// Main loop
	while (!terminationRequested) {
		// Display increasing number
		Grove4DigitDisplay_DisplayValue(dev, number++);
		usleep(1000000);
	}

	Log_Debug("Application exiting\n");
	return 0;
}
