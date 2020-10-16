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

void lp_timerStartSet(LP_TIMER* timerSet[], size_t timerCount);
void lp_timerStopSet(void);
bool lp_timerStart(LP_TIMER* timer);
void lp_timeStop(LP_TIMER* timer);
EventLoop* lp_timerGetEventLoop(void);
void lp_timerStopEventLoop(void);
bool lp_timerChange(LP_TIMER* timer, const struct timespec* period);
bool lp_timerSetOneShot(LP_TIMER* timer, const struct timespec* delay);