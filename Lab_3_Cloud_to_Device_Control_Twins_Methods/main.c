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

#include "learning_path_libs/azure_iot.h"
#include "learning_path_libs/exit_codes.h"
#include "learning_path_libs/globals.h"
#include "learning_path_libs/peripheral_gpio.h"
#include "learning_path_libs/terminate.h"
#include "learning_path_libs/timer.h"

#include "applibs_versions.h"
#include <applibs/gpio.h>
#include <applibs/log.h>
#include <applibs/powermanagement.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>


// Hardware specific

#ifdef OEM_AVNET
#include "learning_path_libs/AVNET/board.h"
#include "learning_path_libs/AVNET/imu_temp_pressure.h"
#include "learning_path_libs/AVNET/light_sensor.h"
#endif // OEM_AVNET

#ifdef OEM_SEEED_STUDIO
#include "learning_path_libs/SEEED_STUDIO/board.h"
#endif // SEEED_STUDIO

#define JSON_MESSAGE_BYTES 128  // Number of bytes to allocate for the JSON telemetry message for IoT Central

// Forward signatures
static void InitPeripheralGpiosAndHandlers(void);
static void ClosePeripheralGpiosAndHandlers(void);
static void Led1BlinkHandler(EventLoopTimer* eventLoopTimer);
static void Led2OffHandler(EventLoopTimer* eventLoopTimer);
static void MeasureSensorHandler(EventLoopTimer* eventLoopTimer);
static void ButtonPressCheckHandler(EventLoopTimer* eventLoopTimer);
static void NetworkConnectionStatusHandler(EventLoopTimer* eventLoopTimer);
static void ResetDeviceHandler(EventLoopTimer* eventLoopTimer);
static void DeviceTwinBlinkRateHandler(LP_DEVICE_TWIN_BINDING* deviceTwinBinding);
static void DeviceTwinRelay1Handler(LP_DEVICE_TWIN_BINDING* deviceTwinBinding);
static LP_DirectMethodResponseCode ResetDirectMethodHandler(JSON_Object* json, LP_DIRECT_METHOD_BINDING* directMethodBinding, char** responseMsg);

static char msgBuffer[JSON_MESSAGE_BYTES] = { 0 };

static const char cstrJsonEvent[] = "{\"%s\":\"occurred\"}";
static const char cstrEvtButtonB[] = "ButtonB";
static const char cstrEvtButtonA[] = "ButtonA";

static const struct timespec led2BlinkPeriod = { 0, 300 * 1000 * 1000 };

static int Led1BlinkIntervalIndex = 0;
static const struct timespec led1BlinkIntervals[] = { {0, 125000000}, {0, 250000000}, {0, 500000000}, {0, 750000000}, {1, 0} };
static const int led1BlinkIntervalsCount = NELEMS(led1BlinkIntervals);

// GPIO Input PeripheralGpios
static LP_PERIPHERAL_GPIO buttonA = { .pin = BUTTON_A, .direction = LP_INPUT, .initialise = lp_openPeripheralGpio, .name = "buttonA" };
static LP_PERIPHERAL_GPIO buttonB = { .pin = BUTTON_B, .direction = LP_INPUT, .initialise = lp_openPeripheralGpio, .name = "buttonB" };

// GPIO Output PeripheralGpios
static LP_PERIPHERAL_GPIO led1 = { .pin = LED1, .direction = LP_OUTPUT, .initialState = GPIO_Value_Low, .invertPin = true,
	.initialise = lp_openPeripheralGpio, .name = "led1" };

static LP_PERIPHERAL_GPIO led2 = { .pin = LED2, .direction = LP_OUTPUT, .initialState = GPIO_Value_Low, .invertPin = true,
	.initialise = lp_openPeripheralGpio, .name = "led2" };

static LP_PERIPHERAL_GPIO networkConnectedLed = { .pin = NETWORK_CONNECTED_LED, .direction = LP_OUTPUT, .initialState = GPIO_Value_Low, .invertPin = true,
	.initialise = lp_openPeripheralGpio, .name = "networkConnectedLed" };

static LP_PERIPHERAL_GPIO relay1 = { .pin = RELAY, .direction = LP_OUTPUT, .initialState = GPIO_Value_Low, .invertPin = false,
	.initialise = lp_openPeripheralGpio, .name = "relay1" };

// Timers
static LP_TIMER led1BlinkTimer = { .period = { 0, 125000000 }, .name = "led1BlinkTimer", .handler = Led1BlinkHandler };
static LP_TIMER led2BlinkOffOneShotTimer = { .period = { 0, 0 }, .name = "led2BlinkOffOneShotTimer", .handler = Led2OffHandler };
static LP_TIMER buttonPressCheckTimer = { .period = { 0, 1000000 }, .name = "buttonPressCheckTimer", .handler = ButtonPressCheckHandler };
static LP_TIMER networkConnectionStatusTimer = { .period = { 5, 0 }, .name = "networkConnectionStatusTimer", .handler = NetworkConnectionStatusHandler };
static LP_TIMER measureSensorTimer = { .period = { 10, 0 }, .name = "measureSensorTimer", .handler = MeasureSensorHandler };
static LP_TIMER resetDeviceOneShotTimer = { .period = { 0, 0 }, .name = "resetDeviceOneShotTimer", .handler = ResetDeviceHandler };

// Azure IoT Device Twins
static LP_DEVICE_TWIN_BINDING led1BlinkRate = { .twinProperty = "LedBlinkRate", .twinType = LP_TYPE_INT, .handler = DeviceTwinBlinkRateHandler };
static LP_DEVICE_TWIN_BINDING relay1DeviceTwin = { .twinProperty = "Relay1", .twinType = LP_TYPE_BOOL, .handler = DeviceTwinRelay1Handler };
static LP_DEVICE_TWIN_BINDING buttonPressed = { .twinProperty = "ButtonPressed", .twinType = LP_TYPE_STRING };
static LP_DEVICE_TWIN_BINDING deviceResetUtc = { .twinProperty = "DeviceResetUTC", .twinType = LP_TYPE_STRING };

// Azure IoT Direct Methods
static LP_DIRECT_METHOD_BINDING resetDevice = { .methodName = "ResetMethod", .handler = ResetDirectMethodHandler };

// Initialize Sets
LP_PERIPHERAL_GPIO* PeripheralGpioSet[] = { &buttonA, &buttonB, &led1, &led2, &networkConnectedLed, &relay1 };
LP_TIMER* timerSet[] = { &led1BlinkTimer, &led2BlinkOffOneShotTimer, &buttonPressCheckTimer, &networkConnectionStatusTimer, &resetDeviceOneShotTimer, &measureSensorTimer };
LP_DEVICE_TWIN_BINDING* deviceTwinBindingSet[] = { &led1BlinkRate, &buttonPressed, &relay1DeviceTwin, &deviceResetUtc };
LP_DIRECT_METHOD_BINDING* directMethodBindingSet[] = { &resetDevice };


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

	if (lp_connectToAzureIot())
	{
		lp_gpioOn(&networkConnectedLed);
	}
	else
	{
		lp_gpioOff(&networkConnectedLed);
	}
}

/// <summary>
/// Turn on LED2, send message to Azure IoT and set a one shot timer to turn LED2 off
/// </summary>
static void SendMsgLed2On(char* message)
{
	lp_gpioOn(&led2);
	Log_Debug("%s\n", message);
	lp_sendMsg(message);
	lp_setOneShotTimer(&led2BlinkOffOneShotTimer, &led2BlinkPeriod);
}

/// <summary>
/// One shot timer to turn LED2 off
/// </summary>
static void Led2OffHandler(EventLoopTimer* eventLoopTimer)
{
	if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0)
	{
		lp_terminate(ExitCode_ConsumeEventLoopTimeEvent);
		return;
	}
	lp_gpioOff(&led2);
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
	if (lp_readTelemetry(msgBuffer, JSON_MESSAGE_BYTES) > 0)
	{
		SendMsgLed2On(msgBuffer);
	}
}

/// <summary>
/// Read Button LP_PERIPHERAL_GPIO returns pressed state
/// </summary>
static bool IsButtonPressed(LP_PERIPHERAL_GPIO button, GPIO_Value_Type* oldState)
{
	bool isButtonPressed = false;
	GPIO_Value_Type newState;

	if (GPIO_GetValue(button.fd, &newState) != 0)
	{
		lp_terminate(ExitCode_Gpio_Read);
	}
	else
	{
		// Button is pressed if it is low and different than last known state.
		isButtonPressed = (newState != *oldState) && (newState == GPIO_Value_Low);
		*oldState = newState;
	}
	return isButtonPressed;
}

/// <summary>
/// Handler to check for Button Presses
/// </summary>
static void ButtonPressCheckHandler(EventLoopTimer* eventLoopTimer)
{
	static GPIO_Value_Type buttonAState;
	static GPIO_Value_Type buttonBState;

	if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0)
	{
		lp_terminate(ExitCode_ConsumeEventLoopTimeEvent);
		return;
	}

	if (IsButtonPressed(buttonA, &buttonAState))
	{
		Led1BlinkIntervalIndex = (Led1BlinkIntervalIndex + 1) % led1BlinkIntervalsCount;
		lp_changeTimer(&led1BlinkTimer, &led1BlinkIntervals[Led1BlinkIntervalIndex]);

		// Report Device Twins Blink Rate and Button Pressed properties
		lp_deviceTwinReportState(&led1BlinkRate, &Led1BlinkIntervalIndex);		// TwinType = LP_TYPE_INT
		lp_deviceTwinReportState(&buttonPressed, "ButtonA");					// TwinType = LP_TYPE_STRING

		// Send ButtonA Pressed Event
		if (snprintf(msgBuffer, JSON_MESSAGE_BYTES, cstrJsonEvent, cstrEvtButtonA) > 0)
		{
			SendMsgLed2On(msgBuffer);
		}
	}
	if (IsButtonPressed(buttonB, &buttonBState))
	{
		// Report Device Twins Button Pressed properties
		lp_deviceTwinReportState(&buttonPressed, "ButtonB");					// TwinType = LP_TYPE_STRING

		// Send ButtonB Pressed Event
		if (snprintf(msgBuffer, JSON_MESSAGE_BYTES, cstrJsonEvent, cstrEvtButtonB) > 0)
		{
			SendMsgLed2On(msgBuffer);
		}
	}
}

/// <summary>
/// Blink Led1 Handler
/// </summary>
static void Led1BlinkHandler(EventLoopTimer* eventLoopTimer)
{
	static bool blinkingLedState = false;

	if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0)
	{
		lp_terminate(ExitCode_ConsumeEventLoopTimeEvent);
		return;
	}

	blinkingLedState = !blinkingLedState;

	if (blinkingLedState) { lp_gpioOff(&led1); }
	else { lp_gpioOn(&led1); }
}

/// <summary>
/// Set Blink Rate using Device Twin "LedBlinkRate": {"value": 0}
/// </summary>
static void DeviceTwinBlinkRateHandler(LP_DEVICE_TWIN_BINDING* deviceTwinBinding)
{
	switch (deviceTwinBinding->twinType)
	{
	case LP_TYPE_INT:
		Log_Debug("\nInteger Value '%d'\n", *(int*)deviceTwinBinding->twinState);

		Led1BlinkIntervalIndex = *(int*)deviceTwinBinding->twinState % led1BlinkIntervalsCount;
		lp_changeTimer(&led1BlinkTimer, &led1BlinkIntervals[Led1BlinkIntervalIndex]);

		break;
	case LP_TYPE_BOOL:
		Log_Debug("\nBoolean Value '%d'\n", *(bool*)deviceTwinBinding->twinState);
		// Your implementation goes here - for example turn in light
	case LP_TYPE_FLOAT:
		Log_Debug("\nFloat Value '%f'\n", *(float*)deviceTwinBinding->twinState);
		// Your implementation goes here - for example set a threshold
		break;
	case LP_TYPE_STRING:
		Log_Debug("\nString Value '%s'\n", (char*)deviceTwinBinding->twinState);
		// Your implementation goes here - for example update display
		break;
	default:
		break;
	}
}

/// <summary>
/// Device Twin to control relay "Relay1": {"value": true }, "Relay1": {"value": false },
/// </summary>
static void DeviceTwinRelay1Handler(LP_DEVICE_TWIN_BINDING* deviceTwinBinding)
{
	switch (deviceTwinBinding->twinType)
	{
	case LP_TYPE_BOOL:
		Log_Debug("\nBool Value '%d'\n", *(bool*)deviceTwinBinding->twinState);
		if (*(bool*)deviceTwinBinding->twinState)
		{
			lp_gpioOn(&relay1);
		}
		else
		{
			lp_gpioOff(&relay1);
		}
		break;
	case LP_TYPE_INT:
	case LP_TYPE_FLOAT:
	case LP_TYPE_STRING:
	case LP_TYPE_UNKNOWN:
		break;
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
/// Start Device Power Restart Direct Method 'ResetMethod' {"reset_timer":5}
/// </summary>
static LP_DirectMethodResponseCode ResetDirectMethodHandler(JSON_Object* json, LP_DIRECT_METHOD_BINDING* directMethodBinding, char** responseMsg)
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

	if (seconds > 0 && seconds < 10)
	{
		// Report Device Reset UTC
		lp_deviceTwinReportState(&deviceResetUtc, lp_getCurrentUtc(msgBuffer, sizeof(msgBuffer)));			// LP_TYPE_STRING

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
/// <returns>0 on success, or -1 on failure</returns>
static void InitPeripheralGpiosAndHandlers(void)
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
static void ClosePeripheralGpiosAndHandlers(void)
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