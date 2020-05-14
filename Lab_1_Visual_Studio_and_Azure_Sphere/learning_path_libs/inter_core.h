#pragma once

#include "eventloop_timer_utilities.h"
#include "terminate.h"
#include <applibs/application.h>
#include <applibs/eventloop.h>
#include <applibs/log.h>
#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include "timer.h"


bool lp_sendInterCoreMessage(const char* msg);
int lp_enableInterCoreCommunications(const char* rtAppComponentId, void (*interCoreCallback)(char*));
