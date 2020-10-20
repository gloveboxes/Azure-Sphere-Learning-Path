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
#include "intercore_contract.h"

bool lp_interCoreSendMessage(LP_INTER_CORE_BLOCK* control_block, size_t len);
int lp_interCoreCommunicationsEnable(const char* rtAppComponentId, void (*interCoreCallback)(LP_INTER_CORE_BLOCK*));
