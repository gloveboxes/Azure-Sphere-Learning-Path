#ifndef inter_core_h
#define inter_core_h

#include "epoll_timerfd_utilities.h"
#include "globals.h"
#include <applibs/application.h>
#include <applibs/log.h>
#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>

bool ProcessMsg(void);
bool SendMessageToRTCore(const char* msg);
int InitInterCoreComms(int epollFd, const char* rtAppComponentId, void (*interCoreCallback)(char*));
void SocketEventHandler(EventData* eventData);

#endif