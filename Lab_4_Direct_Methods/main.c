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
#include "SEEED_STUDIO/board.h"
#endif // SEEED_STUDIO

#define LP_LOGGING_ENABLED FALSE
#define JSON_MESSAGE_BYTES 256 // Number of bytes to allocate for the JSON telemetry message for IoT Central

// Forward signatures
static void InitPeripheralAndHandlers(void);
static void ClosePeripheralAndHandlers(void);
static void AzureIoTConnectionStatusHandler(EventLoopTimer* eventLoopTimer);
static void ResetDeviceHandler(EventLoopTimer* eventLoopTimer);
static LP_DIRECT_METHOD_RESPONSE_CODE ResetDirectMethodHandler(JSON_Value* json, LP_DIRECT_METHOD_BINDING* directMethodBinding, char** responseMsg);

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
static LP_TIMER networkConnectionStatusTimer = {
    .period = {5, 0},
    .name = "networkConnectionStatusTimer",
    .handler = AzureIoTConnectionStatusHandler };

static LP_TIMER resetDeviceOneShotTimer = {
    .period = {0, 0},
    .name = "resetDeviceOneShotTimer",
    .handler = ResetDeviceHandler };

// Azure IoT Device Twins
static LP_DEVICE_TWIN_BINDING deviceResetUtc = {
    .twinProperty = "DeviceResetUTC",
    .twinType = LP_TYPE_STRING };

// Azure IoT Direct Methods
static LP_DIRECT_METHOD_BINDING resetDevice = {
    .methodName = "ResetMethod",
    .handler = ResetDirectMethodHandler };

// Initialize Sets
LP_GPIO* gpioSet[] = { &azureIotConnectedLed };
LP_TIMER* timerSet[] = { &networkConnectionStatusTimer, &resetDeviceOneShotTimer };
LP_DEVICE_TWIN_BINDING* deviceTwinBindingSet[] = { &deviceResetUtc };
LP_DIRECT_METHOD_BINDING* directMethodBindingSet[] = { &resetDevice };


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

    if (lp_azureConnect()) {
        lp_gpioStateSet(&azureIotConnectedLed, toggleConnectionStatusLed);
        toggleConnectionStatusLed = !toggleConnectionStatusLed;
    }
    else {
        lp_gpioStateSet(&azureIotConnectedLed, false);
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
    }
    else {
        PowerManagement_ForceSystemReboot();
    }
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
static void InitPeripheralAndHandlers(void)
{
    lp_azureIdScopeSet(lp_config.scopeId);

    lp_initializeDevKit();

    lp_gpioOpenSet(gpioSet, NELEMS(gpioSet));
    lp_deviceTwinOpenSet(deviceTwinBindingSet, NELEMS(deviceTwinBindingSet));
    lp_directMethodOpenSet(directMethodBindingSet, NELEMS(directMethodBindingSet));

    lp_timerStartSet(timerSet, NELEMS(timerSet));
    
    lp_azureToDeviceStart();
}

/// <summary>
///     Close PeripheralGpios and handlers.
/// </summary>
static void ClosePeripheralAndHandlers(void)
{
    Log_Debug("Closing file descriptors\n");

    lp_timerStopSet(timerSet, NELEMS(timerSet));
    lp_azureToDeviceStop();

    lp_gpioCloseSet(gpioSet, NELEMS(gpioSet));
    lp_deviceTwinCloseSet();
    lp_directMethodCloseSet();

    lp_closeDevKit();

    lp_timerStopEventLoop();
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