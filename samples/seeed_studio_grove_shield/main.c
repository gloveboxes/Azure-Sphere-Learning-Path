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

// Hardware definition
#include "hw/azure_sphere_learning_path.h"

// Learning Path Libraries
#include "azure_iot.h"
#include "exit_codes.h"
#include "globals.h"
#include "peripheral_gpio.h"
#include "terminate.h"
#include "timer.h"

// System Libraries
#include "applibs_versions.h"
#include <applibs/gpio.h>
#include <applibs/log.h>
#include <applibs/powermanagement.h>
#include <applibs/uart.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

// Grove Temperature and Humidity Sensor
#include "Grove.h"
#include "Sensors/GroveTempHumiSHT31.h"

// Required for Grove Sensors
static int i2cFd;
static void *sht31;

#define JSON_MESSAGE_BYTES 256 // Number of bytes to allocate for the JSON telemetry message for IoT Central

// Forward signatures
static void LedOffHandler(EventLoopTimer *eventLoopTimer);
static void MeasureSensorHandler(EventLoopTimer *eventLoopTimer);
static void NetworkConnectionStatusHandler(EventLoopTimer *eventLoopTimer);
static void DeviceTwinSetTemperatureHandler(LP_DEVICE_TWIN_BINDING *deviceTwinBinding);

static char msgBuffer[JSON_MESSAGE_BYTES] = {0};

static const struct timespec sendMsgLedBlinkPeriod = {0, 300 * 1000 * 1000};

enum LEDS
{
	RED,
	GREEN,
	BLUE
};
static enum LEDS current_led = RED;
static const char *hvacState[] = {"heating", "off", "cooling"};

static int desired_temperature = 0;
static float last_temperature = 0;

// GPIO Output PeripheralGpios
static LP_PERIPHERAL_GPIO sendMsgLed = {
	.pin = LED2,
	.direction = LP_OUTPUT,
	.initialState = GPIO_Value_Low,
	.invertPin = true,
	.initialise = lp_openPeripheralGpio,
	.name = "sendMsgLed"};

static LP_PERIPHERAL_GPIO networkConnectedLed = {
	.pin = NETWORK_CONNECTED_LED,
	.direction = LP_OUTPUT,
	.initialState = GPIO_Value_Low,
	.invertPin = true,
	.initialise = lp_openPeripheralGpio,
	.name = "networkConnectedLed"};

static LP_PERIPHERAL_GPIO ledRed = {.pin = LED_RED, .direction = LP_OUTPUT, .initialState = GPIO_Value_Low, .invertPin = true, .initialise = lp_openPeripheralGpio, .name = "red led"};
static LP_PERIPHERAL_GPIO ledGreen = {.pin = LED_GREEN, .direction = LP_OUTPUT, .initialState = GPIO_Value_Low, .invertPin = true, .initialise = lp_openPeripheralGpio, .name = "green led"};
static LP_PERIPHERAL_GPIO ledBlue = {.pin = LED_BLUE, .direction = LP_OUTPUT, .initialState = GPIO_Value_Low, .invertPin = true, .initialise = lp_openPeripheralGpio, .name = "blue led"};
static LP_PERIPHERAL_GPIO *rgbLed[] = {&ledRed, &ledGreen, &ledBlue};

// Timers
static LP_TIMER sendMsgLedOffOneShotTimer = {.period = {0, 0}, .name = "sendMsgLedOffOneShotTimer", .handler = LedOffHandler};
static LP_TIMER networkConnectionStatusTimer = {.period = {5, 0}, .name = "networkConnectionStatusTimer", .handler = NetworkConnectionStatusHandler};
static LP_TIMER measureSensorTimer = {.period = {10, 0}, .name = "measureSensorTimer", .handler = MeasureSensorHandler};

// Azure IoT Device Twins
static LP_DEVICE_TWIN_BINDING desiredTemperature = {.twinProperty = "DesiredTemperature", .twinType = LP_TYPE_FLOAT, .handler = DeviceTwinSetTemperatureHandler};
static LP_DEVICE_TWIN_BINDING actualTemperature = {.twinProperty = "ActualTemperature", .twinType = LP_TYPE_FLOAT};
static LP_DEVICE_TWIN_BINDING actualHvacState = {.twinProperty = "ActualTemperature", .twinType = LP_TYPE_STRING};

// Initialize Sets
LP_PERIPHERAL_GPIO *peripheralGpioSet[] = {
	&sendMsgLed,
	&networkConnectedLed,
	&ledRed,
	&ledGreen,
	&ledBlue,
};
LP_TIMER *timerSet[] = {&sendMsgLedOffOneShotTimer, &networkConnectionStatusTimer, &measureSensorTimer};
LP_DEVICE_TWIN_BINDING *deviceTwinBindingSet[] = {&desiredTemperature, &actualTemperature, &actualHvacState};

// Message templates and property sets

// Common message properties
static LP_MESSAGE_PROPERTY messageAppId = {.key = "appid", .value = "hvac"};
static LP_MESSAGE_PROPERTY messageFormat = {.key = "format", .value = "json"};

// Telemetry message template and properties
static const char *MsgTemplate = "{ \"Temperature\": \"%3.2f\", \"Humidity\": \"%3.1f\", \"Pressure\":\"%3.1f\", \"Light\":%d, \"MsgId\":%d }";
static LP_MESSAGE_PROPERTY telemetryMessageType = {.key = "type", .value = "telemetry"};
static LP_MESSAGE_PROPERTY messageVersion = {.key = "version", .value = "1"};

// Message property sets
static LP_MESSAGE_PROPERTY *telemetryMessageProperties[] = {&messageAppId, &telemetryMessageType, &messageFormat, &messageVersion};

/// <summary>
/// Check status of connection to Azure IoT
/// </summary>
static void NetworkConnectionStatusHandler(EventLoopTimer *eventLoopTimer)
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
static void SendMsgLedOn(char *message)
{
	lp_gpioOn(&sendMsgLed);
	Log_Debug("%s\n", message);

	lp_sendMsg(message);

	lp_setOneShotTimer(&sendMsgLedOffOneShotTimer, &sendMsgLedBlinkPeriod);
}

/// <summary>
/// One shot timer to turn sendMsgLed off
/// </summary>
static void LedOffHandler(EventLoopTimer *eventLoopTimer)
{
	if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0)
	{
		lp_terminate(ExitCode_ConsumeEventLoopTimeEvent);
		return;
	}
	lp_gpioOff(&sendMsgLed);
}

/// <summary>
/// Set the temperature status led.
/// Red if HVAC needs to be turned on to get to desired temperature.
/// Blue to turn on cooler.
/// Green equals just right, no action required.
/// </summary>
void SetTemperatureStatusColour(float actual_temperature)
{
	static enum LEDS previous_led = RED;
	int actual = (int)(actual_temperature);

	current_led = actual == desired_temperature ? GREEN : actual > desired_temperature ? BLUE : RED;

	if (previous_led != current_led)
	{
		lp_gpioOff(rgbLed[(int)previous_led]); // turn off old current colour
		previous_led = current_led;

		lp_deviceTwinReportState(&actualHvacState, hvacState[(int)current_led]);
	}

	lp_gpioOn(rgbLed[(int)current_led]);
}

/// <summary>
/// Read sensor and send to Azure IoT
/// </summary>
static void MeasureSensorHandler(EventLoopTimer *eventLoopTimer)
{
	static int msgId = 0;
	int len = 0;

	if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0)
	{
		lp_terminate(ExitCode_ConsumeEventLoopTimeEvent);
		return;
	}

	GroveTempHumiSHT31_Read(sht31);
	float temperature = GroveTempHumiSHT31_GetTemperature(sht31);
	float humidity = GroveTempHumiSHT31_GetHumidity(sht31);
	if (isnan(temperature) || isnan(humidity))
	{
		return;
	}

	SetTemperatureStatusColour(temperature);

	if (snprintf(msgBuffer, JSON_MESSAGE_BYTES, MsgTemplate, temperature, humidity, 0.0, 0, msgId++) > 0)
	{
		// Message properties can be used for message routing in IOT Hub
		lp_setMessageProperties(telemetryMessageProperties, NELEMS(telemetryMessageProperties));

		SendMsgLedOn(msgBuffer);

		// optional: clear if you are sending other message types that don't require these properties
		lp_clearMessageProperties();
	}

	if ((int)last_temperature != (int)temperature){
		lp_deviceTwinReportState(&actualTemperature, &temperature);
	}

	last_temperature = temperature;
}

/// <summary>
/// Device Twin Handler to set the desired temperature value
/// </summary>
static void DeviceTwinSetTemperatureHandler(LP_DEVICE_TWIN_BINDING *deviceTwinBinding)
{
	if (deviceTwinBinding->twinType == LP_TYPE_FLOAT)
	{
		desired_temperature = (int)*(float *)deviceTwinBinding->twinState;
		SetTemperatureStatusColour(last_temperature);
		lp_deviceTwinReportState(deviceTwinBinding, deviceTwinBinding->twinState);
	}
}

/// <summary>
///  Initialize peripherals, device twins, direct methods, timers.
/// </summary>
/// <returns>0 on success, or -1 on failure</returns>
static void InitPeripheralsAndHandlers(void)
{
	// Initialize Grove Shield and Grove Temperature and Humidity Sensor
	GroveShield_Initialize(&i2cFd, 115200);
	sht31 = GroveTempHumiSHT31_Open(i2cFd);

	lp_openPeripheralGpioSet(peripheralGpioSet, NELEMS(peripheralGpioSet));
	lp_openDeviceTwinSet(deviceTwinBindingSet, NELEMS(deviceTwinBindingSet));

	lp_startTimerSet(timerSet, NELEMS(timerSet));
	lp_startCloudToDevice();
}

/// <summary>
///     Close peripherals and handlers.
/// </summary>
static void ClosePeripheralsAndHandlers(void)
{
	Log_Debug("Closing file descriptors\n");

	lp_stopTimerSet();
	lp_stopCloudToDevice();

	lp_closePeripheralGpioSet();
	lp_closeDeviceTwinSet();

#ifndef SEEED_GROVE_SHIELD
	lp_closeDevKit();
#endif

	lp_stopTimerEventLoop();
}

int main(int argc, char *argv[])
{
	lp_registerTerminationHandler();
	lp_processCmdArgs(argc, argv);

	if (strlen(scopeId) == 0)
	{
		Log_Debug("ScopeId needs to be set in the app_manifest CmdArgs\n");
		return ExitCode_Missing_ID_Scope;
	}

	InitPeripheralsAndHandlers();

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

	ClosePeripheralsAndHandlers();

	Log_Debug("Application exiting.\n");
	return lp_getTerminationExitCode();
}