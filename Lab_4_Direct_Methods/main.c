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

#define JSON_MESSAGE_BYTES 256 // Number of bytes to allocate for the JSON telemetry message for IoT Central

// Forward signatures
static void InitPeripheralAndHandlers(void);
static void ClosePeripheralAndHandlers(void);
static void Led1BlinkHandler(EventLoopTimer* eventLoopTimer);
static void NetworkConnectionStatusHandler(EventLoopTimer* eventLoopTimer);
static void ResetDeviceHandler(EventLoopTimer* eventLoopTimer);
static LP_DIRECT_METHOD_RESPONSE_CODE ResetDirectMethodHandler(JSON_Object* json, LP_DIRECT_METHOD_BINDING* directMethodBinding, char** responseMsg);

static char msgBuffer[JSON_MESSAGE_BYTES] = { 0 };

// GPIO
static LP_PERIPHERAL_GPIO led1 = { .pin = LED1, .direction = LP_OUTPUT, .initialState = GPIO_Value_Low, .invertPin = true,
	.initialise = lp_openPeripheralGpio, .name = "led1" };
static LP_PERIPHERAL_GPIO networkConnectedLed = { .pin = NETWORK_CONNECTED_LED, .direction = LP_OUTPUT, .initialState = GPIO_Value_Low, .invertPin = true, .initialise = lp_openPeripheralGpio, .name = "networkConnectedLed" };

// Timers
static LP_TIMER networkConnectionStatusTimer = { .period = {5, 0}, .name = "networkConnectionStatusTimer", .handler = NetworkConnectionStatusHandler };
static LP_TIMER resetDeviceOneShotTimer = { .period = {0, 0}, .name = "resetDeviceOneShotTimer", .handler = ResetDeviceHandler };
static LP_TIMER led1BlinkTimer = { .period = { 0, 125000000 }, .name = "led1BlinkTimer", .handler = Led1BlinkHandler };

// Azure IoT Device Twins
static LP_DEVICE_TWIN_BINDING deviceResetUtc = { .twinProperty = "DeviceResetUTC", .twinType = LP_TYPE_STRING };

// Azure IoT Direct Methods
static LP_DIRECT_METHOD_BINDING resetDevice = { .methodName = "ResetMethod", .handler = ResetDirectMethodHandler };

// Initialize Sets
LP_PERIPHERAL_GPIO* PeripheralGpioSet[] = { &led1, &networkConnectedLed };
LP_TIMER* timerSet[] = {&networkConnectionStatusTimer, &led1BlinkTimer, &resetDeviceOneShotTimer };
LP_DEVICE_TWIN_BINDING* deviceTwinBindingSet[] = { &deviceResetUtc };
LP_DIRECT_METHOD_BINDING* directMethodBindingSet[] = { &resetDevice };


/// <summary>
/// Check status of connection to Azure IoT
/// </summary>
static void NetworkConnectionStatusHandler(EventLoopTimer* eventLoopTimer)
{
	if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0)
	{
		lp_terminate(ExitCode_ConsumeEventLoopTimeEvent);
		return;
	}

	if (lp_connectToAzureIot()) { lp_gpioOn(&networkConnectedLed); }
	else { lp_gpioOff(&networkConnectedLed); }
}

/// <summary>
/// Blink Led1 Handler
/// </summary>
static void Led1BlinkHandler(EventLoopTimer* eventLoopTimer)
{
	static bool led_state = false;

	if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0)
	{
		lp_terminate(ExitCode_ConsumeEventLoopTimeEvent);
		return;
	}

	led_state = !led_state;

	if (led_state) { lp_gpioOff(&led1); }
	else { lp_gpioOn(&led1); }
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
/// Start Device Power Restart Direct Method 'ResetMethod' {"reset_timer":5}
/// </summary>
static LP_DIRECT_METHOD_RESPONSE_CODE ResetDirectMethodHandler(JSON_Object* json, LP_DIRECT_METHOD_BINDING* directMethodBinding, char** responseMsg)
{
	const char propertyName[] = "reset_timer";
	const size_t responseLen = 60; // Allocate and initialize a response message buffer. The calling function is responsible for the freeing memory
	static struct timespec period;

	*responseMsg = (char*)malloc(responseLen);
	memset(*responseMsg, 0, responseLen);

	if (!json_object_has_value_of_type(json, propertyName, JSONNumber))
	{
		return LP_METHOD_FAILED;
	}
	int seconds = (int)json_object_get_number(json, propertyName);

	// leave enough time for the device twin deviceResetUtc to update before restarting the device
	if (seconds > 2 && seconds < 10)
	{
		// Report Device Reset UTC
		lp_deviceTwinReportState(&deviceResetUtc, lp_getCurrentUtc(msgBuffer, sizeof(msgBuffer))); // LP_TYPE_STRING

		// Create Direct Method Response
		snprintf(*responseMsg, responseLen, "%s called. Reset in %d seconds", directMethodBinding->methodName, seconds);

		// Set One Shot LP_TIMER
		period = (struct timespec){ .tv_sec = seconds, .tv_nsec = 0 };
		lp_setOneShotTimer(&resetDeviceOneShotTimer, &period);

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
	lp_initializeDevKit();

	lp_openPeripheralGpioSet(PeripheralGpioSet, NELEMS(PeripheralGpioSet));
	lp_openDeviceTwinSet(deviceTwinBindingSet, NELEMS(deviceTwinBindingSet));
	lp_openDirectMethodSet(directMethodBindingSet, NELEMS(directMethodBindingSet));

	lp_startTimerSet(timerSet, NELEMS(timerSet));
	lp_startCloudToDevice();
}

/// <summary>
///     Close PeripheralGpios and handlers.
/// </summary>
static void ClosePeripheralAndHandlers(void)
{
	Log_Debug("Closing file descriptors\n");

	lp_stopTimerSet();
	lp_stopCloudToDevice();

	lp_closePeripheralGpioSet();
	lp_closeDeviceTwinSet();
	lp_closeDirectMethodSet();

	lp_closeDevKit();

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

	InitPeripheralAndHandlers();

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

	ClosePeripheralAndHandlers();

	Log_Debug("Application exiting.\n");
	return lp_getTerminationExitCode();
}