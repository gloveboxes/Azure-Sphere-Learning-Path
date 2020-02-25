#include "terminate.h"

volatile sig_atomic_t terminationRequired = false;

void RegisterTerminationHandler(void) {
	struct sigaction action;
	memset(&action, 0, sizeof(struct sigaction));
	action.sa_handler = TerminationHandler;
	sigaction(SIGTERM, &action, NULL);
}


void TerminationHandler(int signalNumber)
{
	// Don't use Log_Debug here, as it is not guaranteed to be async-signal-safe.
	terminationRequired = true;
}

void Terminate(void) {
	terminationRequired = true;
}

bool IsTerminationRequired(void) {
	return terminationRequired;
}


