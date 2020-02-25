#ifndef timer_h
#define timer_h

#include "epoll_timerfd_utilities.h"
#include "stdbool.h"

#define START_TIMER_SET(x) for (int i = 0; i < NELEMS(x); i++) { if (!StartTimer(x[i])) { break; }; }
#define STOP_TIMER_SET(x) for (int i = 0; i < NELEMS(x); i++) { StopTimer(x[i]); }

typedef struct {
	EventData eventData;
	struct timespec period;
	int fd;
	const char* name;
} Timer;

bool StartTimer(Timer* timer);
void StopTimer(Timer* timer);
int GetEpollFd(void);

#endif