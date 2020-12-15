/*
 *   Please read the disclaimer and the developer board selection section below
 *
 *
 *   DISCLAIMER
 *
 *   The learning_path_libs functions provided in the learning_path_libs folder:
 *
 *	   1. are NOT supported Azure Sphere APIs.
 *	   2. are prefixed with lp_, typedefs are prefixed with LP_
 *	   3. are built from the Azure Sphere SDK Samples at https://github.com/Azure/azure-sphere-samples
 *	   4. are not intended as a substitute for understanding the Azure Sphere SDK Samples.
 *	   5. aim to follow best practices as demonstrated by the Azure Sphere SDK Samples.
 *	   6. are provided as is and as a convenience to aid the Azure Sphere Developer Learning experience.
 *
 *
 *   DEVELOPER BOARD SELECTION
 *
 *   The following developer boards are supported.
 *
 *	   1. AVNET Azure Sphere Starter Kit.
 *	   2. Seeed Studio Azure Sphere MT3620 Development Kit aka Reference Design Board or rdb.
 *	   3. Seeed Studio Seeed Studio MT3620 Mini Dev Board.
 *
 *   ENABLE YOUR DEVELOPER BOARD
 *
 *   Each Azure Sphere developer board manufacturer maps pins differently. You need to select the configuration that matches your board.
 *
 *   Follow these steps:
 *
 *	   1. Open CMakeLists.txt.
 *	   2. Uncomment the set command that matches your developer board.
 *	   3. Click File, then Save to save the CMakeLists.txt file which will auto generate the CMake Cache.
 */

#include "hw/azure_sphere_learning_path.h"

#include "azure_iot.h"
#include "exit_codes.h"
#include "globals.h"
#include "inter_core.h"
#include "peripheral_gpio.h"
#include "terminate.h"
#include "timer.h"

#include "applibs_versions.h"
#include <applibs/gpio.h>
#include <applibs/log.h>
#include <applibs/powermanagement.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>


#define JSON_MESSAGE_BYTES 128  // Number of bytes to allocate for the JSON telemetry message for IoT Central

 // Forward signatures
static void InitPeripheralGpiosAndHandlers(void);
static void ClosePeripheralGpiosAndHandlers(void);
static void IntercoreHeartBeatHandler(EventLoopTimer* eventLoopTimer);
static void InterCoreHandler(LP_INTER_CORE_BLOCK* ic_message_block);

static char msgBuffer[JSON_MESSAGE_BYTES] = { 0 };
LP_INTER_CORE_BLOCK ic_control_block;

// Timers
static LP_TIMER intercoreHeartBeatTimer = { .period = { 5, 0 }, .name = "intercoreHeartBeatTimer", .handler = IntercoreHeartBeatHandler };

// Azure IoT Device Twins
static LP_DEVICE_TWIN_BINDING reportDistanceLeft = { .twinProperty = "ReportDistanceLeft", .twinType = LP_TYPE_INT };
static LP_DEVICE_TWIN_BINDING reportDistanceRight = { .twinProperty = "ReportDistanceRight", .twinType = LP_TYPE_INT };

// Initialize Sets
LP_TIMER* timerSet[] = { &intercoreHeartBeatTimer };
LP_DEVICE_TWIN_BINDING* deviceTwinBindingSet[] = { &reportDistanceLeft, &reportDistanceRight };

// Message property set
static LP_MESSAGE_PROPERTY messageLog = { .key = "log", .value = "false" };
static LP_MESSAGE_PROPERTY messageAppId = { .key = "appid", .value = "rover" };
static LP_MESSAGE_PROPERTY* messageProperties[] = { &messageLog, &messageAppId };


static void IntercoreHeartBeatHandler(EventLoopTimer* eventLoopTimer)
{
	if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0)
	{
		lp_terminate(ExitCode_ConsumeEventLoopTimeEvent);
		return;
	}

	ic_control_block.cmd = LP_IC_HEARTBEAT;		// Prime RT Core with Component ID Signature
	lp_sendInterCoreMessage(&ic_control_block, sizeof(ic_control_block));
}


/// <summary>
/// Callback handler for Inter-Core Messaging - Does Device Twin Update, and Event Message
/// </summary>
static void InterCoreHandler(LP_INTER_CORE_BLOCK* ic_message_block)
{
	static const char* msgTemplate = "{ \"DistanceLeft\": %d, \"DistanceRight\": %d, \"MsgId\":%d }";
	static int msgId = 0;

	switch (ic_message_block->cmd)
	{
	case LP_IC_REPORT_DISTANCE:
		lp_deviceTwinReportState(&reportDistanceLeft, &ic_message_block->distance_left);	// TwinType = TYPE_INT
		lp_deviceTwinReportState(&reportDistanceRight, &ic_message_block->distance_right);	// TwinType = TYPE_INT
		break;
	default:
		break;
	}

	if (snprintf(msgBuffer, JSON_MESSAGE_BYTES, msgTemplate, ic_message_block->distance_left, ic_message_block->distance_right, msgId++))
	{
		lp_setMessageProperties(messageProperties, NELEMS(messageProperties));	// send msg with properties
		lp_sendMsg(msgBuffer);
		lp_clearMessageProperties();	//optional: clear if you sending other messages and you don't want these properties
	}
}

/// <summary>
///  Initialize PeripheralGpios, device twins, direct methods, timers.
/// </summary>
/// <returns>0 on success, or -1 on failure</returns>
static void InitPeripheralGpiosAndHandlers(void)
{
	//lp_openPeripheralGpioSet(peripheralGpioSet, NELEMS(peripheralGpioSet));
	lp_openDeviceTwinSet(deviceTwinBindingSet, NELEMS(deviceTwinBindingSet));

	lp_startTimerSet(timerSet, NELEMS(timerSet));
	lp_startCloudToDevice();

	lp_enableInterCoreCommunications(rtAppComponentId, InterCoreHandler);  // Initialize Inter Core Communications

	ic_control_block.cmd = LP_IC_HEARTBEAT;		// Prime RT Core with Component ID Signature
	lp_sendInterCoreMessage(&ic_control_block, sizeof(ic_control_block));
}

/// <summary>
/// Close PeripheralGpios and handlers.
/// </summary>
static void ClosePeripheralGpiosAndHandlers(void)
{
	Log_Debug("Closing file descriptors\n");

	lp_stopTimerSet();
	lp_stopCloudToDevice();

	//lp_closePeripheralGpioSet();
	lp_closeDeviceTwinSet();

	lp_stopTimerEventLoop();
}

int main(int argc, char* argv[])
{
	lp_registerTerminationHandler();
	lp_processCmdArgs(argc, argv);

	if (strlen(scopeId) == 0)
	{
		Log_Debug("ScopeId needs to be set in the app_manifest CmdArgs\n");
		return ExitCode_Missing_ID_Scope;
	}

	InitPeripheralGpiosAndHandlers();

	// Main loop
	while (!lp_isTerminationRequired())
	{
		int result = EventLoop_Run(lp_getTimerEventLoop(), -1, true);
		// Continue if interrupted by signal, e.g. due to breakpoint being set.
		if (result == -1 && errno != EINTR)
		{
			lp_terminate(ExitCode_Main_EventLoopFail);
		}
	}

	ClosePeripheralGpiosAndHandlers();

	Log_Debug("Application exiting.\n");
	return lp_getTerminationExitCode();
}