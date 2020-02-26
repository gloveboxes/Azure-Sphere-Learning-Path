#pragma once

//#include "epoll_timerfd_utilities.h"
#include "../shared/eventloop_timer_utilities.h"
#include "stdbool.h"
#include <applibs/eventloop.h>

typedef struct {
	void (*timerEventHandler)(EventLoopTimer* timer);
	struct timespec period;
	EventLoopTimer* eventLoopTimer;
	const char* name;
} Timer;

void RegisterTimerSet(Timer* timers[], size_t timerCount);
void CloseTimerSet(void);
bool StartTimer(Timer* timer);
void StopTimer(Timer* timer);
EventLoop* GetTimerEventLoop(void);
void CloseTimerEventLoop(void);
