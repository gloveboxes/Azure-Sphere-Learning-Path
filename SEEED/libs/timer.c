#include "timer.h"

Timer** _timers = NULL;
size_t _timerCount = 0;
EventLoop* eventLoop = NULL;


EventLoop* GetTimerEventLoop(void) {
	if (eventLoop == NULL) {
		eventLoop = EventLoop_Create();
	}
	return eventLoop;
}

bool ChangeTimer(Timer* timer, const struct timespec* period) {
	if (timer->eventLoopTimer == NULL) { return false; }
	timer->period.tv_nsec = period->tv_nsec;
	timer->period.tv_sec = period->tv_sec;
	int result = SetEventLoopTimerPeriod(timer->eventLoopTimer, period);

	return result == 0 ? true : false;
}

bool StartTimer(Timer* timer) {
	EventLoop* eventLoop = GetTimerEventLoop();
	if (eventLoop == NULL) {
		return false;
	}

	if (timer->eventLoopTimer != NULL) {
		return true;
	}

	if (timer->period.tv_nsec == 0 && timer->period.tv_sec == 0) {  // Set up a disabled Timer for oneshot or change timer
		timer->eventLoopTimer = CreateEventLoopDisarmedTimer(eventLoop, timer->handler);
		if (timer->eventLoopTimer == NULL) {
			return false;
		}
	}
	else {
		timer->eventLoopTimer = CreateEventLoopPeriodicTimer(eventLoop, timer->handler, &timer->period);
		if (timer->eventLoopTimer == NULL) {
			return false;
		}
	}


	return true;
}

void StopTimer(Timer* timer) {
	if (timer->eventLoopTimer != NULL) {
		DisposeEventLoopTimer(timer->eventLoopTimer);
		timer->eventLoopTimer = NULL;
	}
}

void StartTimerSet(Timer* timerSet[], size_t timerCount) {
	_timers = timerSet;
	_timerCount = timerCount;

	for (int i = 0; i < _timerCount; i++) {
		if (!StartTimer(_timers[i])) {
			break;
		};
	}
}

void StopTimerSet(void) {
	for (int i = 0; i < _timerCount; i++) {
		StopTimer(_timers[i]);
	}
}

void StopTimerEventLoop(void) {
	EventLoop* eventLoop = GetTimerEventLoop();
	if (eventLoop != NULL) {
		EventLoop_Close(eventLoop);
	}
}

bool SetOneShotTimer(Timer* timer, const struct timespec* period) {
	EventLoop* eventLoop = GetTimerEventLoop();
	if (eventLoop == NULL) {
		return false;
	}

	if (timer->eventLoopTimer == NULL) {
		return false;
	}

	if (SetEventLoopTimerOneShot(timer->eventLoopTimer, period) != 0) {
		return false;
	}

	return true;
}