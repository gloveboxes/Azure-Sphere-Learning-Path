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

// Hardware specific
#ifdef OEM_AVNET
#include "AVNET/board.h"
#include "AVNET/imu_temp_pressure.h"
#include "AVNET/light_sensor.h"
#endif // OEM_AVNET

// Hardware specific
#ifdef OEM_SEEED_STUDIO
#include "SEEED_STUDIO/board.h"
#endif // SEEED_STUDIO

#define JSON_MESSAGE_BYTES 256  // Number of bytes to allocate for the JSON telemetry message for IoT Central

// Forward signatures
static void MeasureSensorHandler(EventLoopTimer* eventLoopTimer);
static void AzureIoTConnectionStatusHandler(EventLoopTimer* eventLoopTimer);
static void InterCoreHandler(LP_INTER_CORE_BLOCK* ic_message_block);
static void ResetDeviceHandler(EventLoopTimer* eventLoopTimer);
static void DeviceTwinSetTemperatureHandler(LP_DEVICE_TWIN_BINDING* deviceTwinBinding);
static void DeviceTwinSampleRateHandler(LP_DEVICE_TWIN_BINDING* deviceTwinBinding);
static void DeviceTwinRelay1Handler(LP_DEVICE_TWIN_BINDING* deviceTwinBinding);
static LP_DIRECT_METHOD_RESPONSE_CODE ResetDirectMethodHandler(JSON_Value* json, LP_DIRECT_METHOD_BINDING* directMethodBinding, char** responseMsg);

static char msgBuffer[JSON_MESSAGE_BYTES] = { 0 };
static const char cstrJsonEvent[] = "{\"%s\":\"occurred\"}";
LP_INTER_CORE_BLOCK ic_control_block;

static LP_GPIO azureIotConnectedLed = {
	.pin = NETWORK_CONNECTED_LED,
	.direction = LP_OUTPUT,
	.initialState = GPIO_Value_Low,
	.invertPin = true,
	.initialise = lp_gpioOpen,
	.name = "azureIotConnectedLed" };

static LP_GPIO relay1 = {
	.pin = RELAY,
	.direction = LP_OUTPUT,
	.initialState = GPIO_Value_Low,
	.invertPin = false,
	.initialise = lp_gpioOpen,
	.name = "relay1" };

// Timers
static LP_TIMER azureIotConnectionStatusTimer = {
	.period = { 5, 0 },
	.name = "azureIotConnectionStatusTimer",
	.handler = AzureIoTConnectionStatusHandler };

static LP_TIMER measureSensorTimer = {
	.period = { 10, 0 },
	.name = "measureSensorTimer",
	.handler = MeasureSensorHandler };

static LP_TIMER resetDeviceOneShotTimer = {
	.period = { 0, 0 },
	.name = "resetDeviceOneShotTimer",
	.handler = ResetDeviceHandler };

/**** Azure IoT Device Twins ****/
static LP_DEVICE_TWIN_BINDING buttonPressed = {
	.twinProperty = "ButtonPressed",
	.twinType = LP_TYPE_STRING };

static LP_DEVICE_TWIN_BINDING desiredTemperature = {
	.twinProperty = "DesiredTemperature",
	.twinType = LP_TYPE_FLOAT,
	.handler = DeviceTwinSetTemperatureHandler };

static LP_DEVICE_TWIN_BINDING desiredSampleRate = {
	.twinProperty = "DesiredSampleRate",
	.twinType = LP_TYPE_INT,
	.handler = DeviceTwinSampleRateHandler };

static LP_DEVICE_TWIN_BINDING relay1DeviceTwin = {
	.twinProperty = "Relay1",
	.twinType = LP_TYPE_BOOL,
	.handler = DeviceTwinRelay1Handler };

static LP_DEVICE_TWIN_BINDING deviceResetUtc = {
	.twinProperty = "DeviceResetUTC",
	.twinType = LP_TYPE_STRING };

// Initialize Sets
LP_TIMER* timerSet[] = { &azureIotConnectionStatusTimer, &measureSensorTimer, &resetDeviceOneShotTimer };
LP_GPIO* peripheralGpioSet[] = { &azureIotConnectedLed, &relay1 };
LP_DEVICE_TWIN_BINDING* deviceTwinBindingSet[] = { &desiredSampleRate, &buttonPressed, &desiredTemperature, &relay1DeviceTwin, &deviceResetUtc };

// Azure IoT Direct Methods
LP_DIRECT_METHOD_BINDING* directMethodBindingSet[] = {
	&(LP_DIRECT_METHOD_BINDING) { .methodName = "ResetMethod",.handler = ResetDirectMethodHandler }
};

// Telemetry message template and properties
static const char* msgTemplate = "{ \"Temperature\": \"%3.2f\", \"Humidity\": \"%3.1f\", \"Pressure\":\"%3.1f\", \"Light\":%d, \"MsgId\":%d }";

static LP_MESSAGE_PROPERTY* telemetryMessageProperties[] = {
	&(LP_MESSAGE_PROPERTY) { .key = "appid", .value = "lab-monitor" },
	&(LP_MESSAGE_PROPERTY) {.key = "format", .value = "json" },
	&(LP_MESSAGE_PROPERTY) {.key = "type", .value = "telemetry" },
	&(LP_MESSAGE_PROPERTY) {.key = "version", .value = "1" }
};

/// <summary>
/// Check status of connection to Azure IoT
/// </summary>
static void AzureIoTConnectionStatusHandler(EventLoopTimer* eventLoopTimer)
{
	static bool toggleConnectionStatusLed = true;

	if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0) {
		lp_terminate(ExitCode_ConsumeEventLoopTimeEvent);
		return;
	}

	if (lp_connectToAzureIot()) {
		lp_gpioSetState(&azureIotConnectedLed, toggleConnectionStatusLed);
		toggleConnectionStatusLed = !toggleConnectionStatusLed;
	}
	else {
		lp_gpioSetState(&azureIotConnectedLed, false);
	}
}

/// <summary>
/// This handler called when device twin desired 'SampleRateSeconds' recieved
/// </summary>
static void DeviceTwinSampleRateHandler(LP_DEVICE_TWIN_BINDING* deviceTwinBinding)
{
	int sampleRate = *(int*)deviceTwinBinding->twinState;

	if (sampleRate > 0 && sampleRate < (5 * 60)) // check sensible range
	{
		lp_timerChange(&measureSensorTimer, &(struct timespec){sampleRate, 0});
		lp_deviceTwinAckDesiredState(deviceTwinBinding, deviceTwinBinding->twinState, LP_DEVICE_TWIN_COMPLETED);
	}
	else {
		lp_deviceTwinAckDesiredState(deviceTwinBinding, deviceTwinBinding->twinState, LP_DEVICE_TWIN_ERROR);
	}
}

/// <summary>
/// Device Twin Handler to set the desired temperature value on the Real-Time Core
/// </summary>
static void DeviceTwinSetTemperatureHandler(LP_DEVICE_TWIN_BINDING* deviceTwinBinding)
{
	ic_control_block.cmd = LP_IC_SET_DESIRED_TEMPERATURE;
	ic_control_block.temperature = *(float*)deviceTwinBinding->twinState;
	lp_sendInterCoreMessage(&ic_control_block, sizeof(ic_control_block));

	lp_deviceTwinAckDesiredState(deviceTwinBinding, deviceTwinBinding->twinState, LP_DEVICE_TWIN_COMPLETED);
}

/// <summary>
/// Device Twin to control relay "Relay1": true or false
/// </summary>
static void DeviceTwinRelay1Handler(LP_DEVICE_TWIN_BINDING* deviceTwinBinding)
{
	lp_gpioSetState(&relay1, *(bool*)deviceTwinBinding->twinState);
	lp_deviceTwinAckDesiredState(deviceTwinBinding, deviceTwinBinding->twinState, LP_DEVICE_TWIN_COMPLETED);
}

/// <summary>
/// Read sensor and send to Azure IoT
/// </summary>
static void MeasureSensorHandler(EventLoopTimer* eventLoopTimer)
{
	if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0)
	{
		lp_terminate(ExitCode_ConsumeEventLoopTimeEvent);
		return;
	}

	// send request to Real-Time core app to read temperature, pressure, and humidity
	ic_control_block.cmd = LP_IC_TEMPERATURE_PRESSURE_HUMIDITY;
	lp_sendInterCoreMessage(&ic_control_block, sizeof(ic_control_block));
}

/// <summary>
/// Callback handler for Inter-Core Messaging - Does Device Twin Update, and Event Message
/// </summary>
static void InterCoreHandler(LP_INTER_CORE_BLOCK* ic_message_block)
{
	static int msgId = 0;
	int len = 0;

	switch (ic_message_block->cmd)
	{
	case LP_IC_EVENT_BUTTON_A:
		len = snprintf(msgBuffer, JSON_MESSAGE_BYTES, cstrJsonEvent, "ButtonA");
		lp_deviceTwinReportState(&buttonPressed, "ButtonA");					// TwinType = TYPE_STRING
		break;
	case LP_IC_TEMPERATURE_PRESSURE_HUMIDITY:
		len = snprintf(msgBuffer, JSON_MESSAGE_BYTES, msgTemplate, ic_message_block->temperature, ic_message_block->humidity, ic_message_block->pressure, 0, msgId++);
		break;
	default:
		break;
	}

	if (len > 0)
	{
		Log_Debug("%s\n", msgBuffer);
		lp_sendMsgWithProperties(msgBuffer, telemetryMessageProperties, NELEMS(telemetryMessageProperties));
	}
}

/// <summary>
/// Reset the Device
/// </summary>
static void ResetDeviceHandler(EventLoopTimer* eventLoopTimer)
{
	if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0)
	{
		lp_terminate(ExitCode_ConsumeEventLoopTimeEvent);
		return;
	}
	PowerManagement_ForceSystemReboot();
}

/// <summary>
/// Start Device Power Restart Direct Method 'ResetMethod' integer seconds eg 5
/// </summary>
static LP_DIRECT_METHOD_RESPONSE_CODE ResetDirectMethodHandler(JSON_Value* json, LP_DIRECT_METHOD_BINDING* directMethodBinding, char** responseMsg)
{
	const size_t responseLen = 60; // Allocate and initialize a response message buffer. The calling function is responsible for the freeing memory
	static struct timespec period;

	*responseMsg = (char*)malloc(responseLen);
	memset(*responseMsg, 0, responseLen);

	if (json_value_get_type(json) != JSONNumber) { return LP_METHOD_FAILED; }

	int seconds = (int)json_value_get_number(json);

	// leave enough time for the device twin deviceResetUtc to update before restarting the device
	if (seconds > 2 && seconds < 10)
	{
		// Report Device Reset UTC
		lp_deviceTwinReportState(&deviceResetUtc, lp_getCurrentUtc(msgBuffer, sizeof(msgBuffer))); // LP_TYPE_STRING

		// Create Direct Method Response
		snprintf(*responseMsg, responseLen, "%s called. Reset in %d seconds", directMethodBinding->methodName, seconds);

		// Set One Shot LP_TIMER
		period = (struct timespec){ .tv_sec = seconds, .tv_nsec = 0 };
		lp_timerSetOneShot(&resetDeviceOneShotTimer, &period);

		return LP_METHOD_SUCCEEDED;
	}
	else
	{
		snprintf(*responseMsg, responseLen, "%s called. Reset Failed. Seconds out of range: %d", directMethodBinding->methodName, seconds);
		return LP_METHOD_FAILED;
	}
}

/// <summary>
///  Initialize PeripheralGpios, device twins, direct methods, timers.
/// </summary>
/// <returns>0 on success, or -1 on failure</returns>
static void InitPeripheralAndHandlers(void)
{
	lp_gpioOpenSet(peripheralGpioSet, NELEMS(peripheralGpioSet));
	lp_deviceTwinOpenSet(deviceTwinBindingSet, NELEMS(deviceTwinBindingSet));
	lp_directMethodOpenSet(directMethodBindingSet, NELEMS(directMethodBindingSet));

	lp_timerStartSet(timerSet, NELEMS(timerSet));
	lp_cloudToDeviceStart();

	lp_enableInterCoreCommunications(rtAppComponentId, InterCoreHandler);  // Initialize Inter Core Communications

	ic_control_block.cmd = LP_IC_HEARTBEAT;		// Prime RT Core with Component ID Signature
	lp_sendInterCoreMessage(&ic_control_block, sizeof(ic_control_block));
}

/// <summary>
/// Close PeripheralGpios and handlers.
/// </summary>
static void ClosePeripheralAndHandlers(void)
{
	Log_Debug("Closing file descriptors\n");

	lp_timerStopSet();
	lp_cloudToDeviceStop();

	lp_gpioCloseSet();
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