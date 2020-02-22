#include "utilities.h"

int epollFd = -1;

int GetEpollFd(void) {
	if (epollFd == -1) {  // lazy create epollFd
		epollFd = CreateEpollFd();
		if (epollFd < 0) {
			terminationRequired = true;
			return -1;
		}
	}
	return epollFd;
}


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


int StartTimer(Timer* timer) {
	timer->fd = CreateTimerFdAndAddToEpoll(GetEpollFd(), &timer->period, &timer->eventData, EPOLLIN);
	if (timer->fd < 0) {
		return -1;
	}
	return 0;
}


//int OpenPeripheral(Peripheral* peripheral) {
//	peripheral->fd = GPIO_OpenAsOutput(peripheral->pin, GPIO_OutputMode_PushPull, peripheral->initialState);
//	if (peripheral->fd < 0) {
//		Log_Debug(
//			"Error opening GPIO: %s (%d). Check that app_manifest.json includes the GPIO used.\n",
//			strerror(errno), errno);
//		return -1;
//	}
//	return 0;
//}


void ProcessCmdArgs(int argc, char* argv[]) {
	do {
		argc--;
		switch (argc)
		{
		case 1:
			if (strcmp(argv[1], "Real") == 0) {
				realTelemetry = true;
			}
			break;
		case 2:
			strncpy(scopeId, argv[2], SCOPEID_LENGTH);
			break;
		case 3:
			strncpy(rtAppComponentId, argv[3], RT_APP_COMPONENT_LENGTH);
			break;
		}
	} while (argc > 1);
}