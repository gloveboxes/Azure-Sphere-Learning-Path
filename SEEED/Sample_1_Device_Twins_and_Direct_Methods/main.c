#include "../shared/azure_iot.h"
#include "../shared/globals.h"
#include "../shared/oem/board.h"
#include "../shared/peripheral.h"
#include "../shared/terminate.h"
#include "../shared/timer.h"
#include "applibs_versions.h"
#include "exit_codes.h"
#include <applibs/gpio.h>
#include <applibs/log.h>
#include <applibs/powermanagement.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

#define JSON_MESSAGE_BYTES 128  // Number of bytes to allocate for the JSON telemetry message for IoT Central
static char msgBuffer[JSON_MESSAGE_BYTES] = { 0 };

// Forward signatures
static int InitPeripheralsAndHandlers(void);
static void ClosePeripheralsAndHandlers(void);
static void BlinkLed1Handler(EventLoopTimer* eventLoopTimer);
static void blinkLed2Handler(EventLoopTimer* eventLoopTimer);
static void ButtonPressCheckHandler(EventLoopTimer* eventLoopTimer);
static void AzureConnectionStatusHandler(EventLoopTimer* eventLoopTimer);
static void ResetDeviceHandler(EventLoopTimer* eventLoopTimer);
static void DeviceTwinBlinkRateHandler(DeviceTwinBinding* deviceTwinBinding);
static DirectMethodResponseCode ResetDirectMethod(JSON_Object* json, DirectMethodBinding* directMethodBinding, char** responseMsg);

static const char cstrJsonEvent[] = "{\"%s\":\"occurred\"}";
static const char cstrEvtButtonB[] = "buttonB";
static const char cstrEvtButtonA[] = "buttonA";
static const struct timespec defaultBlinkTimeLed2 = { 0, 300 * 1000 * 1000 };

static int blinkIntervalIndex = 0;
static const struct timespec blinkIntervals[] = { {0, 125000000}, {0, 250000000}, {0, 500000000} };
static const int blinkIntervalsCount = NELEMS(blinkIntervals);

static DeviceTwinBinding ledBlinkRate = { .twinProperty = "LedBlinkRateProperty", .twinType = TYPE_INT, .handler = DeviceTwinBlinkRateHandler };
static DirectMethodBinding resetDevice = { .methodName = "ResetMethod", .handler = ResetDirectMethod };

static Peripheral buttonA = { .fd = -1, .pin = MT3620_RDB_BUTTON_A, .direction = INPUT, .initialise = OpenPeripheral, .name = "buttonA" };
static Peripheral buttonB = { .fd = -1, .pin = MT3620_RDB_BUTTON_B, .direction = INPUT, .initialise = OpenPeripheral, .name = "buttonB" };

static Peripheral led1 = {
	.fd = -1, .pin = MT3620_RDB_LED1_BLUE, .direction = OUTPUT, .initialState = GPIO_Value_High, .invertPin = true,
	.initialise = OpenPeripheral, .name = "led1"
};
static Peripheral led2 = {
	.fd = -1, .pin = MT3620_RDB_LED2_GREEN, .direction = OUTPUT, .initialState = GPIO_Value_High, .invertPin = true,
	.initialise = OpenPeripheral, .name = "led2"
};
static Peripheral networkConnectedLed = {
	.fd = -1, .pin = MT3620_RDB_NETWORKING_LED_GREEN, .direction = OUTPUT, .initialState = GPIO_Value_High, .invertPin = true,
	.initialise = OpenPeripheral, .name = "networkConnectedLed"
};
static Peripheral networkDisconnectedLed = {
	.fd = -1, .pin = MT3620_RDB_NETWORKING_LED_BLUE, .direction = OUTPUT, .initialState = GPIO_Value_Low, .invertPin = true,
	.initialise = OpenPeripheral, .name = "networkDisconnectedLed"
};

static Timer led1BlinkTimer = {
	.period = { 0, 125000000 },
	.name = "BlinkLedTimer", .timerEventHandler = BlinkLed1Handler
};
static Timer led2BlinkOneShotTimer = {
	.period = { 0, 0 },
	.name = "led2BlinkOneShotTimer", .timerEventHandler = blinkLed2Handler
};
static Timer buttonPressCheckTimer = {
	.period = { 0, 1000000 },
	.name = "ButtonPressCheckTimer", .timerEventHandler = ButtonPressCheckHandler
};
static Timer azureConnectionStatusTimer = {
	.period = { 5, 0 },
	.name = "azureConnectionStatusTimer", .timerEventHandler = AzureConnectionStatusHandler
};
static Timer resetDeviceOneShotTimer = {
	.period = { 0, 0 },
	.name = "azureConnectionStatusTimer", .timerEventHandler = ResetDeviceHandler
};


#pragma region define sets for auto initialization and close

DeviceTwinBinding* deviceTwinBindings[] = { &ledBlinkRate };
DirectMethodBinding* directMethodBindings[] = { &resetDevice };
Peripheral* peripherals[] = { &buttonA, &buttonB, &led1, &led2, &networkConnectedLed, &networkDisconnectedLed };
Timer* timers[] = { &led1BlinkTimer, &led2BlinkOneShotTimer, &buttonPressCheckTimer, &azureConnectionStatusTimer, &resetDeviceOneShotTimer };

#pragma endregion


int main(int argc, char* argv[]) {
	RegisterTerminationHandler();
	ProcessCmdArgs(argc, argv);

	if (strlen(scopeId) == 0) {
		Log_Debug("ScopeId needs to be set in the app_manifest CmdArgs\n");
		return ExitCode_Missing_ID_Scope;
	}

	if (InitPeripheralsAndHandlers() != 0) {
		return ExitCode_Init_Failed;
	}

	// Main loop
	while (!IsTerminationRequired()) {
		int result = EventLoop_Run(GetTimerEventLoop(), -1, true);
		// Continue if interrupted by signal, e.g. due to breakpoint being set.
		if (result == -1 && errno != EINTR) {
			Terminate();
		}
	}

	ClosePeripheralsAndHandlers();

	Log_Debug("Application exiting.\n");
	return GetTerminationExitCode();
}

void SendMsgWithBlink(char* message) {
	Gpio_On(&led2);
	SendMsg(message);
	SetOneShotTimer(&led2BlinkOneShotTimer, &defaultBlinkTimeLed2);
}

/// <summary>
/// Read Button Peripheral returns pressed state
/// </summary>
static bool IsButtonPressed(Peripheral button, GPIO_Value_Type* oldState) {
	bool isButtonPressed = false;
	GPIO_Value_Type newState;

	int result = GPIO_GetValue(button.fd, &newState);
	if (result != 0) {
		Log_Debug("ERROR: Could not read button GPIO: %s (%d).\n", strerror(errno), errno);
		Terminate();
	}
	else {
		// Button is pressed if it is low and different than last known state.
		isButtonPressed = (newState != *oldState) && (newState == GPIO_Value_Low);
		*oldState = newState;
	}
	return isButtonPressed;
}

/// <summary>
/// Handler to check for Button Presses
/// </summary>
static void ButtonPressCheckHandler(EventLoopTimer* eventLoopTimer) {
	static GPIO_Value_Type buttonAState;
	static GPIO_Value_Type buttonBState;

	if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0) {
		Terminate();
		return;
	}

	if (IsButtonPressed(buttonA, &buttonAState)) {

		blinkIntervalIndex = (blinkIntervalIndex + 1) % blinkIntervalsCount;
		ChangeTimer(&led1BlinkTimer, &blinkIntervals[blinkIntervalIndex]);

		*(int*)ledBlinkRate.twinState = blinkIntervalIndex;		// Update TwinState first
		TwinReportState(&ledBlinkRate);							// Report TwinState second

		if (snprintf(msgBuffer, JSON_MESSAGE_BYTES, cstrJsonEvent, cstrEvtButtonA) > 0) {
			SendMsgWithBlink(msgBuffer);
		}

		if (ReadTelemetry(msgBuffer, JSON_MESSAGE_BYTES) > 0) {
			Log_Debug("%s\n", msgBuffer);
			SendMsgWithBlink(msgBuffer);
		}
	}

	if (IsButtonPressed(buttonB, &buttonBState)) {
		if (snprintf(msgBuffer, JSON_MESSAGE_BYTES, cstrJsonEvent, cstrEvtButtonB) > 0) {
			SendMsgWithBlink(msgBuffer);
		}
	}
}

/// <summary>
/// Blink Led1 Handler
/// </summary>
static void BlinkLed1Handler(EventLoopTimer* eventLoopTimer) {
	static bool blinkingLedState = false;

	if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0) {
		Terminate();
		return;
	}

	blinkingLedState = !blinkingLedState;

	if (blinkingLedState) { Gpio_Off(&led1); }
	else { Gpio_On(&led1); }
}

static void blinkLed2Handler(EventLoopTimer* eventLoopTimer) {
	if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0) {
		Terminate();
		return;
	}
	Gpio_Off(&led2);
}

/// <summary>
/// Check status of connection to Azure IoT
/// </summary>
static void AzureConnectionStatusHandler(EventLoopTimer* eventLoopTimer) {
	if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0) {
		Terminate();
		return;
	}

	if (ConnectToAzureIot()) {
		Gpio_On(&networkConnectedLed);
		Gpio_Off(&networkDisconnectedLed);
	}
	else {
		Gpio_Off(&networkConnectedLed);
		Gpio_On(&networkDisconnectedLed);
	}
}

/// <summary>
/// Reset the Device
/// </summary>
static void ResetDeviceHandler(EventLoopTimer* eventLoopTimer) {
	if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0) {
		Terminate();
		return;
	}
	PowerManagement_ForceSystemReboot();
}

/// <summary>
/// Set Blink Rate using Device Twin "LedBlinkRateProperty": {"value": 2 },
/// </summary>
static void DeviceTwinBlinkRateHandler(DeviceTwinBinding* deviceTwinBinding) {
	switch (deviceTwinBinding->twinType) {
	case TYPE_INT:
		Log_Debug("\nInteger Value '%d'\n", *(int*)deviceTwinBinding->twinState);

		blinkIntervalIndex = *(int*)deviceTwinBinding->twinState % blinkIntervalsCount;
		ChangeTimer(&led1BlinkTimer, &blinkIntervals[blinkIntervalIndex]);
		break;

	case TYPE_BOOL:
	case TYPE_FLOAT:
	case TYPE_STRING:
	case TYPE_UNKNOWN:
		break;
	}
}

/// <summary>
/// Start Device Power Restart Direct Method 'ResetMethod' {"reset_timer":5}
/// </summary>
static DirectMethodResponseCode ResetDirectMethod(JSON_Object* json, DirectMethodBinding* directMethodBinding, char** responseMsg) {
	const char propertyName[] = "reset_timer";
	const size_t responseLen = 60; // Allocate and initialize a response message buffer. The calling function is responsible for the freeing memory
	static struct timespec period;

	*responseMsg = (char*)malloc(responseLen);
	memset(*responseMsg, 0, responseLen);

	if (!json_object_has_value_of_type(json, propertyName, JSONNumber)) {
		return METHOD_FAILED;
	}

	int seconds = (int)json_object_get_number(json, propertyName);

	if (seconds > 1 && seconds < 10) {

		period = (struct timespec){ .tv_sec = seconds, .tv_nsec = 0 };
		SetOneShotTimer(&resetDeviceOneShotTimer, &period);

		snprintf(*responseMsg, responseLen, "%s called. Reset in %d seconds", directMethodBinding->methodName, seconds);
		return METHOD_SUCCEEDED;
	}
	else {
		snprintf(*responseMsg, responseLen, "%s called. Reset Failed. Seconds out of range: %d", directMethodBinding->methodName, seconds);
		return METHOD_FAILED;
	}
}

/// <summary>
///  Initialize peripherals, device twins, direct methods, timers.
/// </summary>
/// <returns>0 on success, or -1 on failure</returns>
static int InitPeripheralsAndHandlers(void) {
	InitializeDevKit();

	OpenPeripheralSet(peripherals, NELEMS(peripherals));
	OpenDeviceTwinSet(deviceTwinBindings, NELEMS(deviceTwinBindings));
	OpenDirectMethodSet(directMethodBindings, NELEMS(directMethodBindings));

	StartTimerSet(timers, NELEMS(timers));

	StartCloudToDevice();

	return 0;
}

/// <summary>
///     Close peripherals and handlers.
/// </summary>
static void ClosePeripheralsAndHandlers(void) {
	Log_Debug("Closing file descriptors\n");

	StopTimerSet();
	StopCloudToDevice();

	ClosePeripheralSet();
	CloseDeviceTwinSet();
	CloseDirectMethodSet();

	CloseDevKit();

	StopTimerEventLoop();
}