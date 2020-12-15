#pragma once

#include "eventloop_timer_utilities.h"
#include "stdbool.h"
#include <applibs/eventloop.h>

typedef struct {
	void (*handler)(EventLoopTimer* timer);
	struct timespec period;
	EventLoopTimer* eventLoopTimer;
	const char* name;
} LP_TIMER;

void lp_startTimerSet(LP_TIMER* timerSet[], size_t timerCount);
void lp_stopTimerSet(void);
bool lp_startTimer(LP_TIMER* timer);
void lp_stopTimer(LP_TIMER* timer);
EventLoop* lp_getTimerEventLoop(void);
void lp_stopTimerEventLoop(void);
bool lp_changeTimer(LP_TIMER* timer, const struct timespec* period);
bool lp_setOneShotTimer(LP_TIMER* timer, const struct timespec* delay);