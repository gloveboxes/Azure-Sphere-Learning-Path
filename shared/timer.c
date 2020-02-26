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

bool StartTimer(Timer* timer) {
	EventLoop* eventLoop = GetTimerEventLoop();
	if (eventLoop == NULL) {
		return false;
	}
	else {
		timer->eventLoopTimer = CreateEventLoopPeriodicTimer(eventLoop, timer->timerEventHandler, &timer->period);
		if (timer->eventLoopTimer == NULL) {
			return false;
		}
	}	

	return true;
}

void StopTimer(Timer* timer) {
	if (timer->eventLoopTimer != NULL) {
		DisposeEventLoopTimer(timer->eventLoopTimer);
	}
}

void RegisterTimerSet(Timer* timers[], size_t timerCount) {
	_timers = timers;
	_timerCount = timerCount;

	for (int i = 0; i < _timerCount; i++) {
		if (!StartTimer(_timers[i])) {
			break;
		};
	}
}

void CloseTimerSet(void) {
	for (int i = 0; i < _timerCount; i++) {
		StopTimer(_timers[i]);
	}
}

void CloseTimerEventLoop(void) {
	EventLoop* eventLoop = GetTimerEventLoop();
	if (eventLoop != NULL) {
		EventLoop_Close(eventLoop);
	}
}