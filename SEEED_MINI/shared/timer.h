#pragma once

#include "eventloop_timer_utilities.h"
#include "stdbool.h"
#include <applibs/eventloop.h>

typedef struct {
	void (*timerEventHandler)(EventLoopTimer* timer);
	struct timespec period;
	EventLoopTimer* eventLoopTimer;
	const char* name;
} Timer;

void StartTimerSet(Timer* timers[], size_t timerCount);
void StopTimerSet(void);
bool StartTimer(Timer* timer);
void StopTimer(Timer* timer);
EventLoop* GetTimerEventLoop(void);
void StopTimerEventLoop(void);
