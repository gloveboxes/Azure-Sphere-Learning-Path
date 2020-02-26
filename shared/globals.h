#ifndef models_h
#define models_h

#include <signal.h>
#include <stdbool.h>
#include <string.h>

#define SCOPEID_LENGTH 20
#define RT_APP_COMPONENT_LENGTH 36 + 1  // GUID 36 Char + 1 NULL terminate)

#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))
#define GPIO_ON(x) GPIO_SetValue(x.fd, x.invertPin ? GPIO_Value_Low : GPIO_Value_High)
#define GPIO_OFF(x) GPIO_SetValue(x.fd, x.invertPin ? GPIO_Value_High : GPIO_Value_Low)

extern char scopeId[SCOPEID_LENGTH]; // ScopeId for the Azure IoT Central application, set in app_manifest.json, CmdArgs
extern char rtAppComponentId[RT_APP_COMPONENT_LENGTH];  //initialized from cmdline argument

//extern volatile sig_atomic_t terminationRequired;
extern bool realTelemetry;		// flag for real or fake telemetry
void ProcessCmdArgs(int argc, char* argv[]);

/// <summary>
/// Exit codes for this application. These are used for the
/// application exit code.  They must all be between zero and 255,
/// where zero is reserved for successful termination.
/// </summary>
typedef enum {
    ExitCode_Success = 0,
    ExitCode_TermHandler_SigTerm = 1,
    ExitCode_Main_EventLoopFail = 2,
    ExitCode_ButtonTimer_Consume = 3,
    ExitCode_AzureTimer_Consume = 4,
    ExitCode_Init_EventLoop = 5,
    ExitCode_Init_MessageButton = 6,
    ExitCode_Init_OrientationButton = 7,
    ExitCode_Init_TwinStatusLed = 8,
    ExitCode_Init_ButtonPollTimer = 9,
    ExitCode_Init_AzureTimer = 10,
    ExitCode_IsButtonPressed_GetValue = 11
} ExitCode;

#endif