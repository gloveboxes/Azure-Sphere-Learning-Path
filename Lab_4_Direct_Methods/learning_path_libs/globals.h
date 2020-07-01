#pragma once

#include <signal.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#define SCOPEID_LENGTH 20
#define RT_APP_COMPONENT_LENGTH 36 + 1  // GUID 36 Char + 1 NULL terminate)

#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))

extern char scopeId[SCOPEID_LENGTH]; // ScopeId for the Azure IoT Central application, set in app_manifest.json, CmdArgs
extern char rtAppComponentId[RT_APP_COMPONENT_LENGTH];  //initialized from cmdline argument

//extern volatile sig_atomic_t terminationRequired;
extern bool realTelemetry;		// flag for real or fake telemetry
void lp_processCmdArgs(int argc, char* argv[]);
char* lp_getCurrentUtc(char* buffer, size_t bufferSize);
