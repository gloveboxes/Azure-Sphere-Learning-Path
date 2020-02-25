#include "timer.h"

int epollFd = -1;

int GetEpollFd(void) {
	if (epollFd == -1) {  // lazy create epollFd
		epollFd = CreateEpollFd();
		if (epollFd < 0) {
			return -1;
		}
	}
	return epollFd;
}

bool StartTimer(Timer* timer) {
	int epollId = GetEpollFd();

	if (epollFd < 0) { 
		return false; 
	}

	timer->fd = CreateTimerFdAndAddToEpoll(epollId, &timer->period, &timer->eventData, EPOLLIN);
	if (timer->fd < 0) {
		return false;
	}
	return true;
}

void StopTimer(Timer* timer) {
	if (timer->fd > 0) {
		CloseFdAndPrintError(timer->fd, timer->name);
	}
}
