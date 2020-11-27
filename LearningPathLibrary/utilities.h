#pragma once

#include <applibs/log.h>
#include <applibs/networking.h>
#include <stdbool.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <applibs/application.h>

#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))

bool lp_isNetworkReady(void);
bool lp_isDeviceAuthReady(void);
char* lp_getCurrentUtc(char* buffer, size_t bufferSize);
