#include "../MT3620_Grove_Shield/MT3620_Grove_Shield_Library/Grove.h"
#include "../MT3620_Grove_Shield/MT3620_Grove_Shield_Library/Sensors/GroveTempHumiSHT31.h"
#include "globals.h"
#include "peripheral.h"
#include "utilities.h"
#include <applibs/gpio.h>
#include <applibs/log.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


// Select Azure Sphere Dev Kit
#define AVNET_DK 1
//#define SEEED_DK 1
//#define SEEED_MINI_DK 1

#if defined AVNET_DK

#define BUILTIN_LED 4

#elif defined SEEED_DK

// GPIO Pins used in the High Level (HL) Application
#define BUILTIN_LED 19

#endif

#define JSON_MESSAGE_BYTES 100  // Number of bytes to allocate for the JSON telemetry message for IoT Central
static char msgBuffer[JSON_MESSAGE_BYTES] = { 0 };

static int i2cFd;
static void* sht31;

bool ledState = false;

// Forward signatures
static int InitPeripheralsAndHandlers(void);
static void ClosePeripheralsAndHandlers(void);
static void MeasureSensorHandler(EventData* eventData);


static ActuatorPeripheral statusLed = {			// Describes a simple one pin GPIO Peripheral
	.peripheral = {
		.fd = -1,								// The File Descriptor for the GPIO pin
		.pin = BUILTIN_LED,					// The GPIO pin this peripheral will be using. Also defined in app_capabilities.json
		.initialState = GPIO_Value_High,		// Set the initial state of the GPIO when opened
		.invertPin = true,						// Cater for some pins where GPIO Low sets a pin state to 3.3V, and GPIO High to ground
		.initialise = OpenPeripheral,			// The name of the C function to be called to open/initialize the peripheral
		.name = "StatusLED" }					// Name used when closing this peripheral
};


static Timer readSensor = {						//Describes a general timer
	.eventData = {
		.eventHandler = &MeasureSensorHandler },// The name of the C function to call when the timer fires
	.period = { 1, 0 },							// How often should the timer fire (1 second, 0 nanoseconds)
	.name = "ReadSensor"						// Name used when closing this timer
};


#pragma region define sets for auto initialisation/open and close

ActuatorPeripheral* actuatorDevices[] = { &statusLed };		// set/collection of actuatorDevices
Timer* timers[] = { &readSensor };							// set/collection of timers

#pragma endregion


int main(int argc, char* argv[])
{
	RegisterTerminationHandler();
	ProcessCmdArgs(argc, argv);
	srand((unsigned int)time(NULL)); // seed the random number generator for fake telemetry

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
	if (ConsumeTimerFdEvent(readSensor.fd) != 0) {
		terminationRequired = true;
		return;
	}

	ledState = !ledState;  // toggle LED state

	if (ledState) { GPIO_ON(statusLed.peripheral); }
	else { GPIO_OFF(statusLed.peripheral); }

	ReadTelemetry();
	Log_Debug(msgBuffer);
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

	CloseFdAndPrintError(GetEpollFd(), "Epoll");
}
