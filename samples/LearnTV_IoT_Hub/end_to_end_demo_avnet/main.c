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
#include "config.h"
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
#include "board.h"
#include "imu_temp_pressure.h"
#include "light_sensor.h"
#endif // OEM_AVNET

// Hardware specific
#ifdef OEM_SEEED_STUDIO
#include "SEEED_STUDIO/board.h"
#endif // SEEED_STUDIO

// Number of bytes to allocate for the JSON telemetry message for IoT Central
#define JSON_MESSAGE_BYTES 256

// Forward signatures
static void ReadSensorHandler(EventLoopTimer* eventLoopTimer);
static void SampleRateDeviceTwinHandler(LP_DEVICE_TWIN_BINDING* deviceTwinBinding);
static LP_DIRECT_METHOD_RESPONSE_CODE LightControlDirectMethodHandler(JSON_Object* json, LP_DIRECT_METHOD_BINDING* directMethodBinding, char** responseMsg);

// Variables
LP_USER_CONFIG lp_config;
static char msgBuffer[JSON_MESSAGE_BYTES] = { 0 };

// Telemetry message template and properties
static const char* msgTemplate = "{ \"Temperature\":%3.2f, \"Humidity\":%3.1f, \"Pressure\":%3.1f, \"MsgId\":%d }";

static LP_MESSAGE_PROPERTY* msgProperties[] = {
	&(LP_MESSAGE_PROPERTY) { .key = "appid", .value = "lab-monitor" },
	&(LP_MESSAGE_PROPERTY) {.key = "format", .value = "json" },
	&(LP_MESSAGE_PROPERTY) {.key = "type", .value = "telemetry" },
	&(LP_MESSAGE_PROPERTY) {.key = "version", .value = "1" }
};

/***********************************************/
/*****  Declare Timers, Twins and Methods  *****/




/****************************************/
/*****  Initialise collection set  *****/

static LP_TIMER* timerSet[] = {  };
static LP_GPIO* gpioSet[] = {  };
static LP_DIRECT_METHOD_BINDING* directMethodBindingSet[] = {  };
static LP_DEVICE_TWIN_BINDING* deviceTwinBindingSet[] = {  };

/****************************************/
/*****  Demo Code                   *****/




/*****************************************************************************/
/*         Initialise and run code                                           */
/*****************************************************************************/

static void InitPeripheralsAndHandlers(void)
{
	lp_azureInitialize(lp_config.scopeId, lp_config.deviceTwinModelId);

	lp_initializeDevKit();

	lp_gpioSetOpen(gpioSet, NELEMS(gpioSet));
	lp_deviceTwinSetOpen(deviceTwinBindingSet, NELEMS(deviceTwinBindingSet));
	lp_directMethodSetOpen(directMethodBindingSet, NELEMS(directMethodBindingSet));

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
	lp_directMethodSetClose();

	lp_closeDevKit();

	lp_timerEventLoopStop();
}

int main(int argc, char* argv[])
{
	lp_registerTerminationHandler();

	lp_configParseCmdLineArguments(argc, argv, &lp_config);
	if (!lp_configValidate(&lp_config)) {
		return lp_getTerminationExitCode();
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