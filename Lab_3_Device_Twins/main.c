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
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

// Hardware specific
#ifdef OEM_AVNET
#include "AVNET/board.h"
#include "AVNET/imu_temp_pressure.h"
#include "AVNET/light_sensor.h"
#endif // OEM_AVNET

// Hardware specific
#ifdef OEM_SEEED_STUDIO
#include "SEEED_STUDIO/board.h"
#endif // SEEED_STUDIO

#define JSON_MESSAGE_BYTES 256 // Number of bytes to allocate for the JSON telemetry message for IoT Central

// Forward signatures
static void InitPeripheralGpiosAndHandlers(void);
static void ClosePeripheralGpiosAndHandlers(void);
static void TemperatureStatusBlinkHandler(EventLoopTimer* eventLoopTimer);
static void SendMsgLedOffHandler(EventLoopTimer* eventLoopTimer);
static void MeasureSensorHandler(EventLoopTimer* eventLoopTimer);
static void ButtonPressCheckHandler(EventLoopTimer* eventLoopTimer);
static void NetworkConnectionStatusHandler(EventLoopTimer* eventLoopTimer);
static void DeviceTwinSetTemperatureHandler(LP_DEVICE_TWIN_BINDING* deviceTwinBinding);

static char msgBuffer[JSON_MESSAGE_BYTES] = { 0 };

static const struct timespec sendMsgLedBlinkPeriod = { 0, 300 * 1000 * 1000 };

static int led1BlinkIntervalIndex = 2;
static const struct timespec led1BlinkIntervals[] = { {0, 125000000}, {0, 250000000}, {0, 500000000}, {0, 750000000}, {1, 0} };
static const int led1BlinkIntervalsCount = NELEMS(led1BlinkIntervals);

enum LEDS
{
	RED,
	GREEN,
	BLUE
};

static enum LEDS current_led = RED;

static float desired_temperature = 0;
static float last_temperature = 0;

// GPIO Input PeripheralGpios
static LP_PERIPHERAL_GPIO buttonA = { .pin = BUTTON_A, .direction = LP_INPUT, .initialise = lp_openPeripheralGpio, .name = "buttonA" };
static LP_PERIPHERAL_GPIO buttonB = { .pin = BUTTON_B, .direction = LP_INPUT, .initialise = lp_openPeripheralGpio, .name = "buttonB" };

// GPIO Output PeripheralGpios
static LP_PERIPHERAL_GPIO ledRed = { .pin = LED_RED, .direction = LP_OUTPUT, .initialState = GPIO_Value_Low, .invertPin = true, .initialise = lp_openPeripheralGpio, .name = "red led" };
static LP_PERIPHERAL_GPIO ledGreen = { .pin = LED_GREEN, .direction = LP_OUTPUT, .initialState = GPIO_Value_Low, .invertPin = true, .initialise = lp_openPeripheralGpio, .name = "green led" };
static LP_PERIPHERAL_GPIO ledBlue = { .pin = LED_BLUE, .direction = LP_OUTPUT, .initialState = GPIO_Value_Low, .invertPin = true, .initialise = lp_openPeripheralGpio, .name = "blue led" };
static LP_PERIPHERAL_GPIO* rgbLed[] = { &ledRed, &ledGreen, &ledBlue };

static LP_PERIPHERAL_GPIO sendMsgLed = { .pin = LED2, .direction = LP_OUTPUT, .initialState = GPIO_Value_Low, .invertPin = true, .initialise = lp_openPeripheralGpio, .name = "sendMsgLed" };
static LP_PERIPHERAL_GPIO networkConnectedLed = { .pin = NETWORK_CONNECTED_LED, .direction = LP_OUTPUT, .initialState = GPIO_Value_Low, .invertPin = true, .initialise = lp_openPeripheralGpio, .name = "networkConnectedLed" };

// Timers
static LP_TIMER temperatureStatusBlinkTimer = { .period = {0, 500000000}, .name = "temperatureStatusBlinkTimer", .handler = TemperatureStatusBlinkHandler };
static LP_TIMER sendMsgLedOffOneShotTimer = { .period = {0, 0}, .name = "sendMsgLedOffOneShotTimer", .handler = SendMsgLedOffHandler };
static LP_TIMER networkConnectionStatusTimer = { .period = {5, 0}, .name = "networkConnectionStatusTimer", .handler = NetworkConnectionStatusHandler };
static LP_TIMER measureSensorTimer = { .period = {10, 0}, .name = "measureSensorTimer", .handler = MeasureSensorHandler };
static LP_TIMER buttonPressCheckTimer = { .period = { 0, 1000000 }, .name = "buttonPressCheckTimer", .handler = ButtonPressCheckHandler };

// Azure IoT Device Twins
static LP_DEVICE_TWIN_BINDING desiredTemperature = { .twinProperty = "DesiredTemperature", .twinType = LP_TYPE_FLOAT, .handler = DeviceTwinSetTemperatureHandler };
static LP_DEVICE_TWIN_BINDING actualTemperature = { .twinProperty = "ActualTemperature", .twinType = LP_TYPE_FLOAT };

// Initialize Sets
LP_PERIPHERAL_GPIO* PeripheralGpioSet[] = { &buttonA, &buttonB, &ledRed, &ledGreen, &ledBlue, &sendMsgLed, &networkConnectedLed };
LP_TIMER* timerSet[] = { &temperatureStatusBlinkTimer, &sendMsgLedOffOneShotTimer, &networkConnectionStatusTimer, &measureSensorTimer, &buttonPressCheckTimer };
LP_DEVICE_TWIN_BINDING* deviceTwinBindingSet[] = { &desiredTemperature, &actualTemperature };

// Message property set
static LP_MESSAGE_PROPERTY messageAppId = { .key = "appid", .value = "hvac" };
static LP_MESSAGE_PROPERTY messageType = { .key = "type", .value = "telemetry" };
static LP_MESSAGE_PROPERTY messageFormat = { .key = "format", .value = "json" };
static LP_MESSAGE_PROPERTY messageVersion = { .key = "version", .value = "1" };
static LP_MESSAGE_PROPERTY* telemetryMessageProperties[] = { &messageAppId, &messageType, &messageFormat, &messageVersion };


int main(int argc, char* argv[])
{
	lp_registerTerminationHandler();
	lp_processCmdArgs(argc, argv);

	if (strlen(scopeId) == 0)
	{
		Log_Debug("ScopeId needs to be set in the app_manifest CmdArgs\n");
		return ExitCode_Missing_ID_Scope;
	}

	InitPeripheralGpiosAndHandlers();

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

	ClosePeripheralGpiosAndHandlers();

	Log_Debug("Application exiting.\n");
	return lp_getTerminationExitCode();
}

/// <summary>
/// Check status of connection to Azure IoT
/// </summary>
static void NetworkConnectionStatusHandler(EventLoopTimer* eventLoopTimer)
{
	if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0)
	{
		lp_terminate(ExitCode_ConsumeEventLoopTimeEvent);
		return;
	}

	if (lp_connectToAzureIot()) { lp_gpioOn(&networkConnectedLed); }
	else { lp_gpioOff(&networkConnectedLed); }
}

/// <summary>
/// Turn on Send Msg LED, send message to Azure IoT and set a one shot timer to turn Send Msg LED off
/// </summary>
static void SendMsgLedOn(char* message)
{
	lp_gpioOn(&sendMsgLed);
	Log_Debug("%s\n", message);

	// optional: message properties can be used for message routing in IOT Hub
	lp_setMessageProperties(telemetryMessageProperties, NELEMS(telemetryMessageProperties));

	lp_sendMsg(message);

	// optional: clear if you are sending other message types that don't require these properties
	lp_clearMessageProperties();

	lp_setOneShotTimer(&sendMsgLedOffOneShotTimer, &sendMsgLedBlinkPeriod);
}

/// <summary>
/// One shot timer to turn LED2 off
/// </summary>
static void SendMsgLedOffHandler(EventLoopTimer* eventLoopTimer)
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
/// Red if heater needs to be turned on to get to desired temperature. 
/// Blue to turn on cooler. 
/// Green equals just right, no action required.
/// </summary>
void SetTemperatureStatusColour(float actual_temperature)
{
	int actual = (int)(actual_temperature);
	int desired = (int)(desired_temperature);

	current_led = actual == desired ? GREEN : actual > desired ? BLUE : RED;
}

/// <summary>
/// Read sensor and send to Azure IoT
/// </summary>
static void MeasureSensorHandler(EventLoopTimer* eventLoopTimer)
{
	static int msgId = 0;
	static LP_ENVIRONMENT environment;
	static const char* MsgTemplate = "{ \"Temperature\": \"%3.2f\", \"Humidity\": \"%3.1f\", \"Pressure\":\"%3.1f\", \"Light\":%d, \"MsgId\":%d }";

	if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0)
	{
		lp_terminate(ExitCode_ConsumeEventLoopTimeEvent);
		return;
	}

	if (lp_readTelemetry(&environment))
	{
		SetTemperatureStatusColour(environment.temperature);
		last_temperature = environment.temperature;

		if (snprintf(msgBuffer, JSON_MESSAGE_BYTES, MsgTemplate, environment.temperature, environment.humidity, environment.pressure, environment.light, msgId++) > 0)
		{
			SendMsgLedOn(msgBuffer);
		}
	}
}

/// <summary>
/// Blink Temperature Status Blink Handler
/// </summary>
static void TemperatureStatusBlinkHandler(EventLoopTimer* eventLoopTimer)
{
	static enum LEDS previous_led = RED;
	static bool led_state = false;

	if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0)
	{
		lp_terminate(ExitCode_ConsumeEventLoopTimeEvent);
		return;
	}

	if (previous_led != current_led)
	{
		lp_gpioOff(rgbLed[(int)previous_led]); // turn off old current colour
		previous_led = current_led;
	}

	led_state = !led_state;

	if (led_state) { lp_gpioOff(rgbLed[(int)current_led]); }
	else { lp_gpioOn(rgbLed[(int)current_led]); }	
}

/// <summary>
/// Device Twin Handler to set the desired temperature value
/// </summary>
static void DeviceTwinSetTemperatureHandler(LP_DEVICE_TWIN_BINDING* deviceTwinBinding)
{
	if (deviceTwinBinding->twinType == LP_TYPE_FLOAT)
	{
		desired_temperature = *(float*)deviceTwinBinding->twinState;
		SetTemperatureStatusColour(last_temperature);
		lp_deviceTwinReportState(deviceTwinBinding, deviceTwinBinding->twinState);
	}	

	/*	Casting device twin state examples

		float value = *(float*)deviceTwinBinding->twinState;
		int value = *(int*)deviceTwinBinding->twinState;
		bool value = *(bool*)deviceTwinBinding->twinState;
		char* value = (char*)deviceTwinBinding->twinState;
	*/
}

/// <summary>
/// Handler to check for Button Presses.
/// </summary>
static void ButtonPressCheckHandler(EventLoopTimer* eventLoopTimer)
{
	static GPIO_Value_Type buttonAState;
	static GPIO_Value_Type buttonBState;

	if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0)
	{
		lp_terminate(ExitCode_ConsumeEventLoopTimeEvent);
		return;
	}

	if (lp_gpioGetState(&buttonA, &buttonAState) || lp_gpioGetState(&buttonB, &buttonBState))
	{
		led1BlinkIntervalIndex = (led1BlinkIntervalIndex + 1) % led1BlinkIntervalsCount;
		lp_changeTimer(&temperatureStatusBlinkTimer, &led1BlinkIntervals[led1BlinkIntervalIndex]);

		lp_deviceTwinReportState(&actualTemperature, &last_temperature);	// TwinType = LP_TYPE_FLOAT
	}
}

/// <summary>
///  Initialize PeripheralGpios, device twins, direct methods, timers.
/// </summary>
/// <returns>0 on success, or -1 on failure</returns>
static void InitPeripheralGpiosAndHandlers(void)
{
	lp_initializeDevKit();

	lp_openPeripheralGpioSet(PeripheralGpioSet, NELEMS(PeripheralGpioSet));
	lp_openDeviceTwinSet(deviceTwinBindingSet, NELEMS(deviceTwinBindingSet));

	lp_startTimerSet(timerSet, NELEMS(timerSet));
	lp_startCloudToDevice();
}

/// <summary>
///     Close PeripheralGpios and handlers.
/// </summary>
static void ClosePeripheralGpiosAndHandlers(void)
{
	Log_Debug("Closing file descriptors\n");

	lp_stopTimerSet();
	lp_stopCloudToDevice();

	lp_closePeripheralGpioSet();
	lp_closeDeviceTwinSet();

	lp_closeDevKit();

	lp_stopTimerEventLoop();
}