#include "../libs/azure_iot.h"
#include "../libs/globals.h"
#include "../libs/inter_core.h"
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
static void Led2OffHandler(EventLoopTimer* eventLoopTimer);
static void MeasureSensorHandler(EventLoopTimer* eventLoopTimer);
static void NetworkConnectionStatusHandler(EventLoopTimer* eventLoopTimer);
static void ResetDeviceHandler(EventLoopTimer* eventLoopTimer);
static void DeviceTwinRelay1RateHandler(DeviceTwinBinding* deviceTwinBinding);
static DirectMethodResponseCode ResetDirectMethod(JSON_Object* json, DirectMethodBinding* directMethodBinding, char** responseMsg);
static void InterCoreHandler(char* msg);
static void RealTimeCoreHeartBeat(EventLoopTimer* eventLoopTimer);

static char msgBuffer[JSON_MESSAGE_BYTES] = { 0 };
static const char cstrJsonEvent[] = "{\"%s\":\"occurred\"}";
static const struct timespec led2BlinkPeriod = { 0, 300 * 1000 * 1000 };

// GPIO Peripherals
static Peripheral led2 = {
	.fd = -1, .pin = LED2, .direction = OUTPUT, .initialState = GPIO_Value_Low, .invertPin = false,
	.initialise = OpenPeripheral, .name = "led2"
};
static Peripheral networkConnectedLed = {
	.fd = -1, .pin = NETWORK_CONNECTED_LED, .direction = OUTPUT, .initialState = GPIO_Value_Low, .invertPin = false,
	.initialise = OpenPeripheral, .name = "networkConnectedLed"
};
static Peripheral relay1 = {
	.fd = -1, .pin = RELAY, .direction = OUTPUT, .initialState = GPIO_Value_Low, .invertPin = false,
	.initialise = OpenPeripheral, .name = "relay1"
};

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
static Timer measureSensorTimer = {
	.period = { 10, 0 },
	.name = "measureSensorTimer", .timerEventHandler = MeasureSensorHandler
};
static Timer realTimeCoreHeatBeatTimer = {
	.period = { 30, 0 },
	.name = "rtCoreSend", .timerEventHandler = RealTimeCoreHeartBeat
};

// Azure IoT Device Twins
static DeviceTwinBinding buttonPressed = { .twinProperty = "ButtonPressed", .twinType = TYPE_STRING };
static DeviceTwinBinding relay1DeviceTwin = { .twinProperty = "Relay1", .twinType = TYPE_BOOL, .handler = DeviceTwinRelay1RateHandler };

// Azure IoT Direct Methods
static DirectMethodBinding resetDevice = { .methodName = "ResetMethod", .handler = ResetDirectMethod };

// Initialize peripheral, timer, device twin, and direct method sets
DeviceTwinBinding* deviceTwinBindings[] = { &buttonPressed, &relay1DeviceTwin };
DirectMethodBinding* directMethodBindings[] = { &resetDevice };
Peripheral* peripherals[] = { &led2, &networkConnectedLed, &relay1 };
Timer* timers[] = { &led2BlinkOffOneShotTimer, &networkConnectionStatusTimer, &resetDeviceOneShotTimer, &measureSensorTimer, &realTimeCoreHeatBeatTimer };


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
/// Set Relay state using Device Twin "Relay1": {"value": true },
/// </summary>
static void DeviceTwinRelay1RateHandler(DeviceTwinBinding* deviceTwinBinding) {
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
/// Callback handler for Inter-Core Messaging - Does Device Twin Update, and Event Message
/// </summary>
static void InterCoreHandler(char* msg) {
	DeviceTwinReportState(&buttonPressed, msg);					// TwinType = TYPE_STRING

	if (snprintf(msgBuffer, JSON_MESSAGE_BYTES, cstrJsonEvent, msg) > 0) {
		SendMsgLed2On(msgBuffer);
	}
}

/// <summary>
/// Real Time Inter-Core Heartbeat - primarily sends HL Component ID to RT core to enable secure messaging
/// </summary>
static void RealTimeCoreHeartBeat(EventLoopTimer* eventLoopTimer) {
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
/// Close peripherals and handlers.
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