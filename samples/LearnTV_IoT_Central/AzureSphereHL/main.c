﻿/*
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

 // Hardware definition
#include "hw/azure_sphere_learning_path.h"

// Learning Path Libraries
#include "azure_iot.h"
#include "exit_codes.h"
#include "globals.h"
#include "inter_core.h"
#include "peripheral_gpio.h"
#include "terminate.h"
#include "timer.h"

// System Libraries
#include "applibs_versions.h"
#include <applibs/gpio.h>
#include <applibs/log.h>
#include <applibs/powermanagement.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

#define JSON_MESSAGE_BYTES 256  // Number of bytes to allocate for the JSON telemetry message for IoT Central

// Forward signatures
static void ReadSensorHandler(EventLoopTimer* eventLoopTimer);
static void SampleRateDeviceTwinHandler(LP_DEVICE_TWIN_BINDING* deviceTwinBinding);
static void AzureIotConnectionStatusHandler(EventLoopTimer* eventLoopTimer);
static LP_DIRECT_METHOD_RESPONSE_CODE LightControlDirectMethodHandler(JSON_Value* json, LP_DIRECT_METHOD_BINDING* directMethodBinding, char** responseMsg);


static char msgBuffer[JSON_MESSAGE_BYTES] = { 0 };
static LP_INTER_CORE_BLOCK ic_control_block;
static int msgId = 0;

// Telemetry message template and properties
static const char* msgTemplate = "{ \"Temperature\": \"%3.2f\", \"Pressure\":\"%3.1f\", \"MsgId\":%d }";

static LP_MESSAGE_PROPERTY* msgProperties[] = {
	&(LP_MESSAGE_PROPERTY) { .key = "appid", .value = "lab-monitor" },
	&(LP_MESSAGE_PROPERTY) {.key = "format", .value = "json" },
	&(LP_MESSAGE_PROPERTY) {.key = "type", .value = "telemetry" },
	&(LP_MESSAGE_PROPERTY) {.key = "version", .value = "1" }
};


/***********************************************/
/*****  Declare Timers, Twins and Methods  *****/

// Timer
static LP_TIMER readSensorTimer = {
    .period = {5, 0},
    .name = "readSensorTimer",
    .handler = ReadSensorHandler };

static LP_GPIO relay = {
    .pin = RELAY,
    .direction = LP_OUTPUT,
    .initialState = GPIO_Value_Low,
    .invertPin = false,
    .name = "relay" };

static LP_DIRECT_METHOD_BINDING lightControl = {
    .methodName = "LightControl",
    .handler = LightControlDirectMethodHandler };

static LP_DEVICE_TWIN_BINDING sampleRate_DeviceTwin = {
    .twinProperty = "SampleRateSeconds",
    .twinType = LP_TYPE_INT,
    .handler = SampleRateDeviceTwinHandler };

static LP_TIMER azureIotConnectionStatusTimer = {
	.period = {5, 0},
	.name = "azureIotConnectionStatusTimer",
	.handler = AzureIotConnectionStatusHandler };

static LP_GPIO azureIotConnectedLed = {
	.pin = NETWORK_CONNECTED_LED,
	.direction = LP_OUTPUT,
	.initialState = GPIO_Value_Low,
	.invertPin = true,
	.name = "azureIotConnectedLed" };



/****************************************/
/*****  Initialise collection set  *****/

static LP_TIMER* timerSet[] = { &readSensorTimer, &azureIotConnectionStatusTimer };
static LP_GPIO* gpioSet[] = { &relay, &azureIotConnectedLed };
static LP_DIRECT_METHOD_BINDING* directMethodBindingSet[] = { &lightControl };
static LP_DEVICE_TWIN_BINDING* deviceTwinBindingSet[] = { &sampleRate_DeviceTwin };

/****************************************/
/*****  Demo Code                   *****/

/// <summary>
/// Check status of connection to Azure IoT
/// </summary>
static void AzureIotConnectionStatusHandler(EventLoopTimer* eventLoopTimer)
{
	static bool toggleStatusLed = true;

	if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0) {
		lp_terminate(ExitCode_ConsumeEventLoopTimeEvent);
		return;
	}

	if (lp_azureConnect()) {
		lp_gpioSetState(&azureIotConnectedLed, toggleStatusLed);
		toggleStatusLed = !toggleStatusLed;
	}
	else {
		lp_gpioSetState(&azureIotConnectedLed, false);
	}
}

/// <summary>
/// This handler called when device twin desired 'SampleRateSeconds' recieved
/// </summary>
static void SampleRateDeviceTwinHandler(LP_DEVICE_TWIN_BINDING* deviceTwinBinding)
{
    int sampleRate = *(int*)deviceTwinBinding->twinState;

    if (sampleRate > 0 && sampleRate < (5 * 60)) // check sensible range
    {
        lp_timerChange(&readSensorTimer, &(struct timespec){sampleRate, 0});
        lp_deviceTwinAckDesiredState(deviceTwinBinding, deviceTwinBinding->twinState, LP_DEVICE_TWIN_COMPLETED);
    }
    else {
        lp_deviceTwinAckDesiredState(deviceTwinBinding, deviceTwinBinding->twinState, LP_DEVICE_TWIN_ERROR);
    }
}


/// <summary>
/// Remote control lab light Direct Method 'LightControl' true or false
/// </summary>
static LP_DIRECT_METHOD_RESPONSE_CODE LightControlDirectMethodHandler(JSON_Value* json, LP_DIRECT_METHOD_BINDING* directMethodBinding, char** responseMsg)
{
    if (json_value_get_type(json) != JSONBoolean) { return LP_METHOD_FAILED; }

    bool state = (bool)json_value_get_boolean(json);

    lp_gpioSetState(&relay, state);

    return LP_METHOD_SUCCEEDED;
}

/// <summary>
/// Read sensor and send to Azure IoT
/// </summary>
static void ReadSensorHandler(EventLoopTimer* eventLoopTimer)
{
    if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0) {
        lp_terminate(ExitCode_ConsumeEventLoopTimeEvent);
    }
    else {
        // send request to Real-Time core app to read temperature, pressure, and humidity
        ic_control_block.cmd = LP_IC_ENVIRONMENT_SENSOR;
        lp_sendInterCoreMessage(&ic_control_block, sizeof(ic_control_block));
    }
}

/*****************************************************************************/
/*         Initialise and run code                                           */
/*****************************************************************************/

/// <summary>
/// Callback handler for Inter-Core Messaging - Does Device Twin Update, and Event Message
/// </summary>
static void InterCoreHandler(LP_INTER_CORE_BLOCK* ic_message_block)
{
	switch (ic_message_block->cmd)
	{
	case LP_IC_ENVIRONMENT_SENSOR:
		if (snprintf(msgBuffer, JSON_MESSAGE_BYTES, msgTemplate, ic_message_block->temperature, ic_message_block->pressure, msgId++) > 0)
		{
			Log_Debug(msgBuffer);
			lp_azureMsgSendWithProperties(msgBuffer, msgProperties, NELEMS(msgProperties));
		}
		break;
	default:
		break;
	}
}

/// <summary>
///  Initialize PeripheralGpios, device twins, direct methods, timers.
/// </summary>
static void InitPeripheralAndHandlers(void)
{
	lp_gpioOpenSet(gpioSet, NELEMS(gpioSet));
	lp_deviceTwinOpenSet(deviceTwinBindingSet, NELEMS(deviceTwinBindingSet));
	lp_directMethodOpenSet(directMethodBindingSet, NELEMS(directMethodBindingSet));

	lp_timerStartSet(timerSet, NELEMS(timerSet));
	lp_azureToDeviceStart();

	lp_enableInterCoreCommunications(rtAppComponentId, InterCoreHandler);  // Initialize Inter Core Communications

	lp_timerSetOneShot(&readSensorTimer, &(struct timespec){1, 0});
}

/// <summary>
/// Close PeripheralGpios and handlers.
/// </summary>
static void ClosePeripheralAndHandlers(void)
{
	Log_Debug("Closing file descriptors\n");

	lp_timerStopSet(timerSet, NELEMS(timerSet));
	lp_azureToDeviceStop();

	lp_gpioCloseSet(gpioSet, NELEMS(gpioSet));
	lp_deviceTwinCloseSet();
	lp_directMethodSetClose();

	lp_timerStopEventLoop();
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

	InitPeripheralAndHandlers();

	// Main loop
	while (!lp_isTerminationRequired())
	{
		int result = EventLoop_Run(lp_timerGetEventLoop(), -1, true);
		// Continue if interrupted by signal, e.g. due to breakpoint being set.
		if (result == -1 && errno != EINTR)
		{
			lp_terminate(ExitCode_Main_EventLoopFail);
		}
	}

	ClosePeripheralAndHandlers();

	Log_Debug("Application exiting.\n");
	return lp_getTerminationExitCode();
}