#pragma once

#include "../../Shared/inc/inter_core_definition.h"
#include "eventloop_timer_utilities.h"
#include "terminate.h"
#include "timer.h"
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

bool lp_sendInterCoreMessage(LP_INTER_CORE_BLOCK* control_block, size_t len);
int lp_enableInterCoreCommunications(char* rtAppComponentId, void (*interCoreCallback)(LP_INTER_CORE_BLOCK*));
