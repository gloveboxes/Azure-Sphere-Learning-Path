#include "timer.h"

LP_TIMER** _timers = NULL;
size_t _timerCount = 0;
EventLoop* eventLoop = NULL;


EventLoop* lp_getTimerEventLoop(void) {
	if (eventLoop == NULL) {
		eventLoop = EventLoop_Create();
	}
	return eventLoop;
}

bool lp_changeTimer(LP_TIMER* timer, const struct timespec* period) {
	if (timer->eventLoopTimer == NULL) { return false; }
	timer->period.tv_nsec = period->tv_nsec;
	timer->period.tv_sec = period->tv_sec;
	int result = SetEventLoopTimerPeriod(timer->eventLoopTimer, period);

	return result == 0 ? true : false;
}

bool lp_startTimer(LP_TIMER* timer) {
	EventLoop* eventLoop = lp_getTimerEventLoop();
	if (eventLoop == NULL) {
		return false;
	}

	if (timer->eventLoopTimer != NULL) {
		return true;
	}

	if (timer->period.tv_nsec == 0 && timer->period.tv_sec == 0) {  // Set up a disabled LP_TIMER for oneshot or change timer
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

void lp_stopTimer(LP_TIMER* timer) {
	if (timer->eventLoopTimer != NULL) {
		DisposeEventLoopTimer(timer->eventLoopTimer);
		timer->eventLoopTimer = NULL;
	}
}

void lp_startTimerSet(LP_TIMER* timerSet[], size_t timerCount) {
	_timers = timerSet;
	_timerCount = timerCount;

	for (int i = 0; i < _timerCount; i++) {
		if (!lp_startTimer(_timers[i])) {
			break;
		};
	}
}

void lp_stopTimerSet(void) {
	for (int i = 0; i < _timerCount; i++) {
		lp_stopTimer(_timers[i]);
	}
}

void lp_stopTimerEventLoop(void) {
	EventLoop* eventLoop = lp_getTimerEventLoop();
	if (eventLoop != NULL) {
		EventLoop_Close(eventLoop);
	}
}

bool lp_setOneShotTimer(LP_TIMER* timer, const struct timespec* period) {
	EventLoop* eventLoop = lp_getTimerEventLoop();
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