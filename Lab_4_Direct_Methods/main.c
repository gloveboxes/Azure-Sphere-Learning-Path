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
 */

 // Hardware definition
#include "hw/azure_sphere_learning_path.h"

// Learning Path Libraries
#include "azure_iot.h"
#include "config.h"
#include "exit_codes.h"
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
#include "board.h"
#include "imu_temp_pressure.h"
#include "light_sensor.h"
#endif // OEM_AVNET

// Hardware specific
#ifdef OEM_SEEED_STUDIO
#include "board.h"
#endif // SEEED_STUDIO

#define LP_LOGGING_ENABLED FALSE
#define JSON_MESSAGE_BYTES 256 // Number of bytes to allocate for the JSON telemetry message for IoT Central
#define IOT_PLUG_AND_PLAY_MODEL_ID "dtmi:com:example:azuresphere:labmonitor;1"	// https://docs.microsoft.com/en-us/azure/iot-pnp/overview-iot-plug-and-play

// Forward signatures
static void MeasureSensorHandler(EventLoopTimer* eventLoopTimer);
static void AzureIoTConnectionStatusHandler(EventLoopTimer* eventLoopTimer);
static void DelayRestartDeviceTimerHandler(EventLoopTimer* eventLoopTimer);
static LP_DIRECT_METHOD_RESPONSE_CODE RestartDeviceHandler(JSON_Value* json, LP_DIRECT_METHOD_BINDING* directMethodBinding, char** responseMsg);

LP_USER_CONFIG lp_config;

static char msgBuffer[JSON_MESSAGE_BYTES] = { 0 };

// GPIO
static LP_GPIO azureIotConnectedLed = {
    .pin = NETWORK_CONNECTED_LED,
    .direction = LP_OUTPUT,
    .initialState = GPIO_Value_Low,
    .invertPin = true,
    .name = "networkConnectedLed" };

// Timers
static LP_TIMER azureIotConnectionStatusTimer = {
    .period = {5, 0},
    .name = "azureIotConnectionStatusTimer",
    .handler = AzureIoTConnectionStatusHandler };

static LP_TIMER restartDeviceOneShotTimer = {
    .period = {0, 0},
    .name = "restartDeviceOneShotTimer",
    .handler = DelayRestartDeviceTimerHandler };

static LP_TIMER measureSensorTimer = {
	.period = { 6, 0 },
	.name = "measureSensorTimer",
	.handler = MeasureSensorHandler };

// Azure IoT Device Twins
static LP_DEVICE_TWIN_BINDING dt_reportedRestartUtc = {
    .twinProperty = "ReportedRestartUTC",
    .twinType = LP_TYPE_STRING };

// Azure IoT Direct Methods
static LP_DIRECT_METHOD_BINDING dm_restartDevice = {
    .methodName = "RestartDevice",
    .handler = RestartDeviceHandler };

// Initialize Sets
LP_GPIO* gpioSet[] = { &azureIotConnectedLed };
LP_TIMER* timerSet[] = { &measureSensorTimer, &azureIotConnectionStatusTimer, &restartDeviceOneShotTimer };
LP_DEVICE_TWIN_BINDING* deviceTwinBindingSet[] = { &dt_reportedRestartUtc };
LP_DIRECT_METHOD_BINDING* directMethodBindingSet[] = { &dm_restartDevice };

// Message templates and property sets

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

	if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0) {
		lp_terminate(ExitCode_ConsumeEventLoopTimeEvent);
	}
	else {
		if (lp_azureIsConnected()) {
			lp_gpioStateSet(&azureIotConnectedLed, toggleConnectionStatusLed);
			toggleConnectionStatusLed = !toggleConnectionStatusLed;
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
	static int msgId = 0;
	static LP_ENVIRONMENT environment;

	if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0)
	{
		lp_terminate(ExitCode_ConsumeEventLoopTimeEvent);
	}
	else {
		if (lp_readTelemetry(&environment) &&
			snprintf(msgBuffer, JSON_MESSAGE_BYTES, msgTemplate,
				environment.temperature, environment.humidity, environment.pressure, msgId++) > 0)
		{
			Log_Debug("%s\n", msgBuffer);
			lp_azureMsgSendWithProperties(msgBuffer, telemetryMessageProperties, NELEMS(telemetryMessageProperties));
		}
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
/// </summary>
static LP_DIRECT_METHOD_RESPONSE_CODE RestartDeviceHandler(JSON_Value* json, LP_DIRECT_METHOD_BINDING* directMethodBinding, char** responseMsg)
{
    const size_t responseLen = 100; // Allocate and initialize a response message buffer. The calling function is responsible for the freeing memory
    static struct timespec period;

    *responseMsg = (char*)malloc(responseLen);
    memset(*responseMsg, 0, responseLen);

    if (json_value_get_type(json) != JSONNumber) { return LP_METHOD_FAILED; }

    int seconds = (int)json_value_get_number(json);

    // leave enough time for the device twin dt_reportedRestartUtc to update before restarting the device
    if (seconds > 2 && seconds < 10)
    {
        // Report Device Restart UTC
        lp_deviceTwinReportState(&dt_reportedRestartUtc, lp_getCurrentUtc(msgBuffer, sizeof(msgBuffer))); // LP_TYPE_STRING

        // Create Direct Method Response
        snprintf(*responseMsg, responseLen, "%s called. Restart in %d seconds", directMethodBinding->methodName, seconds);

        // Set One Shot LP_TIMER
        period = (struct timespec){ .tv_sec = seconds, .tv_nsec = 0 };
        lp_timerOneShotSet(&restartDeviceOneShotTimer, &period);

        return LP_METHOD_SUCCEEDED;
    }
    else
    {
        snprintf(*responseMsg, responseLen, "%s called. Restart Failed. Seconds out of range: %d", directMethodBinding->methodName, seconds);
        return LP_METHOD_FAILED;
    }
}

/// <summary>
///  Initialize PeripheralGpios, device twins, direct methods, timers.
/// </summary>
static void InitPeripheralAndHandlers(void)
{
    lp_azureInitialize(lp_config.scopeId, IOT_PLUG_AND_PLAY_MODEL_ID);

    lp_initializeDevKit();

    lp_gpioSetOpen(gpioSet, NELEMS(gpioSet));

    lp_deviceTwinSetOpen(deviceTwinBindingSet, NELEMS(deviceTwinBindingSet));
    
    lp_directMethodSetOpen(directMethodBindingSet, NELEMS(directMethodBindingSet));

    lp_timerSetStart(timerSet, NELEMS(timerSet));
    
    lp_azureToDeviceStart();
}

/// <summary>
///     Close PeripheralGpios and handlers.
/// </summary>
static void ClosePeripheralAndHandlers(void)
{
    lp_timerSetStop(timerSet, NELEMS(timerSet));
    lp_azureToDeviceStop();

    lp_gpioSetClose(gpioSet, NELEMS(gpioSet));
    lp_deviceTwinSetClose();
    lp_directMethodSetClose();

    lp_closeDevKit();

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