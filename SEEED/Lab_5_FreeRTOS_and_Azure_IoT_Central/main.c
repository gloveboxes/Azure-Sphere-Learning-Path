#include "../oem/board.h"
#include "../shared/azure_iot.h"
#include "../shared/globals.h"
#include "../shared/inter_core.h"
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

// Forward signatures
static int InitPeripheralsAndHandlers(void);
static void ClosePeripheralsAndHandlers(void);
static void Led2OffHandler(EventLoopTimer* eventLoopTimer);
static void MeasureSensorHandler(EventLoopTimer* eventLoopTimer);
static void NetworkConnectionStatusHandler(EventLoopTimer* eventLoopTimer);
static void ResetDeviceHandler(EventLoopTimer* eventLoopTimer);
static DirectMethodResponseCode ResetDirectMethod(JSON_Object* json, DirectMethodBinding* directMethodBinding, char** responseMsg);
static void InterCoreHandler(char* msg);
static void InterCoreHeartBeat(EventLoopTimer* eventLoopTimer);

static char msgBuffer[JSON_MESSAGE_BYTES] = { 0 };

static const char cstrJsonEvent[] = "{\"%s\":\"occurred\"}";

static const struct timespec led2BlinkPeriod = { 0, 300 * 1000 * 1000 };

static int Led1BlinkIntervalIndex = 0;
static const struct timespec led1BlinkIntervals[] = { {0, 125000000}, {0, 250000000}, {0, 500000000}, {0, 750000000}, {1, 0} };
static const int led1BlinkIntervalsCount = NELEMS(led1BlinkIntervals);

// GPIO Peripherals
static Peripheral led2 = {
	.fd = -1, .pin = LED2, .direction = OUTPUT, .initialState = GPIO_Value_High, .invertPin = true,
	.initialise = OpenPeripheral, .name = "led2"
};
static Peripheral networkConnectedLed = {
	.fd = -1, .pin = NETWORK_CONNECTED_LED, .direction = OUTPUT, .initialState = GPIO_Value_High, .invertPin = true,
	.initialise = OpenPeripheral, .name = "networkConnectedLed"
};
//static Peripheral networkDisconnectedLed = {
//	.fd = -1, .pin = NETWORK_DISCONNECTED_LED, .direction = OUTPUT, .initialState = GPIO_Value_Low, .invertPin = true,
//	.initialise = OpenPeripheral, .name = "networkDisconnectedLed"
//};

// Timers
static Timer led2BlinkOffOneShotTimer = {
	.period = { 0, 0 },
	.name = "led2BlinkOffOneShotTimer", .timerEventHandler = Led2OffHandler
};
static Timer networkConnectionStatusTimer = {
	.period = { 5, 0 },
	.name = "networkConnectionStatusTimer", .timerEventHandler = NetworkConnectionStatusHandler
};
static Timer resetDeviceOneShotTimer = {
	.period = { 0, 0 },
	.name = "resetDeviceOneShotTimer", .timerEventHandler = ResetDeviceHandler
};
static Timer measureSensorTimer = {.period = { 10, 0 }, 
	.name = "measureSensorTimer", .timerEventHandler = MeasureSensorHandler
};
static Timer rtCoreHeatBeat = { .period = { 30, 0 }, .name = "rtCoreSend", .timerEventHandler = InterCoreHeartBeat };

// Azure IoT Device Twins
static DeviceTwinBinding buttonPressed = { .twinProperty = "ButtonPressed", .twinType = TYPE_STRING };

// Azure IoT Direct Methods
static DirectMethodBinding resetDevice = { .methodName = "ResetMethod", .handler = ResetDirectMethod };

// Initialize peripheral, timer, device twin, and direct method sets
DeviceTwinBinding* deviceTwinBindings[] = { &buttonPressed };
DirectMethodBinding* directMethodBindings[] = { &resetDevice };
Peripheral* peripherals[] = { &led2, &networkConnectedLed };
Timer* timers[] = { &led2BlinkOffOneShotTimer, &networkConnectionStatusTimer, &resetDeviceOneShotTimer, &measureSensorTimer, &rtCoreHeatBeat };


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
		//Gpio_Off(&networkDisconnectedLed);
	}
	else {
		Gpio_Off(&networkConnectedLed);
		//Gpio_On(&networkDisconnectedLed);
	}
}

/// <summary>
/// Turn on LED2, send message to Azure IoT and set a one shot timer to turn LED2 off
/// </summary>
static void SendMsgLed2On(char* message) {
	Gpio_On(&led2);
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
		Log_Debug(msgBuffer);
		SendMsgLed2On(msgBuffer);
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

static void InterCoreHandler(char* msg) {
	DeviceTwinReportState(&buttonPressed, msg);					// TwinType = TYPE_STRING

	if (snprintf(msgBuffer, JSON_MESSAGE_BYTES, cstrJsonEvent, msg) > 0) {
		SendMsgLed2On(msgBuffer);
	}
}

/// <summary>
///     Handle send timer event by writing data to the real-time capable application.
/// </summary>
static void InterCoreHeartBeat(EventLoopTimer* eventLoopTimer) {
	static int heartBeatCount = 0;
	char interCoreMsg[30];

	if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0) {
		Terminate();
		return;
	}

	if (snprintf(interCoreMsg, sizeof(interCoreMsg), "HeartBeat-%d", heartBeatCount++) > 0) {
		SendInterCoreMessage(interCoreMsg);
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

	EnableInterCoreCommunications(rtAppComponentId, InterCoreHandler);  // Initialize Inter Core Communications
	SendInterCoreMessage("HeartBeat"); // Prime RT Core with Component ID Signature

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