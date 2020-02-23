#include "../libs/MT3620_Grove_Shield/MT3620_Grove_Shield_Library/Grove.h"
#include "../libs/MT3620_Grove_Shield/MT3620_Grove_Shield_Library/Sensors/GroveTempHumiSHT31.h"
#include "../shared/azure_iot.h"
#include "../shared/globals.h"
#include "../shared/inter_core.h"
#include "../shared/peripheral.h"
#include "../shared/utilities.h"
#include <applibs/gpio.h>
#include <applibs/log.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Select Azure Sphere Dev Kit
//#define AVNET_DK 1
//#define SEEED_DK 1
//#define SEEED_MINI_DK 1

#if defined AVNET_DK

#define BUILTIN_LED 4
#define LIGHT_PIN 5
#define RELAY_PIN 16
#define FAN_PIN 34

#elif defined SEEED_DK

// GPIO Pins used in the High Level (HL) Application
#define BUILTIN_LED 19
#define LIGHT_PIN 21
#define RELAY_PIN 0
#define FAN_PIN 4

#endif

#define JSON_MESSAGE_BYTES 100  // Number of bytes to allocate for the JSON telemetry message for IoT Central
static char msgBuffer[JSON_MESSAGE_BYTES] = { 0 };

static int i2cFd;
static void* sht31;

// Forward signatures
static int InitPeripheralsAndHandlers(void);
static void ClosePeripheralsAndHandlers(void);
static void MeasureSensorHandler(EventData* eventData);
static void DeviceTwinHandler(JSON_Object* json, DeviceTwinPeripheral* deviceTwinPeripheral);
static MethodResponseCode SetFanSpeedDirectMethod(JSON_Object* json, DirectMethodPeripheral* directMethodperipheral);
static int InitFanPWM(struct _peripheral* peripheral);
static void InterCoreHandler(char* msg);
static void InterCoreHeartBeat(EventData* eventData);

static DeviceTwinPeripheral relay = {
	.peripheral = {
		.fd = -1, 
		.pin = RELAY_PIN, 
		.initialState = GPIO_Value_Low, 
		.invertPin = false, 
		.initialise = OpenPeripheral, 
		.name = "relay1" },
	.twinState = false,
	.twinProperty = "relay1",
	.handler = DeviceTwinHandler
};

static DeviceTwinPeripheral light = {
	.peripheral = {
		.fd = -1, .pin = LIGHT_PIN, .initialState = GPIO_Value_High, .invertPin = true, .initialise = OpenPeripheral, .name = "led1" },
	.twinState = false,
	.twinProperty = "led1",
	.handler = DeviceTwinHandler
};

static DirectMethodPeripheral fan = {
	.peripheral = {
		.fd = -1, .pin = FAN_PIN, .initialState = GPIO_Value_Low, .invertPin = false, .initialise = InitFanPWM, .name = "fan1" },
	.methodName = "fan1",
	.handler = SetFanSpeedDirectMethod
};

static ActuatorPeripheral sendStatus = {
	.peripheral = {
		.fd = -1, .pin = BUILTIN_LED, .initialState = GPIO_Value_High, .invertPin = true, .initialise = OpenPeripheral, .name = "SendStatus" }
};

static Timer sendTelemetry = {
	.eventData = {.eventHandler = &MeasureSensorHandler },
	.period = { 10, 0 },
	.name = "MeasureSensor"
};

static Timer rtCoreHeatBeat = {
	.eventData = {.eventHandler = &InterCoreHeartBeat },
	.period = { 30, 0 },
	.name = "rtCoreSend"
};

#pragma region define sets for auto initialisation and close

DeviceTwinPeripheral* deviceTwinDevices[] = { &relay, &light };
DirectMethodPeripheral* directMethodDevices[] = { &fan };
ActuatorPeripheral* actuatorDevices[] = { &sendStatus };
Timer* timers[] = { &sendTelemetry, &rtCoreHeatBeat };

#pragma endregion


int main(int argc, char* argv[])
{
	//SetConnectionString("");		// Lab only, do NOT use connection strings in production.
	RegisterTerminationHandler();
	ProcessCmdArgs(argc, argv);
	srand((unsigned int)time(NULL)); // seed the random number generator for fake telemetry

	if (strlen(scopeId) == 0 || strlen(rtAppComponentId) == 0) {
		Log_Debug("ScopeId and rtAppComponentId need to be set in the app_manifest CmdArgs\n");
		return -1;
	}

	Log_Debug("IoT Hub/Central Application starting.\n");

	if (InitPeripheralsAndHandlers() != 0) {
		terminationRequired = true;
	}

	// Main loop
	while (!terminationRequired) {
		if (WaitForEventAndCallHandler(GetEpollFd()) != 0) {
			terminationRequired = true;
		}
	}

	ClosePeripheralsAndHandlers();

	Log_Debug("Application exiting.\n");

	return 0;
}

/// <summary>
///     Reads telemetry and returns the length of JSON data.
/// </summary>
static int ReadTelemetry(void) {
	static int msgId = 0;
	float temperature;
	float humidity;

	if (realTelemetry) {
		GroveTempHumiSHT31_Read(sht31);
		temperature = GroveTempHumiSHT31_GetTemperature(sht31);
		humidity = GroveTempHumiSHT31_GetHumidity(sht31);
	}
	else {
		int rnd = (rand() % 10) - 5;
		temperature = (float)(25.0 + rnd);
		humidity = (float)(50.0 + rnd);
	}

	static const char* MsgTemplate = "{ \"Temperature\": \"%3.2f\", \"Humidity\": \"%3.1f\", \"MsgId\":%d }";
	return snprintf(msgBuffer, JSON_MESSAGE_BYTES, MsgTemplate, temperature, humidity, msgId++);
}

/// <summary>
/// Azure timer event:  Check connection status and send telemetry
/// </summary>
static void MeasureSensorHandler(EventData* eventData)
{
	if (ConsumeTimerFdEvent(sendTelemetry.fd) != 0) {
		terminationRequired = true;
		return;
	}

	GPIO_ON(sendStatus.peripheral); // blink send status LED

	if (ReadTelemetry() > 0) {
		SendMsg(msgBuffer);
	}

	GPIO_OFF(sendStatus.peripheral);
}


/// <summary>
///     Set up SIGTERM termination handler, initialize peripherals, and set up event handlers.
/// </summary>
/// <returns>0 on success, or -1 on failure</returns>
static int InitPeripheralsAndHandlers(void)
{
	if (realTelemetry) { // Initialize Grove Shield and Grove Temperature and Humidity Sensor		
		GroveShield_Initialize(&i2cFd, 115200);
		sht31 = GroveTempHumiSHT31_Open(i2cFd);
	}

	OPEN_PERIPHERAL_SET(actuatorDevices);
	OPEN_PERIPHERAL_SET(deviceTwinDevices);
	OPEN_PERIPHERAL_SET(directMethodDevices);

	START_TIMER_SET(timers);

	EnableInterCoreCommunications(rtAppComponentId, InterCoreHandler);  // Initialize Inter Core Communications
	SendInterCoreMessage("HeartBeat"); // Prime RT Core with Component ID Signature

	EnableDeviceTwins(deviceTwinDevices, NELEMS(deviceTwinDevices));
	EnableDirectMethods(directMethodDevices, NELEMS(directMethodDevices));
	EnableCloudToDevice();

	return 0;
}


/// <summary>
///     Close peripherals and handlers.
/// </summary>
static void ClosePeripheralsAndHandlers(void)
{
	Log_Debug("Closing file descriptors\n");

	STOP_TIMER_SET(timers);

	CLOSE_PERIPHERAL_SET(actuatorDevices);
	CLOSE_PERIPHERAL_SET(deviceTwinDevices);
	CLOSE_PERIPHERAL_SET(directMethodDevices);

	DisableCloudToDevice();

	CloseFdAndPrintError(GetEpollFd(), "Epoll");
}

#pragma Azure IoT Device Twins Supports

/// <summary>
///		This Device Twin Handler assumes the value field is a boolean from a IoT Central Toggle control.
///		To handle other value types just create another handler for the type required - eg float and associate the new handler 
///		with the Digital Twin definition.
/// </summary>
static void DeviceTwinHandler(JSON_Object* json, DeviceTwinPeripheral* deviceTwinPeripheral) {
	deviceTwinPeripheral->twinState = (bool)json_object_get_boolean(json, "value");
	if (deviceTwinPeripheral->twinState) {
		GPIO_ON(deviceTwinPeripheral->peripheral);
	}
	else {
		GPIO_OFF(deviceTwinPeripheral->peripheral);
	}
}

#pragma endregion

#pragma Azure IoT Direct Method support

static int InitFanPWM(struct _peripheral* peripheral) {
	// for you to implement:)
	return 0;
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
		return METHOD_SUCCEEDED;
	}
	else
	{
		snprintf(directMethodperipheral->responseMessage, responseLen, "%s FAILED, speed out of range %d", directMethodperipheral->methodName, speed);
		return METHOD_FAILED;
	}
}

#pragma endregion

#pragma InterCore Communications Support

static void InterCoreHandler(char* msg) {
	static int buttonPressCount = 0;
	const struct timespec sleepTime = { 0, 100000000L };

	// Toggle LED
	if (relay.twinState) { GPIO_OFF(relay.peripheral); }
	else { GPIO_ON(relay.peripheral); }

	nanosleep(&sleepTime, NULL);

	// Return LED to twinState
	if (relay.twinState) { GPIO_ON(relay.peripheral); }
	else { GPIO_OFF(relay.peripheral); }

	if (snprintf(msgBuffer, JSON_MESSAGE_BYTES, "{ \"ButtonPressed\": %d }", ++buttonPressCount) > 0) {
		SendMsg(msgBuffer);
	}
}

/// <summary>
///     Handle send timer event by writing data to the real-time capable application.
/// </summary>
static void InterCoreHeartBeat(EventData* eventData)
{
	static int heartBeatCount = 0;
	char interCoreMsg[30];

	if (ConsumeTimerFdEvent(rtCoreHeatBeat.fd) != 0) {
		terminationRequired = true;
		return;
	}

	if (snprintf(interCoreMsg, sizeof(interCoreMsg), "HeartBeat-%d", heartBeatCount++) > 0) {
		SendInterCoreMessage(interCoreMsg);
	}
}

#pragma endregion