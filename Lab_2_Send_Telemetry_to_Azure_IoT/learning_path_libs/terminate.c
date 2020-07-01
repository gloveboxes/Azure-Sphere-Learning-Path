#include "terminate.h"

volatile sig_atomic_t terminationRequired = false;
volatile sig_atomic_t _exitCode = 0;

void lp_registerTerminationHandler(void) {
	struct sigaction action;
	memset(&action, 0, sizeof(struct sigaction));
	action.sa_handler = lp_terminationHandler;
	sigaction(SIGTERM, &action, NULL);
}

void lp_terminationHandler(int signalNumber) {
	// Don't use Log_Debug here, as it is not guaranteed to be async-signal-safe.
	terminationRequired = true;
	_exitCode = ExitCode_TermHandler_SigTerm;
}

void lp_terminate(int exitCode) {
	_exitCode = exitCode;
	terminationRequired = true;
}

bool lp_isTerminationRequired(void) {
	return terminationRequired;
}

int lp_getTerminationExitCode(void) {
	return _exitCode;
}
