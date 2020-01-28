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
#define JSON_MESSAGE_BYTES 100  // Number of bytes to allocate for the JSON telemetry message for IoT Central

static char msgBuffer[JSON_MESSAGE_BYTES] = { 0 };

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


static ActuatorPeripheral sendStatus = {
	.peripheral = {
		.fd = -1, 
		.pin = SEND_STATUS_PIN, 
		.initialState = GPIO_Value_High, 
		.invertPin = true, 
		.initialise = OpenPeripheral, 
		.name = "SendStatus" }
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
static int ReadTelemetry(char msgBuffer[], size_t len) {
	static int msgId = 0;
	GroveTempHumiSHT31_Read(sht31);
	float temperature = GroveTempHumiSHT31_GetTemperature(sht31);
	float humidity = GroveTempHumiSHT31_GetHumidity(sht31);

	static const char* MsgTemplate = "{ \"Temperature\": \"%3.2f\", \"Humidity\": \"%3.1f\", \"MsgId\":%d }";
	return snprintf(msgBuffer, len, MsgTemplate, temperature, humidity, msgId++);
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


static void TerminationHandler(int signalNumber)
{
	// Don't use Log_Debug here, as it is not guaranteed to be async-signal-safe.
	terminationRequired = true;
}

