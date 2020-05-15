#pragma once


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
	ExitCode_IsButtonPressed_GetValue = 11,
	ExitCode_Missing_ID_Scope = 12,
	ExitCode_Init_Failed = 13
} ExitCode;