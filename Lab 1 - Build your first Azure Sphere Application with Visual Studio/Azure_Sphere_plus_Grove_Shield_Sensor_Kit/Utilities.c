#include "utilities.h"

void RegisterTerminationHandler() {
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

int StartTimer(Timer* timer, int epollFd) {
	timer->fd = CreateTimerFdAndAddToEpoll(epollFd, &timer->period, &timer->eventData, EPOLLIN);
	if (timer->fd < 0) {
		return -1;
	}
	return 0;
}


int OpenPeripheral(Peripheral* peripheral) {
	peripheral->fd = GPIO_OpenAsOutput(peripheral->pin, GPIO_OutputMode_PushPull, peripheral->initialState);
	if (peripheral->fd < 0) {
		Log_Debug(
			"Error opening GPIO: %s (%d). Check that app_manifest.json includes the GPIO used.\n",
			strerror(errno), errno);
		return -1;
	}
	return 0;
}