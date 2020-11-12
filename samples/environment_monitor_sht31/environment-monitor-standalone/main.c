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

static char msgBuffer[JSON_MESSAGE_BYTES] = { 0 };
float temperature, humidity;

// Timers
static LP_TIMER measureSensorTimer = { .period = {4, 0}, .name = "measureSensorTimer", .handler = MeasureSensorHandler };

// Initialize Sets
LP_TIMER* timerSet[] = { &measureSensorTimer };

static const char* MsgTemplate = "{ \"Temperature\": %3.2f, \"Humidity\": \"%3.1f\", \"MsgId\":%d }";

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
		}
	}
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
	lp_timerSetStart(timerSet, NELEMS(timerSet));
}

/// <summary>
///     Close PeripheralGpios and handlers.
/// </summary>
static void ClosePeripheralGpiosAndHandlers(void)
{
	Log_Debug("Closing file descriptors\n");
	lp_timerSetStop(timerSet, NELEMS(timerSet));
	lp_timerEventLoopStop();
}

int main(int argc, char* argv[])
{
	lp_registerTerminationHandler();

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