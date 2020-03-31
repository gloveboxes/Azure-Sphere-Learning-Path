#include "../libs/azure_iot.h"
#include "../libs/globals.h"
#include "../libs/peripheral.h"
#include "../libs/terminate.h"
#include "../libs/timer.h"
#include "../oem/board.h"
#include "applibs_versions.h"
#include "exit_codes.h"
#include <applibs/gpio.h>
#include <applibs/log.h>
#include <applibs/powermanagement.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

#define JSON_MESSAGE_BYTES 128  // Number of bytes to allocate for the JSON telemetry message for IoT Central

// Forward signatures
static int InitPeripheralsAndHandlers(void);
static void ClosePeripheralsAndHandlers(void);
static void Led1BlinkHandler(EventLoopTimer* eventLoopTimer);
static void Led2OffHandler(EventLoopTimer* eventLoopTimer);
static void MeasureSensorHandler(EventLoopTimer* eventLoopTimer);
static void ButtonPressCheckHandler(EventLoopTimer* eventLoopTimer);
static void VirtualButtonsHandler(EventLoopTimer* eventLoopTimer);
static void NetworkConnectionStatusHandler(EventLoopTimer* eventLoopTimer);
static void ResetDeviceHandler(EventLoopTimer* eventLoopTimer);
static void DeviceTwinBlinkRateHandler(DeviceTwinBinding* deviceTwinBinding);
static void DeviceTwinRelay1Handler(DeviceTwinBinding* deviceTwinBinding);
static DirectMethodResponseCode ResetDirectMethodHandler(JSON_Object* json, DirectMethodBinding* directMethodBinding, char** responseMsg);

static char msgBuffer[JSON_MESSAGE_BYTES] = { 0 };

static const char cstrJsonEvent[] = "{\"%s\":\"occurred\"}";
static const char cstrEvtButtonB[] = "ButtonB";
static const char cstrEvtButtonA[] = "ButtonA";

static const struct timespec led2BlinkPeriod = { 0, 300 * 1000 * 1000 };

static int Led1BlinkIntervalIndex = 0;
static const struct timespec led1BlinkIntervals[] = { {0, 125000000}, {0, 250000000}, {0, 500000000}, {0, 750000000}, {1, 0} };
static const int led1BlinkIntervalsCount = NELEMS(led1BlinkIntervals);

// GPIO Input Peripherals
static Peripheral buttonA = { .pin = BUTTON_A, .direction = INPUT, .initialise = OpenPeripheral, .name = "buttonA" };
static Peripheral buttonB = { .pin = BUTTON_B, .direction = INPUT, .initialise = OpenPeripheral, .name = "buttonB" };

// GPIO Output Peripherals
static Peripheral led1 = {
	.pin = LED1, .direction = OUTPUT, .initialState = GPIO_Value_Low, .invertPin = true,
	.initialise = OpenPeripheral, .name = "led1"
};
static Peripheral led2 = {
	.pin = LED2, .direction = OUTPUT, .initialState = GPIO_Value_Low, .invertPin = true,
	.initialise = OpenPeripheral, .name = "led2"
};
static Peripheral networkConnectedLed = {
	.pin = NETWORK_CONNECTED_LED, .direction = OUTPUT, .initialState = GPIO_Value_Low, .invertPin = true,
	.initialise = OpenPeripheral, .name = "networkConnectedLed"
};
static Peripheral relay1 = {
	.pin = RELAY, .direction = OUTPUT, .initialState = GPIO_Value_Low, .invertPin = false,
	.initialise = OpenPeripheral, .name = "relay1"
};

// Timers
static Timer led1BlinkTimer = { .period = { 0, 125000000 }, .name = "led1BlinkTimer", .handler = Led1BlinkHandler };
static Timer led2BlinkOffOneShotTimer = { .period = { 0, 0 }, .name = "led2BlinkOffOneShotTimer", .handler = Led2OffHandler };
static Timer buttonPressCheckTimer = { .period = { 0, 1000000 }, .name = "buttonPressCheckTimer", .handler = ButtonPressCheckHandler };
static Timer networkConnectionStatusTimer = { .period = { 5, 0 }, .name = "networkConnectionStatusTimer", .handler = NetworkConnectionStatusHandler };
static Timer measureSensorTimer = { .period = { 10, 0 }, .name = "measureSensorTimer", .handler = MeasureSensorHandler };
static Timer resetDeviceOneShotTimer = { .period = { 0, 0 }, .name = "resetDeviceOneShotTimer", .handler = ResetDeviceHandler };
static Timer virtualButtonsTimer = { .period = { 5, 0 }, .name = "virtualButtonsTimer", .handler = VirtualButtonsHandler };

// Azure IoT Device Twins
static DeviceTwinBinding led1BlinkRate = { .twinProperty = "LedBlinkRate", .twinType = TYPE_INT, .handler = DeviceTwinBlinkRateHandler };
static DeviceTwinBinding relay1DeviceTwin = { .twinProperty = "Relay1", .twinType = TYPE_BOOL, .handler = DeviceTwinRelay1Handler };
static DeviceTwinBinding buttonPressed = { .twinProperty = "ButtonPressed", .twinType = TYPE_STRING };
static DeviceTwinBinding deviceResetUtc = { .twinProperty = "DeviceResetUTC", .twinType = TYPE_STRING };

// Azure IoT Direct Methods
static DirectMethodBinding resetDevice = { .methodName = "ResetMethod", .handler = ResetDirectMethodHandler };

// Initialize Sets
Peripheral* peripheralSet[] = { &buttonA, &buttonB, &led1, &led2, &networkConnectedLed, &relay1 };
Timer* timerSet[] = { &led1BlinkTimer, &led2BlinkOffOneShotTimer, &buttonPressCheckTimer, &networkConnectionStatusTimer, &resetDeviceOneShotTimer, &measureSensorTimer, &virtualButtonsTimer };
DeviceTwinBinding* deviceTwinBindingSet[] = { &led1BlinkRate, &buttonPressed, &relay1DeviceTwin, &deviceResetUtc };
DirectMethodBinding* directMethodBindingSet[] = { &resetDevice };


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

/// <summary>
/// Check status of connection to Azure IoT
/// </summary>
static void NetworkConnectionStatusHandler(EventLoopTimer* eventLoopTimer) {
	if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0) {
		Terminate();
		return;
	}

	if (ConnectToAzureIot()) {
		Gpio_On(&networkConnectedLed);
	}
	else {
		Gpio_Off(&networkConnectedLed);
	}
}

/// <summary>
/// Turn on LED2, send message to Azure IoT and set a one shot timer to turn LED2 off
/// </summary>
static void SendMsgLed2On(char* message) {
	Gpio_On(&led2);
	Log_Debug("%s\n", message);
	SendMsg(message);
	SetOneShotTimer(&led2BlinkOffOneShotTimer, &led2BlinkPeriod);
}

/// <summary>
/// One shot timer to turn LED2 off
/// </summary>
static void Led2OffHandler(EventLoopTimer* eventLoopTimer) {
	if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0) {
		Terminate();
		return;
	}
	Gpio_Off(&led2);
}

/// <summary>
/// Read sensor and send to Azure IoT
/// </summary>
static void MeasureSensorHandler(EventLoopTimer* eventLoopTimer) {
	if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0) {
		Terminate();
		return;
	}
	if (ReadTelemetry(msgBuffer, JSON_MESSAGE_BYTES) > 0) {
		SendMsgLed2On(msgBuffer);
	}
}

/// <summary>
/// Read Button Peripheral returns pressed state
/// </summary>
static bool IsButtonPressed(Peripheral button, GPIO_Value_Type* oldState) {
	bool isButtonPressed = false;
	GPIO_Value_Type newState;

	if (GPIO_GetValue(button.fd, &newState) != 0) {
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

		Led1BlinkIntervalIndex = (Led1BlinkIntervalIndex + 1) % led1BlinkIntervalsCount;
		ChangeTimer(&led1BlinkTimer, &led1BlinkIntervals[Led1BlinkIntervalIndex]);

		DeviceTwinReportState(&led1BlinkRate, &Led1BlinkIntervalIndex);		// TwinType = TYPE_INT
		DeviceTwinReportState(&buttonPressed, "ButtonA");					// TwinType = TYPE_STRING

		if (snprintf(msgBuffer, JSON_MESSAGE_BYTES, cstrJsonEvent, cstrEvtButtonA) > 0) {
			SendMsgLed2On(msgBuffer);
		}
	}
	if (IsButtonPressed(buttonB, &buttonBState)) {

		DeviceTwinReportState(&buttonPressed, "ButtonB");					// TwinType = TYPE_STRING

		if (snprintf(msgBuffer, JSON_MESSAGE_BYTES, cstrJsonEvent, cstrEvtButtonB) > 0) {
			SendMsgLed2On(msgBuffer);
		}
	}
}

/// <summary>
/// On the Seeed Studio Azure Sphere Mini there are no built in button - so this function emulates button presses
/// </summary>
static void VirtualButtonsHandler(EventLoopTimer* eventLoopTimer) {
	static bool toggle = false;

	if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0) {
		Terminate();
		return;
	}

	if (toggle) {
		Led1BlinkIntervalIndex = (Led1BlinkIntervalIndex + 1) % led1BlinkIntervalsCount;
		ChangeTimer(&led1BlinkTimer, &led1BlinkIntervals[Led1BlinkIntervalIndex]);

		DeviceTwinReportState(&led1BlinkRate, &Led1BlinkIntervalIndex);		// TwinType = TYPE_INT
		DeviceTwinReportState(&buttonPressed, "ButtonA");					// TwinType = TYPE_STRING

		if (snprintf(msgBuffer, JSON_MESSAGE_BYTES, cstrJsonEvent, cstrEvtButtonA) > 0) {
			SendMsgLed2On(msgBuffer);
		}
	}
	else {
		DeviceTwinReportState(&buttonPressed, "ButtonB");					// TwinType = TYPE_STRING

		if (snprintf(msgBuffer, JSON_MESSAGE_BYTES, cstrJsonEvent, cstrEvtButtonB) > 0) {
			SendMsgLed2On(msgBuffer);
		}
	}

	toggle = !toggle;
}

/// <summary>
/// Blink Led1 Handler
/// </summary>
static void Led1BlinkHandler(EventLoopTimer* eventLoopTimer) {
	static bool blinkingLedState = false;

	if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0) {
		Terminate();
		return;
	}

	blinkingLedState = !blinkingLedState;

	if (blinkingLedState) { Gpio_Off(&led1); }
	else { Gpio_On(&led1); }
}

/// <summary>
/// Set Blink Rate using Device Twin "LedBlinkRate": {"value": 0}
/// </summary>
static void DeviceTwinBlinkRateHandler(DeviceTwinBinding* deviceTwinBinding) {
	switch (deviceTwinBinding->twinType) {
	case TYPE_INT:
		Log_Debug("\nInteger Value '%d'\n", *(int*)deviceTwinBinding->twinState);

		Led1BlinkIntervalIndex = *(int*)deviceTwinBinding->twinState % led1BlinkIntervalsCount;
		ChangeTimer(&led1BlinkTimer, &led1BlinkIntervals[Led1BlinkIntervalIndex]);

		break;
	case TYPE_BOOL:
		Log_Debug("\nBoolean Value '%d'\n", *(bool*)deviceTwinBinding->twinState);
		// Your implementation goes here - for example turn in light
	case TYPE_FLOAT:
		Log_Debug("\nFloat Value '%f'\n", *(float*)deviceTwinBinding->twinState);
		// Your implementation goes here - for example set a threshold
		break;
	case TYPE_STRING:
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
static void DeviceTwinRelay1Handler(DeviceTwinBinding* deviceTwinBinding) {
	switch (deviceTwinBinding->twinType) {
	case TYPE_BOOL:
		Log_Debug("\nBool Value '%d'\n", *(bool*)deviceTwinBinding->twinState);
		if (*(bool*)deviceTwinBinding->twinState) {
			Gpio_On(&relay1);
		}
		else {
			Gpio_Off(&relay1);
		}
		break;
	case TYPE_INT:
	case TYPE_FLOAT:
	case TYPE_STRING:
	case TYPE_UNKNOWN:
		break;
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
/// Start Device Power Restart Direct Method 'ResetMethod' {"reset_timer":5}
/// </summary>
static DirectMethodResponseCode ResetDirectMethodHandler(JSON_Object* json, DirectMethodBinding* directMethodBinding, char** responseMsg) {
	const char propertyName[] = "reset_timer";
	const size_t responseLen = 60; // Allocate and initialize a response message buffer. The calling function is responsible for the freeing memory
	static struct timespec period;

	*responseMsg = (char*)malloc(responseLen);
	memset(*responseMsg, 0, responseLen);

	if (!json_object_has_value_of_type(json, propertyName, JSONNumber)) {
		return METHOD_FAILED;
	}
	int seconds = (int)json_object_get_number(json, propertyName);

	if (seconds > 0 && seconds < 10) {

		// Report Device Reset UTC
		DeviceTwinReportState(&deviceResetUtc, GetCurrentUtc(msgBuffer, sizeof(msgBuffer)));			// TYPE_STRING

		// Create Direct Method Response
		snprintf(*responseMsg, responseLen, "%s called. Reset in %d seconds", directMethodBinding->methodName, seconds);

		// Set One Shot Timer
		period = (struct timespec){ .tv_sec = seconds, .tv_nsec = 0 };
		SetOneShotTimer(&resetDeviceOneShotTimer, &period);

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

	OpenPeripheralSet(peripheralSet, NELEMS(peripheralSet));
	OpenDeviceTwinSet(deviceTwinBindingSet, NELEMS(deviceTwinBindingSet));
	OpenDirectMethodSet(directMethodBindingSet, NELEMS(directMethodBindingSet));

	StartTimerSet(timerSet, NELEMS(timerSet));

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