#ifndef inter_core_h
#define inter_core_h

#include "epoll_timerfd_utilities.h"
#include "globals.h"
#include "terminate.h"
#include <applibs/application.h>
#include <applibs/log.h>
#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include "timer.h"

bool ProcessMsg(void);
bool SendInterCoreMessage(const char* msg);
int EnableInterCoreCommunications(const char* rtAppComponentId, void (*interCoreCallback)(char*));

#endif