/*************************************************************************************************************************************
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
 *     2. AVNET Azure Sphere Starter Kit Revision 2.
 *	   3. Seeed Studio Azure Sphere MT3620 Development Kit aka Reference Design Board or rdb.
 *	   4. Seeed Studio Seeed Studio MT3620 Mini Dev Board.
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
 *
 *
 *
 * The intercore communications labs require multiple instances of VS Code to be running
 *
 * It is recommended to install the VS Code Peacock extension for the intercore communications labs.
 * The Peacock extension allows you to change the color of your Visual Studio Code workspace.
 * Ideal when you have multiple VS Code instances
 *
 * Install the Peacock extension from https://marketplace.visualstudio.com/items?itemName=johnpapa.vscode-peacock
 *
 * The following colours have been set:
 * The VS Code instance attached to the Real-Time core will be red. Real-time is red, as in racing red.
 * The VS Code instance attached to the High-level core is blue. High-level is blue, as in sky is high and blue.
 * You can change the default colours to match your preferences.
 *
 *
 * Intercore messaging.
 *
 * There needs to be a shared understanding of the data structure being shared between the real-time and high-level apps
 * This shared understanding is declared in the intercore_contract.h file.  This file can be found in the IntercoreContract directory.
 *
*************************************************************************************************************************************/


// Hardware definition
#include "hw/azure_sphere_learning_path.h"

// Learning Path Libraries
#include "azure_iot.h"
#include "config.h"
#include "exit_codes.h"
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

#define LP_LOGGING_ENABLED FALSE
#define JSON_MESSAGE_BYTES 256  // Number of bytes to allocate for the JSON telemetry message for IoT Central
#define IOT_PLUG_AND_PLAY_MODEL_ID "dtmi:com:example:azuresphere:labmonitor;1"	// https://docs.microsoft.com/en-us/azure/iot-pnp/overview-iot-plug-and-play
#define REAL_TIME_COMPONENT_ID "6583cf17-d321-4d72-8283-0b7c5b56442b"


// Forward signatures
static LP_DIRECT_METHOD_RESPONSE_CODE RestartDeviceHandler(JSON_Value* json, LP_DIRECT_METHOD_BINDING* directMethodBinding, char** responseMsg);
static void AzureIoTConnectionStatusHandler(EventLoopTimer* eventLoopTimer);
static void DelayRestartDeviceTimerHandler(EventLoopTimer* eventLoopTimer);
static void DeviceTwinSetSampleRateHandler(LP_DEVICE_TWIN_BINDING* deviceTwinBinding);
static void DeviceTwinSetTemperatureHandler(LP_DEVICE_TWIN_BINDING* deviceTwinBinding);
static void InterCoreHandler(LP_INTER_CORE_BLOCK* ic_message_block);
static void MeasureSensorHandler(EventLoopTimer* eventLoopTimer);


LP_USER_CONFIG lp_config;
LP_INTER_CORE_BLOCK ic_control_block;

static int previous_temperature = 999999;

enum LEDS { RED, GREEN, BLUE, UNKNOWN };
static enum LEDS current_led = RED;
static const char* hvacState[] = { "heating", "off", "cooling" };

static char msgBuffer[JSON_MESSAGE_BYTES] = { 0 };

// Declare GPIO
static LP_GPIO azureIotConnectedLed = {
	.pin = NETWORK_CONNECTED_LED,
	.direction = LP_OUTPUT,
	.initialState = GPIO_Value_Low,
	.invertPin = true,
	.name = "azureIotConnectedLed" };

static LP_GPIO* ledRgb[] = {
	&(LP_GPIO) { .pin = LED_RED, .direction = LP_OUTPUT, .initialState = GPIO_Value_Low, .invertPin = true,.name = "red led" },
	&(LP_GPIO) {.pin = LED_GREEN, .direction = LP_OUTPUT, .initialState = GPIO_Value_Low, .invertPin = true, .name = "green led" },
	&(LP_GPIO) {.pin = LED_BLUE, .direction = LP_OUTPUT, .initialState = GPIO_Value_Low, .invertPin = true, .name = "blue led" }
};

// Timers
static LP_TIMER azureIotConnectionStatusTimer = {
	.period = { 5, 0 },
	.name = "azureIotConnectionStatusTimer",
	.handler = AzureIoTConnectionStatusHandler };

static LP_TIMER measureSensorTimer = {
	.period = { 6, 0 },
	.name = "measureSensorTimer",
	.handler = MeasureSensorHandler };

static LP_TIMER restartDeviceOneShotTimer = {
	.period = {0, 0},
	.name = "restartDeviceOneShotTimer",
	.handler = DelayRestartDeviceTimerHandler };

// Azure IoT Device Twins
static LP_DEVICE_TWIN_BINDING dt_desiredTemperature = {
	.twinProperty = "DesiredTemperature",
	.twinType = LP_TYPE_FLOAT,
	.handler = DeviceTwinSetTemperatureHandler };

static LP_DEVICE_TWIN_BINDING dt_desiredSampleRateInSeconds = {
	.twinProperty = "DesiredSampleRateInSeconds",
	.twinType = LP_TYPE_INT,
	.handler = DeviceTwinSetSampleRateHandler };

static LP_DEVICE_TWIN_BINDING dt_reportedTemperature = {
	.twinProperty = "ReportedTemperature",
	.twinType = LP_TYPE_FLOAT };

static LP_DEVICE_TWIN_BINDING dt_reportedHvacState = {
	.twinProperty = "ReportedHvacState",
	.twinType = LP_TYPE_STRING };

static LP_DEVICE_TWIN_BINDING dt_reportedDeviceStartTime = {
	.twinProperty = "ReportedDeviceStartTime",
	.twinType = LP_TYPE_STRING };

static LP_DEVICE_TWIN_BINDING dt_reportedRestartUtc = {
	.twinProperty = "ReportedRestartUTC",
	.twinType = LP_TYPE_STRING };

// Azure IoT Direct Methods
static LP_DIRECT_METHOD_BINDING dm_restartDevice = {
	.methodName = "RestartDevice",
	.handler = RestartDeviceHandler };

// Initialize Sets
LP_GPIO* gpioSet[] = { &azureIotConnectedLed };
LP_TIMER* timerSet[] = { &azureIotConnectionStatusTimer, &measureSensorTimer, &restartDeviceOneShotTimer };
LP_DEVICE_TWIN_BINDING* deviceTwinBindingSet[] = {
	&dt_desiredTemperature, &dt_reportedTemperature, &dt_reportedHvacState,
	&dt_reportedDeviceStartTime, &dt_desiredSampleRateInSeconds, &dt_reportedRestartUtc
};
LP_DIRECT_METHOD_BINDING* directMethodBindingSet[] = { &dm_restartDevice };

// Telemetry message template and properties
static const char* msgTemplate = "{ \"Temperature\":%3.2f, \"Humidity\":%3.1f, \"Pressure\":%3.1f, \"MsgId\":%d }";

static LP_MESSAGE_PROPERTY* telemetryMessageProperties[] = {
	&(LP_MESSAGE_PROPERTY) { .key = "appid", .value = "hvac" },
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
	static bool firstConnect = true;

	if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0) {
		lp_terminate(ExitCode_ConsumeEventLoopTimeEvent);
	}
	else {
		if (lp_azureConnect()) {
			lp_gpioStateSet(&azureIotConnectedLed, toggleConnectionStatusLed);
			toggleConnectionStatusLed = !toggleConnectionStatusLed;

			if (firstConnect) {
				lp_deviceTwinReportState(&dt_reportedDeviceStartTime, lp_getCurrentUtc(msgBuffer, sizeof(msgBuffer))); // LP_TYPE_STRING
				firstConnect = false;
			}
		}
		else {
			lp_gpioStateSet(&azureIotConnectedLed, false);
		}
	}
}

/// <summary>
/// Read sensor and send to Azure IoT
/// </summary>
static void MeasureSensorHandler(EventLoopTimer* eventLoopTimer)
{
	if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0)
	{
		lp_terminate(ExitCode_ConsumeEventLoopTimeEvent);
	}
	else {
		// send request to Real-Time core app to read temperature, pressure, and humidity
		ic_control_block.cmd = LP_IC_ENVIRONMENT_SENSOR;
		lp_interCoreSendMessage(&ic_control_block, sizeof(ic_control_block));
	}
}

/// <summary>
/// Set the temperature status led. 
/// Red if HVAC needs to be turned on to get to desired temperature. 
/// Blue to turn on cooler. 
/// Green equals just right, no action required.
/// </summary>
void SetHvacStatusColour(int temperature)
{
	static enum LEDS previous_led = UNKNOWN;

	// No desired temperature device twin update to date so return 
	if (!dt_desiredTemperature.twinStateUpdated) { return; }

	int desired = (int)(*(float*)dt_desiredTemperature.twinState);

	current_led = temperature == desired ? GREEN : temperature > desired ? BLUE : RED;

	if (previous_led != current_led)
	{
		lp_gpioOff(ledRgb[(int)previous_led]); // turn off old current colour
		previous_led = current_led;
		lp_deviceTwinReportState(&dt_reportedHvacState, (void*)hvacState[(int)current_led]);
	}
	lp_gpioOn(ledRgb[(int)current_led]);
}

/// <summary>
/// Device Twin Handler to set the desired temperature value
/// </summary>
static void DeviceTwinSetTemperatureHandler(LP_DEVICE_TWIN_BINDING* deviceTwinBinding)
{
	// validate data is sensible range before applying
	if (deviceTwinBinding->twinType == LP_TYPE_FLOAT && *(float*)deviceTwinBinding->twinState >= -20.0f && *(float*)deviceTwinBinding->twinState <= 80.0f)
	{
		lp_deviceTwinAckDesiredState(deviceTwinBinding, deviceTwinBinding->twinState, LP_DEVICE_TWIN_COMPLETED);
		SetHvacStatusColour((int)previous_temperature);
	}
	else {
		lp_deviceTwinAckDesiredState(deviceTwinBinding, deviceTwinBinding->twinState, LP_DEVICE_TWIN_ERROR);
	}
}

/// <summary>
/// Device Twin Handler to set the sensor sample rate on the real-time core
/// </summary>
static void DeviceTwinSetSampleRateHandler(LP_DEVICE_TWIN_BINDING* deviceTwinBinding)
{
	// validate data is sensible range before applying
	if (deviceTwinBinding->twinType == LP_TYPE_INT && *(int*)deviceTwinBinding->twinState > 0 && *(int*)deviceTwinBinding->twinState <= 60)
	{
		ic_control_block.cmd = LP_IC_SAMPLE_RATE;
		ic_control_block.sample_rate = *(int*)deviceTwinBinding->twinState;
		lp_interCoreSendMessage(&ic_control_block, sizeof(ic_control_block));

		lp_deviceTwinAckDesiredState(deviceTwinBinding, deviceTwinBinding->twinState, LP_DEVICE_TWIN_COMPLETED);
	}
	else {
		lp_deviceTwinAckDesiredState(deviceTwinBinding, deviceTwinBinding->twinState, LP_DEVICE_TWIN_ERROR);
	}
}


/// <summary>
/// Callback handler for Inter-Core Messaging - Does Device Twin Update, and Event Message
/// </summary>
static void InterCoreHandler(LP_INTER_CORE_BLOCK* ic_message_block)
{
	static int msgId = 0;

	switch (ic_message_block->cmd)
	{
	case LP_IC_ENVIRONMENT_SENSOR:
		if (snprintf(msgBuffer, JSON_MESSAGE_BYTES, msgTemplate, ic_message_block->temperature,
			ic_message_block->humidity, ic_message_block->pressure, msgId++) > 0) {

			Log_Debug("%s\n", msgBuffer);
			lp_azureMsgSendWithProperties(msgBuffer, telemetryMessageProperties, NELEMS(telemetryMessageProperties));

			SetHvacStatusColour((int)ic_message_block->temperature);

			// If the previous temperature not equal to the new temperature then update ReportedTemperature device twin
			if (previous_temperature != (int)ic_message_block->temperature) {
				lp_deviceTwinReportState(&dt_reportedTemperature, &ic_message_block->temperature);
				previous_temperature = (int)ic_message_block->temperature;
			}			
		}
		break;
	default:
		break;
	}
}

/// <summary>
/// Restart the Device
/// </summary>
static void DelayRestartDeviceTimerHandler(EventLoopTimer* eventLoopTimer)
{
	if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0)
	{
		lp_terminate(ExitCode_ConsumeEventLoopTimeEvent);
	}
	else {
		PowerManagement_ForceSystemReboot();
	}
}

/// <summary>
/// Start Device Power Restart Direct Method 'ResetMethod' integer seconds eg 5
/// Update Restart UTC Device Twin
/// Set oneshot timer to restart the device
/// </summary>
static LP_DIRECT_METHOD_RESPONSE_CODE RestartDeviceHandler(JSON_Value* json, LP_DIRECT_METHOD_BINDING* directMethodBinding, char** responseMsg)
{
	if (json_value_get_type(json) != JSONNumber) { return LP_METHOD_FAILED; }

	int seconds = (int)json_value_get_number(json);

	if (seconds > 2 && seconds < 10) // leave enough time for the device twin dt_reportedRestartUtc to update before restarting the device
	{
		lp_deviceTwinReportState(&dt_reportedRestartUtc, lp_getCurrentUtc(msgBuffer, sizeof(msgBuffer))); // LP_TYPE_STRING
		lp_timerOneShotSet(&restartDeviceOneShotTimer, &(struct timespec){.tv_sec = seconds, .tv_nsec = 0 });
		return LP_METHOD_SUCCEEDED;
	}
	else {
		return LP_METHOD_FAILED;
	}
}

/// <summary>
///  Initialize PeripheralGpios, device twins, direct methods, timers.
/// </summary>
/// <returns>0 on success, or -1 on failure</returns>
static void InitPeripheralAndHandlers(void)
{
	lp_azureInitialize(lp_config.scopeId, IOT_PLUG_AND_PLAY_MODEL_ID);

	lp_gpioSetOpen(gpioSet, NELEMS(gpioSet));
	lp_gpioSetOpen(ledRgb, NELEMS(ledRgb));
	lp_timerSetStart(timerSet, NELEMS(timerSet));
	lp_deviceTwinSetOpen(deviceTwinBindingSet, NELEMS(deviceTwinBindingSet));
	lp_directMethodSetOpen(directMethodBindingSet, NELEMS(directMethodBindingSet));

	lp_azureToDeviceStart();

	lp_interCoreCommunicationsEnable(REAL_TIME_COMPONENT_ID, InterCoreHandler);  // Initialize Inter Core Communications
}

/// <summary>
/// Close PeripheralGpios and handlers.
/// </summary>
static void ClosePeripheralAndHandlers(void)
{
	Log_Debug("Closing file descriptors\n");

	lp_deviceTwinSetClose();
	lp_directMethodSetClose();

	lp_timerSetStop(timerSet, NELEMS(timerSet));
	lp_azureToDeviceStop();

	lp_gpioSetClose(gpioSet, NELEMS(gpioSet));
	lp_gpioSetClose(ledRgb, NELEMS(ledRgb));
	lp_timerEventLoopStop();
}

int main(int argc, char* argv[])
{
	lp_registerTerminationHandler();

	lp_configParseCmdLineArguments(argc, argv, &lp_config);
	if (!lp_configValidate(&lp_config)) {
		return lp_getTerminationExitCode();
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