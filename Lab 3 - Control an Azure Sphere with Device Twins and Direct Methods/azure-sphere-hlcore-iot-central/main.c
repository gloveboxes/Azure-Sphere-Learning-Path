#include "../MT3620_Grove_Shield/MT3620_Grove_Shield_Library/Grove.h"
#include "../MT3620_Grove_Shield/MT3620_Grove_Shield_Library/Sensors/GroveTempHumiSHT31.h"
#include "globals.h"
#include "azure_iot.h"
#include <applibs/gpio.h>
#include <applibs/log.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

// GPIO Pins used in the High Level (HL) Application
#define SEND_STATUS_PIN 19
#define LIGHT_PIN 21
#define RELAY_PIN 0
#define FAN_PIN 4
#define JSON_MESSAGE_BYTES 100  // Number of bytes to allocate for the JSON telemetry message for IoT Central

static char msgBuffer[JSON_MESSAGE_BYTES] = { 0 };
static char rtAppComponentId[RT_APP_COMPONENT_LENGTH];  //initialized from cmdline argument

static int epollFd = -1;
static int i2cFd;
static void* sht31;

// Forward signatures
static void TerminationHandler(int signalNumber);
static int InitPeripheralsAndHandlers(void);
static void ClosePeripheralsAndHandlers(void);
static void SendTelemetryEventHandler(EventData* eventData);
static int OpenPeripheral(Peripheral* peripheral);
static int StartTimer(Timer* timer);
static void DeviceTwinHandler(JSON_Object* json, DeviceTwinPeripheral* deviceTwinPeripheral);
static bool SetFanSpeedDirectMethod(JSON_Object* json, DirectMethodPeripheral* peripheral);
static int InitFanPWM(struct _peripheral* peripheral);

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
	.peripheral = {.fd = -1, .pin = LIGHT_PIN, .initialState = GPIO_Value_High, .invertPin = true, .initialise = OpenPeripheral, .name = "led1" },
	.twinState = false,
	.twinProperty = "led1",
	.handler = DeviceTwinHandler
};

static DirectMethodPeripheral fan = {
	.peripheral = {
		.fd = -1,
		.pin = FAN_PIN,
		.initialState = GPIO_Value_Low,
		.invertPin = false,
		.initialise = InitFanPWM,
		.name = "fan1" },
	.methodName = "fan1",
	.handler = SetFanSpeedDirectMethod
};

static ActuatorPeripheral sendStatus = {
	.peripheral = {.fd = -1, .pin = SEND_STATUS_PIN, .initialState = GPIO_Value_High, .invertPin = true, .initialise = OpenPeripheral, .name = "SendStatus" }
};

static Timer iotClientDoWork = {
	.eventData = {.eventHandler = &AzureDoWorkTimerEventHandler },
	.period = { 1, 0 },
	.name = "DoWork"
};
static Timer sendTelemetry = {
	.eventData = {.eventHandler = &SendTelemetryEventHandler },
	.period = { 10, 0 },
	.name = "MeasureSensor"
};

#pragma region define sets for auto initialisation and close

DeviceTwinPeripheral* deviceTwinDevices[] = { &relay, &light };
DirectMethodPeripheral* directMethodDevices[] = { &fan };
ActuatorPeripheral* actuatorDevices[] = { &sendStatus };
Timer* timers[] = { &iotClientDoWork, &sendTelemetry };

#pragma endregion


int main(int argc, char* argv[])
{
	Log_Debug("IoT Hub/Central Application starting.\n");

	if (argc == 2) {
		Log_Debug("Setting Azure Scope ID %s\n", argv[1]);
		strncpy(scopeId, argv[1], SCOPEID_LENGTH);
	}
	else {
		Log_Debug("ScopeId and RT Core ComponentId need to be set in the app_manifest CmdArgs\n");
		return -1;
	}

	if (InitPeripheralsAndHandlers() != 0) {
		terminationRequired = true;
	}

	// Main loop
	while (!terminationRequired) {
		if (WaitForEventAndCallHandler(epollFd) != 0) {
			terminationRequired = true;
		}
	}

	ClosePeripheralsAndHandlers();

	Log_Debug("Application exiting.\n");

	return 0;
}

/// <summary>
///     Reads telemetry and returns the data as a JSON object.
/// </summary>
static int ReadTelemetry(char eventBuffer[], size_t len) {
	static int msgId = 0;
	GroveTempHumiSHT31_Read(sht31);
	float temperature = GroveTempHumiSHT31_GetTemperature(sht31);
	float humidity = GroveTempHumiSHT31_GetHumidity(sht31);

	static const char* EventMsgTemplate = "{ \"Temperature\": \"%3.2f\", \"Humidity\": \"%3.1f\", \"MsgId\":%d }";
	return snprintf(eventBuffer, len, EventMsgTemplate, temperature, humidity, msgId++);
}

/// <summary>
/// Azure timer event:  Check connection status and send telemetry
/// </summary>
static void SendTelemetryEventHandler(EventData* eventData)
{
	if (ConsumeTimerFdEvent(sendTelemetry.fd) != 0) {
		terminationRequired = true;
		return;
	}

	GPIO_ON(sendStatus.peripheral); // blink send status LED

	if (ReadTelemetry(msgBuffer, JSON_MESSAGE_BYTES) > 0) {
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
	struct sigaction action;
	memset(&action, 0, sizeof(struct sigaction));
	action.sa_handler = TerminationHandler;
	sigaction(SIGTERM, &action, NULL);

	epollFd = CreateEpollFd();
	if (epollFd < 0) {
		return -1;
	}

	OPEN_PERIPHERAL_SET(actuatorDevices);
	OPEN_PERIPHERAL_SET(deviceTwinDevices);
	OPEN_PERIPHERAL_SET(directMethodDevices);

	InitDeviceTwins(deviceTwinDevices, NELEMS(deviceTwinDevices));
	InitDirectMethods(directMethodDevices, NELEMS(directMethodDevices));

	// Initialize Grove Shield and Grove Temperature and Humidity Sensor
	GroveShield_Initialize(&i2cFd, 115200);
	sht31 = GroveTempHumiSHT31_Open(i2cFd);

	START_TIMER_SET(timers);

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

	CloseFdAndPrintError(epollFd, "Epoll");
}


static int OpenPeripheral(Peripheral* peripheral) {
	peripheral->fd = GPIO_OpenAsOutput(peripheral->pin, GPIO_OutputMode_PushPull, peripheral->initialState);
	if (peripheral->fd < 0) {
		Log_Debug(
			"Error opening GPIO: %s (%d). Check that app_manifest.json includes the GPIO used.\n",
			strerror(errno), errno);
		return -1;
	}
	return 0;
}


static int StartTimer(Timer* timer) {
	timer->fd = CreateTimerFdAndAddToEpoll(epollFd, &timer->period, &timer->eventData, EPOLLIN);
	if (timer->fd < 0) {
		return -1;
	}
	return 0;
}


static int InitFanPWM(struct _peripheral* peripheral) {
	// for you to implement:)
	return 0;
}


static void TerminationHandler(int signalNumber)
{
	// Don't use Log_Debug here, as it is not guaranteed to be async-signal-safe.
	terminationRequired = true;
}


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


static bool SetFanSpeedDirectMethod(JSON_Object* json, DirectMethodPeripheral* peripheral) {
	// for you to fully implement
	int speed = (int)json_object_get_number(json, "speed");
	Log_Debug("Set fan speed %d", speed);
	return true;
}