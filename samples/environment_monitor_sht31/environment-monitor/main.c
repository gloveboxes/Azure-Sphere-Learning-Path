/*
 *   Please read the disclaimer
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
 */

#include "hw/azure_sphere_learning_path.h"

#include "azure_iot.h"
#include "exit_codes.h"
#include "config.h"
#include "inter_core.h"
#include "peripheral_gpio.h"
#include "terminate.h"
#include "timer.h"

#include "applibs_versions.h"
#include <applibs/gpio.h>
#include <applibs/log.h>
#include <applibs/powermanagement.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

#include "./embedded/sht31/sht3x.h"

#define JSON_MESSAGE_BYTES 256 // Number of bytes to allocate for the JSON telemetry message for IoT Central

 // Forward signatures
static void MeasureSensorHandler(EventLoopTimer* eventLoopTimer);
static void NetworkConnectionStatusHandler(EventLoopTimer* eventLoopTimer);
static void TemperatureAlertHandler(EventLoopTimer* eventLoopTimer);
static void TemperatureAlertBuzzerOffOneShotTimer(EventLoopTimer* eventLoopTimer);
static void DeviceTwinGenericHandler(LP_DEVICE_TWIN_BINDING* deviceTwinBinding);

LP_USER_CONFIG lp_config;

static char msgBuffer[JSON_MESSAGE_BYTES] = { 0 };

enum HVAC { HEATING, COOLING, OFF };
static char* hvacState[] = { "Heating", "Cooling", "Off" };

static enum HVAC current_hvac_state = OFF;
static float temperature, humidity;
static int previous_temperature = 0;
static const struct timespec co2AlertBuzzerPeriod = { 0, 5 * 100 * 1000 };

// GPIO Output PeripheralGpios
#ifdef OEM_SEEED_STUDIO
static LP_GPIO hvacHeatingLed = { .pin = LED_RED, .direction = LP_OUTPUT, .initialState = GPIO_Value_Low,  .name = "hvacHeatingLed" };
static LP_GPIO hvacCoolingLed = { .pin = LED_BLUE, .direction = LP_OUTPUT, .initialState = GPIO_Value_Low,  .name = "hvacCoolingLed" };
static LP_GPIO co2AlertPin = { .pin = CO2_ALERT, .direction = LP_OUTPUT, .initialState = GPIO_Value_Low,  .name = "co2AlertPin" };
#endif // OEM_AVNET

#ifdef OEM_AVNET
static LP_GPIO hvacHeatingLed = { .pin = LED_RED, .direction = LP_OUTPUT, .initialState = GPIO_Value_Low, .invertPin = true,  .name = "hvacHeatingLed" };
static LP_GPIO hvacCoolingLed = { .pin = LED_BLUE, .direction = LP_OUTPUT, .initialState = GPIO_Value_Low, .invertPin = true,  .name = "hvacCoolingLed" };
static LP_GPIO co2AlertPin = { .pin = CO2_ALERT, .direction = LP_OUTPUT, .initialState = GPIO_Value_Low, .invertPin = true,  .name = "co2AlertPin" };
#endif // OEM_AVNET

static LP_GPIO azureIotConnectedLed = { .pin = NETWORK_CONNECTED_LED, .direction = LP_OUTPUT, .initialState = GPIO_Value_Low, .invertPin = true,  .name = "azureConnectedLed" };

// Timers
static LP_TIMER azureConnectedStatusTimer = { .period = {10, 0}, .name = "azureConnectedStatusTimer", .handler = NetworkConnectionStatusHandler };
static LP_TIMER measureSensorTimer = { .period = {4, 0}, .name = "measureSensorTimer", .handler = MeasureSensorHandler };
static LP_TIMER temperatureAlertTimer = { .period = {4, 0}, .name = "temperatureAlertTimer", .handler = TemperatureAlertHandler };
static LP_TIMER temperatureAlertBuzzerOffOneShotTimer = { .period = { 0, 0 }, .name = "temperatureAlertBuzzerOffOneShotTimer", .handler = TemperatureAlertBuzzerOffOneShotTimer };

// Azure IoT Device Twins
static LP_DEVICE_TWIN_BINDING desiredTemperature = { .twinProperty = "DesiredTemperature", .twinType = LP_TYPE_FLOAT, .handler = DeviceTwinGenericHandler };
static LP_DEVICE_TWIN_BINDING desiredTemperatureAlertLevel = { .twinProperty = "DesiredTemperatureAlertLevel", .twinType = LP_TYPE_FLOAT, .handler = DeviceTwinGenericHandler };
static LP_DEVICE_TWIN_BINDING actualTemperature = { .twinProperty = "ReportedTemperature", .twinType = LP_TYPE_FLOAT };
static LP_DEVICE_TWIN_BINDING actualHvacState = { .twinProperty = "ReportedHvacState", .twinType = LP_TYPE_STRING };

// Initialize Sets
LP_GPIO* PeripheralGpioSet[] = { &hvacHeatingLed, &hvacCoolingLed, &co2AlertPin, &azureIotConnectedLed };
LP_DEVICE_TWIN_BINDING* deviceTwinBindingSet[] = { &desiredTemperature, &actualTemperature, &desiredTemperatureAlertLevel, &actualHvacState };
LP_TIMER* timerSet[] = { &azureConnectedStatusTimer, &measureSensorTimer, &temperatureAlertTimer, &temperatureAlertBuzzerOffOneShotTimer };

static const char* MsgTemplate = "{ \"Temperature\": %3.2f, \"Humidity\": \"%3.1f\", \"MsgId\":%d }";
static LP_MESSAGE_PROPERTY* telemetryMessageProperties[] = {
	&(LP_MESSAGE_PROPERTY) { .key = "appid", .value = "hvac" },
	&(LP_MESSAGE_PROPERTY) {.key = "format", .value = "json" },
	&(LP_MESSAGE_PROPERTY) {.key = "type", .value = "telemetry" },
	&(LP_MESSAGE_PROPERTY) {.key = "version", .value = "1" }
};

/// <summary>
/// Check status of connection to Azure IoT
/// </summary>
static void NetworkConnectionStatusHandler(EventLoopTimer* eventLoopTimer)
{
	static bool toggleConnectionStatusLed = true;

	if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0)
	{
		lp_terminate(ExitCode_ConsumeEventLoopTimeEvent);
	}
	else
	{
		if (lp_azureConnect())
		{
			lp_gpioStateSet(&azureIotConnectedLed, toggleConnectionStatusLed);
			toggleConnectionStatusLed = !toggleConnectionStatusLed;
		}
		else
		{
			lp_gpioStateSet(&azureIotConnectedLed, false);
		}
	}
}

/// <summary>
/// Set the HVAC status led. 
/// Red if heater needs to be turned on to get to desired temperature. 
/// Blue to turn on cooler. 
/// Green equals just right, no action required.
/// </summary>
static void SetHvacStatusColour(int temperature)
{
	if (!desiredTemperature.twinStateUpdated) { return; }

	static enum HVAC previous_hvac_state = OFF;
	int actual = (int)temperature;
	int desired = (int)*(float*)desiredTemperature.twinState;

	current_hvac_state = actual == desired ? OFF : actual > desired ? COOLING : HEATING;

	if (previous_hvac_state != current_hvac_state)
	{
		previous_hvac_state = current_hvac_state;
		lp_deviceTwinReportState(&actualHvacState, hvacState[current_hvac_state]);
	}

	lp_gpioOff(&hvacHeatingLed);
	lp_gpioOff(&hvacCoolingLed);

	switch (current_hvac_state)
	{
	case HEATING:
		lp_gpioOn(&hvacHeatingLed);
		break;
	case COOLING:
		lp_gpioOn(&hvacCoolingLed);
		break;
	default:
		break;
	}
}

/// <summary>
/// Turn off CO2 Buzzer
/// </summary>
static void TemperatureAlertBuzzerOffOneShotTimer(EventLoopTimer* eventLoopTimer)
{
	if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0)
	{
		lp_terminate(ExitCode_ConsumeEventLoopTimeEvent);
		return;
	}
	lp_gpioOff(&co2AlertPin);
}


/// <summary>
/// Turn on CO2 Buzzer if CO2 ppm greater than desiredTemperatureAlertLevel device twin
/// </summary>
static void TemperatureAlertHandler(EventLoopTimer* eventLoopTimer)
{
	if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0)
	{
		lp_terminate(ExitCode_ConsumeEventLoopTimeEvent);
		return;
	}

	if (desiredTemperatureAlertLevel.twinStateUpdated && temperature > *(float*)desiredTemperatureAlertLevel.twinState)
	{
		lp_gpioOn(&co2AlertPin);
		lp_timerOneShotSet(&temperatureAlertBuzzerOffOneShotTimer, &co2AlertBuzzerPeriod);
	}
}


/// <summary>
/// Read sensor and send to Azure IoT
/// </summary>
static void MeasureSensorHandler(EventLoopTimer* eventLoopTimer)
{
	static int msgId = 0;
	int32_t int32_temperature, int32_humidity;

	if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0)
	{
		lp_terminate(ExitCode_ConsumeEventLoopTimeEvent);
	}
	else {
		/* Measure temperature and relative humidity and store into variables
		 * temperature, humidity (each output multiplied by 1000).
		 */
		int16_t ret = sht3x_measure_blocking_read(&int32_temperature, &int32_humidity);

		temperature = int32_temperature / 1000.0f;
		humidity = int32_humidity / 1000.0f;

		if (ret == STATUS_OK && snprintf(msgBuffer, JSON_MESSAGE_BYTES, MsgTemplate, temperature, humidity, ++msgId) > 0)
		{
			Log_Debug("%s\n", msgBuffer);
			lp_azureMsgSendWithProperties(msgBuffer, telemetryMessageProperties, NELEMS(telemetryMessageProperties));
		}

		SetHvacStatusColour((int)temperature);

		// If the previous temperature not equal to the new temperature then update ReportedTemperature device twin
		if (previous_temperature != (int)temperature) {
			lp_deviceTwinReportState(&actualTemperature, &temperature);
		}
		previous_temperature = (int)temperature;
	}
}

/// <summary>
/// Generic Device Twin Handler. It just sets reported state for the twin
/// </summary>
static void DeviceTwinGenericHandler(LP_DEVICE_TWIN_BINDING* deviceTwinBinding)
{
	lp_deviceTwinReportState(deviceTwinBinding, deviceTwinBinding->twinState);
	lp_deviceTwinAckDesiredState(deviceTwinBinding, deviceTwinBinding->twinState, LP_DEVICE_TWIN_COMPLETED);

	SetHvacStatusColour(previous_temperature);
}

static bool InitializeSht31(void)
{
	uint16_t interval_in_seconds = 2;
	int retry = 0;

	sensirion_i2c_init();

	while (sht3x_probe() != STATUS_OK && ++retry < 5) {
		Log_Debug("SHT sensor probing failed\n");
		sensirion_sleep_usec(1000000u);
	}

	if (retry < 5) {
		Log_Debug("SHT sensor probing successful\n");
	}
	else {
		Log_Debug("SHT sensor probing failed\n");
	}

	sensirion_sleep_usec(interval_in_seconds * 1000000u);	// sleep for good luck

	return true;
}

/// <summary>
///  Initialize PeripheralGpios, device twins, direct methods, timers.
/// </summary>
/// <returns>0 on success, or -1 on failure</returns>
static void InitPeripheralGpiosAndHandlers(void)
{
	InitializeSht31();

	lp_azureInitialize(lp_config.scopeId, lp_config.deviceTwinModelId);

	lp_gpioSetOpen(PeripheralGpioSet, NELEMS(PeripheralGpioSet));
	lp_deviceTwinSetOpen(deviceTwinBindingSet, NELEMS(deviceTwinBindingSet));

	lp_timerSetStart(timerSet, NELEMS(timerSet));

	lp_azureToDeviceStart();
}

/// <summary>
///     Close PeripheralGpios and handlers.
/// </summary>
static void ClosePeripheralGpiosAndHandlers(void)
{
	Log_Debug("Closing file descriptors\n");

	lp_timerSetStop(timerSet, NELEMS(timerSet));

	lp_azureToDeviceStop();

	lp_gpioSetClose(PeripheralGpioSet, NELEMS(PeripheralGpioSet));
	lp_deviceTwinSetClose();

	lp_timerEventLoopStop();
}

int main(int argc, char* argv[])
{
	lp_registerTerminationHandler();

	lp_configParseCmdLineArguments(argc, argv, &lp_config);
	if (!lp_configValidate(&lp_config))
	{
		return lp_getTerminationExitCode();
	}

	InitPeripheralGpiosAndHandlers();

	// Main loop
	while (!lp_isTerminationRequired())
	{
		int result = EventLoop_Run(lp_timerGetEventLoop(), -1, true);
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