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

#include "./embedded-scd/scd30/scd30.h"


#define JSON_MESSAGE_BYTES 256 // Number of bytes to allocate for the JSON telemetry message for IoT Central

 // Forward signatures
static void MeasureSensorHandler(EventLoopTimer* eventLoopTimer);
static void PublishTelemetryTimer(EventLoopTimer* eventLoopTimer);
static void NetworkConnectionStatusHandler(EventLoopTimer* eventLoopTimer);
static void CO2AlertHandler(EventLoopTimer* eventLoopTimer);
static void CO2AlertBuzzerOffOneShotTimer(EventLoopTimer* eventLoopTimer);
static void DeviceTwinGenericHandler(LP_DEVICE_TWIN_BINDING* deviceTwinBinding);

LP_USER_CONFIG lp_config;

static char msgBuffer[JSON_MESSAGE_BYTES] = { 0 };

static float co2_ppm, temperature, relative_humidity = NAN;
static const struct timespec co2AlertBuzzerPeriod = { 0, 5 * 100 * 1000 };

// GPIO Output PeripheralGpios
#ifdef OEM_SEEED_STUDIO
static LP_GPIO co2AlertPin = { .pin = CO2_ALERT, .direction = LP_OUTPUT, .initialState = GPIO_Value_Low, .name = "co2AlertPin" };
#endif // OEM_AVNET

#ifdef OEM_AVNET
static LP_PERIPHERAL_GPIO hvacHeatingLed = { .pin = LED_RED, .direction = LP_OUTPUT, .initialState = GPIO_Value_Low, .invertPin = true, .initialise = lp_openPeripheralGpio, .name = "hvacHeatingLed" };
static LP_PERIPHERAL_GPIO hvacCoolingLed = { .pin = LED_BLUE, .direction = LP_OUTPUT, .initialState = GPIO_Value_Low, .invertPin = true, .initialise = lp_openPeripheralGpio, .name = "hvacCoolingLed" };
static LP_PERIPHERAL_GPIO co2AlertPin = { .pin = CO2_ALERT, .direction = LP_OUTPUT, .initialState = GPIO_Value_Low, .invertPin = true, .initialise = lp_openPeripheralGpio, .name = "co2AlertPin" };
#endif // OEM_AVNET

static LP_GPIO azureIotConnectedLed = { .pin = NETWORK_CONNECTED_LED, .direction = LP_OUTPUT, .initialState = GPIO_Value_Low, .invertPin = true, .name = "azureConnectedLed" };

// Timers
static LP_TIMER azureConnectedStatusTimer = { .period = {10, 0}, .name = "azureConnectedStatusTimer", .handler = NetworkConnectionStatusHandler };
static LP_TIMER measureSensorTimer = { .period = {20, 0}, .name = "measureSensorTimer", .handler = MeasureSensorHandler };
static LP_TIMER publishTelemetryTimer = { .period = {30, 0}, .name = "publishTelemetryTimer", .handler = PublishTelemetryTimer };
static LP_TIMER co2AlertTimer = { .period = {4, 0}, .name = "co2AlertTimer", .handler = CO2AlertHandler };
static LP_TIMER co2AlertBuzzerOffOneShotTimer = { .period = { 0, 0 }, .name = "co2AlertBuzzerOffOneShotTimer", .handler = CO2AlertBuzzerOffOneShotTimer };

// Azure IoT Device Twins
static LP_DEVICE_TWIN_BINDING desiredCO2AlertLevel = { .twinProperty = "DesiredCO2AlertLevel", .twinType = LP_TYPE_FLOAT, .handler = DeviceTwinGenericHandler };
static LP_DEVICE_TWIN_BINDING actualCO2Level = { .twinProperty = "ActualCO2Level", .twinType = LP_TYPE_FLOAT };

// Initialize Sets
LP_GPIO* PeripheralGpioSet[] = { &co2AlertPin, &azureIotConnectedLed };
LP_DEVICE_TWIN_BINDING* deviceTwinBindingSet[] = { &desiredCO2AlertLevel, &actualCO2Level };
LP_TIMER* timerSet[] = {
		&azureConnectedStatusTimer, &measureSensorTimer, &publishTelemetryTimer,
		&co2AlertTimer, &co2AlertBuzzerOffOneShotTimer
};

static const char* MsgTemplate = "{ \"CO2\": %3.2f, \"Temperature\": %3.2f, \"Humidity\": \"%3.1f\", \"MsgId\":%d }";
static LP_MESSAGE_PROPERTY* telemetryMessageProperties[] = {
	&(LP_MESSAGE_PROPERTY) { .key = "appid", .value = "co2monitor" },
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
/// Turn off CO2 Buzzer
/// </summary>
static void CO2AlertBuzzerOffOneShotTimer(EventLoopTimer* eventLoopTimer)
{
	if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0)
	{
		lp_terminate(ExitCode_ConsumeEventLoopTimeEvent);
		return;
	}
	lp_gpioOff(&co2AlertPin);
}


/// <summary>
/// Turn on CO2 Buzzer if CO2 ppm greater than desiredCO2AlertLevel device twin
/// </summary>
static void CO2AlertHandler(EventLoopTimer* eventLoopTimer)
{
	//static bool buzzerState = false;

	if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0)
	{
		lp_terminate(ExitCode_ConsumeEventLoopTimeEvent);
	}
	else
	{
		if (desiredCO2AlertLevel.twinStateUpdated && !isnan(co2_ppm) && co2_ppm > * (float*)desiredCO2AlertLevel.twinState)
		{
			lp_gpioOn(&co2AlertPin);
			lp_timerOneShotSet(&co2AlertBuzzerOffOneShotTimer, &co2AlertBuzzerPeriod);
		}
	}
}

static void PublishTelemetryTimer(EventLoopTimer* eventLoopTimer)
{
	static int msgId = 0;

	if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0)
	{
		lp_terminate(ExitCode_ConsumeEventLoopTimeEvent);
	}
	else
	{
		if (!isnan(co2_ppm))
		{
			if (snprintf(msgBuffer, JSON_MESSAGE_BYTES, MsgTemplate, co2_ppm, temperature, relative_humidity, ++msgId) > 0)
			{
				Log_Debug("%s\n", msgBuffer);
				lp_azureMsgSendWithProperties(msgBuffer, telemetryMessageProperties, NELEMS(telemetryMessageProperties));
			}
			lp_deviceTwinReportState(&actualCO2Level, &co2_ppm);
		}
	}
}


/// <summary>
/// Read sensor and send to Azure IoT
/// </summary>
static void MeasureSensorHandler(EventLoopTimer* eventLoopTimer)
{
	if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0)
	{
		lp_terminate(ExitCode_ConsumeEventLoopTimeEvent);
	}
	else
	{
		if (scd30_read_measurement(&co2_ppm, &temperature, &relative_humidity) != STATUS_OK)
		{
			co2_ppm = NAN;
		}
	}
}

/// <summary>
/// Generic Device Twin Handler. It just sets reported state for the twin
/// </summary>
static void DeviceTwinGenericHandler(LP_DEVICE_TWIN_BINDING* deviceTwinBinding)
{
	lp_deviceTwinReportState(deviceTwinBinding, deviceTwinBinding->twinState);
	lp_deviceTwinAckDesiredState(deviceTwinBinding, deviceTwinBinding->twinState, LP_DEVICE_TWIN_COMPLETED);
}

static bool InitializeSdc30(void)
{
	uint16_t interval_in_seconds = 2;
	int retry = 0;
	uint8_t asc_enabled, enable_asc;

	sensirion_i2c_init();

	while (scd30_probe() != STATUS_OK && ++retry < 5)
	{
		printf("SCD30 sensor probing failed\n");
		sensirion_sleep_usec(1000000u);
	}

	if (retry >= 5) { return false; }

	/*
	When scd30 automatic self calibration activated for the first time a period of minimum 7 days is needed so
	that the algorithm can find its initial parameter set for ASC. The sensor has to be exposed to fresh air for at least 1 hour every day.
	Refer to the datasheet for further conditions and scd30.h for more info.
	*/

	if (scd30_get_automatic_self_calibration(&asc_enabled) == 0)
	{
		if (asc_enabled == 0)
		{
			enable_asc = 1;
			if (scd30_enable_automatic_self_calibration(enable_asc) == 0)
			{
				Log_Debug("scd30 automatic self calibration enabled. Takes 7 days, at least 1 hour/day outside, powered continuously");
			}
		}
	}

	scd30_set_measurement_interval(interval_in_seconds);
	sensirion_sleep_usec(20000u);
	scd30_start_periodic_measurement(0);
	sensirion_sleep_usec(interval_in_seconds * 1000000u);

	return true;
}

/// <summary>
///  Initialize PeripheralGpios, device twins, direct methods, timers.
/// </summary>
/// <returns>0 on success, or -1 on failure</returns>
static void InitPeripheralGpiosAndHandlers(void)
{
	lp_azureInitialize(lp_config.scopeId, lp_config.deviceTwinModelId);

	InitializeSdc30();

	lp_gpioSetOpen(PeripheralGpioSet, NELEMS(PeripheralGpioSet));
	lp_deviceTwinSetOpen(deviceTwinBindingSet, NELEMS(deviceTwinBindingSet));

	lp_timerSetStart(timerSet, NELEMS(timerSet));

	lp_azureToDeviceStart();

	scd30_read_measurement(&co2_ppm, &temperature, &relative_humidity);
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

	scd30_stop_periodic_measurement();

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