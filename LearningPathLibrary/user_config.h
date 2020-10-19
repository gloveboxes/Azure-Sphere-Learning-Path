#pragma once

#include "terminate.h"
#include <applibs/log.h>
#include <ctype.h>
#include <getopt.h>
#include <string.h>

/// <summary>
/// Connection types to use when connecting to the Azure IoT Hub.
/// </summary>
typedef enum {
    ConnectionType_NotDefined = 0,
    ConnectionType_DPS = 1,
    ConnectionType_Direct = 2
} ConnectionType;

typedef struct {
    char* scopeId;                                      // ID Scope for DPS.
    char* connectionString;                             // Fallback to Connection string (not recommended)
    char* rtComponentId;                                // Real time app component id
    ConnectionType connectionType;
} LP_USER_CONFIG;

LP_USER_CONFIG lp_userConfig;

void lp_parseCommandLineArguments(int argc, char* argv[]);
bool lp_validateUserConfiguration(void);
