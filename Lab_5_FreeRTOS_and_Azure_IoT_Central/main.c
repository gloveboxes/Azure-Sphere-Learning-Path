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

#include "hw/azure_sphere_learning_path.h"

#include "learning_path_libs/azure_iot.h"
#include "learning_path_libs/exit_codes.h"
#include "learning_path_libs/globals.h"
#include "learning_path_libs/inter_core.h"
#include "learning_path_libs/peripheral_gpio.h"
#include "learning_path_libs/terminate.h"
#include "learning_path_libs/timer.h"

#include "applibs_versions.h"
#include <applibs/gpio.h>
#include <applibs/log.h>
#include <applibs/powermanagement.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>


 // Hardware specific

#ifdef OEM_AVNET
#include "learning_path_libs/AVNET/board.h"
#include "learning_path_libs/AVNET/imu_temp_pressure.h"
#include "learning_path_libs/AVNET/light_sensor.h"
#endif // OEM_AVNET

#ifdef OEM_SEEED_STUDIO
#include "learning_path_libs/SEEED_STUDIO/board.h"
#endif // SEEED_STUDIO

#define JSON_MESSAGE_BYTES 128  // Number of bytes to allocate for the JSON telemetry message for IoT Central

// Forward signatures
static void InitPeripheralGpiosAndHandlers(void);
static void ClosePeripheralGpiosAndHandlers(void);
static void Led2OffHandler(EventLoopTimer* eventLoopTimer);
static void MeasureSensorHandler(EventLoopTimer* eventLoopTimer);
static void NetworkConnectionStatusHandler(EventLoopTimer* eventLoopTimer);
static void InterCoreHandler(LP_INTER_CORE_BLOCK* ic_message_block);
static void DeviceTwinSetTemperatureHandler(LP_DEVICE_TWIN_BINDING* deviceTwinBinding);

static char msgBuffer[JSON_MESSAGE_BYTES] = { 0 };
static const char cstrJsonEvent[] = "{\"%s\":\"occurred\"}";
static const struct timespec led2BlinkPeriod = { 0, 500 * 1000 * 1000 };
LP_INTER_CORE_BLOCK ic_control_block;


// GPIO Output PeripheralGpios
static LP_PERIPHERAL_GPIO led2 = { .pin = LED2, .direction = LP_OUTPUT, .initialState = GPIO_Value_Low, .invertPin = true,
	.initialise = lp_openPeripheralGpio, .name = "led2" };

static LP_PERIPHERAL_GPIO networkConnectedLed = { .pin = NETWORK_CONNECTED_LED, .direction = LP_OUTPUT, .initialState = GPIO_Value_Low, .invertPin = true,
	.initialise = lp_openPeripheralGpio, .name = "networkConnectedLed" };

// Timers
static LP_TIMER led2BlinkOffOneShotTimer = { .period = { 0, 0 }, .name = "led2BlinkOffOneShotTimer", .handler = Led2OffHandler };
static LP_TIMER networkConnectionStatusTimer = { .period = { 5, 0 }, .name = "networkConnectionStatusTimer", .handler = NetworkConnectionStatusHandler };
static LP_TIMER measureSensorTimer = { .period = { 10, 0 }, .name = "measureSensorTimer", .handler = MeasureSensorHandler };

// Azure IoT Device Twins
static LP_DEVICE_TWIN_BINDING buttonPressed = { .twinProperty = "ButtonPressed", .twinType = LP_TYPE_STRING };
static LP_DEVICE_TWIN_BINDING desiredTemperature = { .twinProperty = "DesiredTemperature", .twinType = LP_TYPE_FLOAT, .handler = DeviceTwinSetTemperatureHandler };

// Initialize Sets
LP_PERIPHERAL_GPIO* peripheralGpioSet[] = { &networkConnectedLed, &led2 };
LP_TIMER* timerSet[] = { &led2BlinkOffOneShotTimer, &networkConnectionStatusTimer, &measureSensorTimer };
LP_DEVICE_TWIN_BINDING* deviceTwinBindingSet[] = { &buttonPressed, &desiredTemperature };


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
/// Device Twin Handler to set the desired temperature value on the Real-Time Core
/// </summary>
static void DeviceTwinSetTemperatureHandler(LP_DEVICE_TWIN_BINDING* deviceTwinBinding)
{
	ic_control_block.cmd = LP_IC_SET_DESIRED_TEMPERATURE;
	ic_control_block.temperature = *(float*)deviceTwinBinding->twinState;
	lp_sendInterCoreMessage(&ic_control_block, sizeof(ic_control_block));
}

/// <summary>
/// Turn on LED2, send message to Azure IoT and set a one shot timer to turn LED2 off
/// </summary>
static void SendMsgLed2On(char* message)
{
	lp_gpioOn(&led2);
	Log_Debug("%s\n", message);
	lp_sendMsg(message);
	lp_setOneShotTimer(&led2BlinkOffOneShotTimer, &led2BlinkPeriod);
}

/// <summary>
/// One shot timer to turn LED2 off
/// </summary>
static void Led2OffHandler(EventLoopTimer* eventLoopTimer)
{
	if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0)
	{
		lp_terminate(ExitCode_ConsumeEventLoopTimeEvent);
		return;
	}
	lp_gpioOff(&led2);
}

/// <summary>
/// Read sensor and send to Azure IoT
/// </summary>
static void MeasureSensorHandler(EventLoopTimer* eventLoopTimer)
{
	if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0)
	{
		lp_terminate(ExitCode_ConsumeEventLoopTimeEvent);
		return;
	}

	// send request to Real-Time core app to read temperature and pressure
	ic_control_block.cmd = LP_IC_TEMPERATURE_HUMIDITY;
	lp_sendInterCoreMessage(&ic_control_block, sizeof(ic_control_block));
}

/// <summary>
/// Callback handler for Inter-Core Messaging - Does Device Twin Update, and Event Message
/// </summary>
static void InterCoreHandler(LP_INTER_CORE_BLOCK* ic_message_block)
{
	static const char* msgTemplate = "{ \"Temperature\": \"%3.2f\", \"Pressure\":\"%3.1f\", \"MsgId\":%d }";
	static msgId = 0;
	int len = 0;

	switch (ic_message_block->cmd)
	{
	case LP_IC_EVENT_BUTTON_A:
		len = snprintf(msgBuffer, JSON_MESSAGE_BYTES, cstrJsonEvent, "ButtonA");
		lp_deviceTwinReportState(&buttonPressed, "ButtonA");					// TwinType = TYPE_STRING
		break;
	case LP_IC_EVENT_BUTTON_B:
		len = snprintf(msgBuffer, JSON_MESSAGE_BYTES, cstrJsonEvent, "ButtonB");
		lp_deviceTwinReportState(&buttonPressed, "ButtonB");					// TwinType = TYPE_STRING
		break;
	case LP_IC_TEMPERATURE_HUMIDITY:
		len = snprintf(msgBuffer, JSON_MESSAGE_BYTES, msgTemplate, ic_message_block->temperature, ic_message_block->pressure, msgId++);
		break;
	default:
		break;
	}

	if (len > 0)
	{
		SendMsgLed2On(msgBuffer);
	}
}

/// <summary>
///  Initialize PeripheralGpios, device twins, direct methods, timers.
/// </summary>
/// <returns>0 on success, or -1 on failure</returns>
static void InitPeripheralGpiosAndHandlers(void)
{
	lp_openPeripheralGpioSet(peripheralGpioSet, NELEMS(peripheralGpioSet));
	lp_openDeviceTwinSet(deviceTwinBindingSet, NELEMS(deviceTwinBindingSet));

	lp_startTimerSet(timerSet, NELEMS(timerSet));
	lp_startCloudToDevice();

	lp_enableInterCoreCommunications(rtAppComponentId, InterCoreHandler);  // Initialize Inter Core Communications

	ic_control_block.cmd = LP_IC_HEARTBEAT;		// Prime RT Core with Component ID Signature
	lp_sendInterCoreMessage(&ic_control_block, sizeof(ic_control_block));
}

/// <summary>
/// Close PeripheralGpios and handlers.
/// </summary>
static void ClosePeripheralGpiosAndHandlers(void)
{
	Log_Debug("Closing file descriptors\n");

	lp_stopTimerSet();
	lp_stopCloudToDevice();

	lp_closePeripheralGpioSet();
	lp_closeDeviceTwinSet();

	lp_stopTimerEventLoop();
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