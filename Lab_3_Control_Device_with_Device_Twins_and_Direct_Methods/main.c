#include "../shared/azure_iot.h"
#include "../shared/globals.h"
#include "../shared/peripheral.h"
#include "../shared/terminate.h"
#include "../shared/timer.h"
#include "applibs_versions.h"
#include <applibs/gpio.h>
#include <applibs/log.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

#define AVNET 1
//#define SEEED 1
//#define SEEED_MINI 1

#if defined AVNET
#include "../shared/avnet_sk/avnet_sk.h"
#endif // AVNET

#define JSON_MESSAGE_BYTES 128  // Number of bytes to allocate for the JSON telemetry message for IoT Central
static char msgBuffer[JSON_MESSAGE_BYTES] = { 0 };

// Forward signatures
static int InitPeripheralsAndHandlers(void);
static void ClosePeripheralsAndHandlers(void);
static void MeasureSensorHandler(EventLoopTimer* eventData);
static void DeviceTwinHandler(DeviceTwinPeripheral* deviceTwinPeripheral);
static MethodResponseCode SetFanSpeedDirectMethod(JSON_Object* json, DirectMethodPeripheral* directMethodperipheral);


static DeviceTwinPeripheral relay = {
	.peripheral = {
		.fd = -1, .pin = RELAY_PIN, .initialState = GPIO_Value_Low, .invertPin = false, .initialise = OpenPeripheral, .name = "relay1" },
	.twinProperty = "relay1",
	.twinType = TYPE_BOOL,
	.handler = DeviceTwinHandler
};

static DeviceTwinPeripheral light = {
	.peripheral = {
		.fd = -1, .pin = LIGHT_PIN, .initialState = GPIO_Value_High, .invertPin = true, .initialise = OpenPeripheral, .name = "led1" },
	.twinProperty = "led1",
	.twinType = TYPE_BOOL,
	.handler = DeviceTwinHandler
};

static DirectMethodPeripheral fan = {
	.methodName = "fan1",
	.handler = SetFanSpeedDirectMethod
};

static Peripheral sendStatusLed = {
	.fd = -1, .pin = BUILTIN_LED, .initialState = GPIO_Value_High, .invertPin = true, .initialise = OpenPeripheral, .name = "SendStatus"
};

static Timer sendTelemetry = {
	.period = { 10, 0 },
	.name = "MeasureSensor",
	.timerEventHandler = &MeasureSensorHandler
};

#pragma region define sets for auto initialization and close

DeviceTwinPeripheral* deviceTwinDevices[] = { &relay, &light };
DirectMethodPeripheral* directMethodDevices[] = { &fan };
Peripheral* peripherals[] = { &sendStatusLed };
Timer* timers[] = { &sendTelemetry };

#pragma endregion
// end define sets for auto initialization and close


int main(int argc, char* argv[])
{
	RegisterTerminationHandler();
	ProcessCmdArgs(argc, argv);
	srand((unsigned int)time(NULL)); // seed the random number generator for fake telemetry

	if (strlen(scopeId) == 0) {
		Log_Debug("ScopeId needs to be set in the app_manifest CmdArgs\n");
		return -1;
	}

	Log_Debug("IoT Hub/Central Application starting.\n");

	if (InitPeripheralsAndHandlers() != 0) {
		Terminate();
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
	return 0;
}


/// <summary>
/// Azure timer event:  Check connection status and send telemetry
/// </summary>
static void MeasureSensorHandler(EventLoopTimer* eventLoopTimer)
{
	if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0) {
		Terminate();
		return;
	}

	GPIO_ON(sendStatusLed); // blink send status LED

	if (ReadTelemetry(msgBuffer, JSON_MESSAGE_BYTES) > 0) {
		Log_Debug("%s\n", msgBuffer);
		SendMsg(msgBuffer);
	}

	GPIO_OFF(sendStatusLed);
}


/// <summary>
///     Set up SIGTERM termination handler, initialize peripherals, and set up event handlers.
/// </summary>
/// <returns>0 on success, or -1 on failure</returns>
static int InitPeripheralsAndHandlers(void)
{
	RegisterBoard();  // Avnet & Seeed dev kits

	RegisterPeripheralSet(peripherals, NELEMS(peripherals));
	RegisterDeviceTwinSet(deviceTwinDevices, NELEMS(deviceTwinDevices));
	RegisterDirectMethodSet(directMethodDevices, NELEMS(directMethodDevices));

	RegisterTimerSet(timers, NELEMS(timers));

	EnableCloudToDevice();

	return 0;
}

/// <summary>
///     Close peripherals and handlers.
/// </summary>
static void ClosePeripheralsAndHandlers(void)
{
	Log_Debug("Closing file descriptors\n");

	CloseTimerSet();
	DisableCloudToDevice();

	ClosePeripheralSet();
	CloseDeviceTwinSet();
	CloseDirectMethodSet();	

	CloseBoard();	// Avnet & Seeed dev kits

	CloseTimerEventLoop();
}


static void DeviceTwinHandler(DeviceTwinPeripheral* deviceTwinPeripheral) {
	switch (deviceTwinPeripheral->twinType)
	{
	case TYPE_BOOL:
		if (*(bool*)deviceTwinPeripheral->twinState) {
			GPIO_ON(deviceTwinPeripheral->peripheral);
		}
		else {
			GPIO_OFF(deviceTwinPeripheral->peripheral);
		}
		break;
	case TYPE_INT:
		Log_Debug("\nInteger Value '%d'\n", *(int*)deviceTwinPeripheral->twinState);
		// Your implementation goes here - for example change the sensor measure rate
		break;
	case TYPE_FLOAT:
		Log_Debug("\nFloat Value '%f'\n", *(float*)deviceTwinPeripheral->twinState);
		// Your implementation goes here - for example set a threshold
		break;
	case TYPE_STRING:
		Log_Debug("\nString Value '%s'\n", (char*)deviceTwinPeripheral->twinState);
		// Your implementation goes here - for example update display
		break;
	default:
		break;
	}
}


static MethodResponseCode SetFanSpeedDirectMethod(JSON_Object* json, DirectMethodPeripheral* directMethodperipheral) {
	// Sample implementation - doesn't do anything other than returning a response message and status

	// Allocate and initialize a response message buffer. The calling function is responsible for the freeing memory
	const size_t responseLen = 40;
	directMethodperipheral->responseMessage = (char*)malloc(responseLen);
	memset(directMethodperipheral->responseMessage, 0, responseLen);

	int speed = (int)json_object_get_number(json, "speed");

	if (speed >= 0 && speed <= 100) {
		snprintf(directMethodperipheral->responseMessage, responseLen, "%s succeeded, speed set to %d", directMethodperipheral->methodName, speed);
		Log_Debug("\nDirect Method Response '%s'\n", directMethodperipheral->responseMessage);
		return METHOD_SUCCEEDED;
	}
	else
	{
		snprintf(directMethodperipheral->responseMessage, responseLen, "%s FAILED, speed out of range %d", directMethodperipheral->methodName, speed);
		Log_Debug("\nDirect Method Response '%s'\n", directMethodperipheral->responseMessage);
		return METHOD_FAILED;
	}
}