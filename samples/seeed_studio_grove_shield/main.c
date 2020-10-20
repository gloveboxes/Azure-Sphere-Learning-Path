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


// Number of bytes to allocate for the JSON telemetry message for IoT Central
#define JSON_MESSAGE_BYTES 256  

// Forward signatures
static void ReadSensorHandler(EventLoopTimer* eventLoopTimer);
static void SampleRateHandler(LP_DEVICE_TWIN_BINDING* deviceTwinBinding);

// Variables
static char msgBuffer[JSON_MESSAGE_BYTES] = { 0 };

// Telemetry message template and properties
static const char* MsgTemplate = "{ \"Temperature\": \"%3.2f\", \"Humidity\": \"%3.1f\", \"Pressure\":\"%3.1f\", \"Light\":%d, \"MsgId\":%d }";

static LP_MESSAGE_PROPERTY messageAppId = { .key = "appid", .value = "hvac" };
static LP_MESSAGE_PROPERTY messageFormat = { .key = "format", .value = "json" };
static LP_MESSAGE_PROPERTY telemetryMessageType = { .key = "type", .value = "telemetry" };
static LP_MESSAGE_PROPERTY messageVersion = { .key = "version", .value = "1" };

static LP_MESSAGE_PROPERTY* telemetryMessageProperties[] = { &messageAppId, &telemetryMessageType, &messageFormat, &messageVersion };

// Timer
static LP_TIMER readSensorTimer = {
	.period = { 5, 0 },
	.name = "readSensorTimer",
	.handler = ReadSensorHandler
};

static void ReadSensorHandler(EventLoopTimer* eventLoopTimer)
{
	static int msgId = 0;
	static LP_ENVIRONMENT environment;

	if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0)
	{
		lp_terminate(ExitCode_ConsumeEventLoopTimeEvent);
		return;
	}

	if (lp_readTelemetry(&environment))
	{
		if (snprintf(msgBuffer, JSON_MESSAGE_BYTES, MsgTemplate, environment.temperature, environment.humidity, environment.pressure, environment.light, msgId++) > 0)
		{
			lp_azureMsgSendWithProperties(msgBuffer, telemetryMessageProperties, NELEMS(telemetryMessageProperties));
		}
	}
}

// Cloud to Device
static LP_DEVICE_TWIN_BINDING sampleRate_DeviceTwin = {
	.twinProperty = "SampleRateSeconds",
	.twinType = LP_TYPE_INT,
	.handler = SampleRateHandler
};


static void SampleRateHandler(LP_DEVICE_TWIN_BINDING* deviceTwinBinding)
{
	struct timespec sampleRateSeconds = { *(int*)deviceTwinBinding->twinState, 0 };

	if (sampleRateSeconds.tv_sec > 0 && sampleRateSeconds.tv_sec < (5 * 60))
	{
		lp_timerChange(&readSensorTimer, &sampleRateSeconds);
	}
}

// Sets
static LP_TIMER* timerSet[] = { &readSensorTimer };
static LP_GPIO* gpioSet[] = { };
static LP_DEVICE_TWIN_BINDING* deviceTwinBindingSet[] = { &sampleRate_DeviceTwin };


static void InitPeripheralsAndHandlers(void)
{
	lp_initializeDevKit();

	lp_gpioSetOpen(gpioSet, NELEMS(gpioSet));
	lp_deviceTwinSetOpen(deviceTwinBindingSet, NELEMS(deviceTwinBindingSet));

	lp_timerSetStart(timerSet, NELEMS(timerSet));
	lp_azureToDeviceStart();
}

static void ClosePeripheralsAndHandlers(void)
{
	Log_Debug("Closing file descriptors\n");

	lp_timerSetStop(timerSet, NELEMS(timerSet));
	lp_azureToDeviceStop();

	lp_gpioSetClose(gpioSet, NELEMS(gpioSet));
	lp_deviceTwinSetClose();

	lp_closeDevKit();

	lp_timerEventLoopStop();
}

int main(int argc, char* argv[])
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

		int result = EventLoop_Run(lp_timerGetEventLoop(), -1, true);

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