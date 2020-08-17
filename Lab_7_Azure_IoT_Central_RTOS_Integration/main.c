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
#include "inter_core.h"
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

#define JSON_MESSAGE_BYTES 256  // Number of bytes to allocate for the JSON telemetry message for IoT Central

// Forward signatures
static void Led2OffHandler(EventLoopTimer* eventLoopTimer);
static void MeasureSensorHandler(EventLoopTimer* eventLoopTimer);
static void NetworkConnectionStatusHandler(EventLoopTimer* eventLoopTimer);
static void InterCoreHandler(LP_INTER_CORE_BLOCK* ic_message_block);
static void ResetDeviceHandler(EventLoopTimer* eventLoopTimer);
static void DeviceTwinSetTemperatureHandler(LP_DEVICE_TWIN_BINDING* deviceTwinBinding);
static void DeviceTwinBlinkRateHandler(LP_DEVICE_TWIN_BINDING* deviceTwinBinding);
static void DeviceTwinRelay1Handler(LP_DEVICE_TWIN_BINDING* deviceTwinBinding);
static LP_DIRECT_METHOD_RESPONSE_CODE ResetDirectMethodHandler(JSON_Object* json, LP_DIRECT_METHOD_BINDING* directMethodBinding, char** responseMsg);

static char msgBuffer[JSON_MESSAGE_BYTES] = { 0 };
static const char cstrJsonEvent[] = "{\"%s\":\"occurred\"}";
static const struct timespec sendMsgLedBlinkPeriod = { 0, 500 * 1000 * 1000 };
LP_INTER_CORE_BLOCK ic_control_block;


// GPIO Output PeripheralGpios
static LP_PERIPHERAL_GPIO led2 = { .pin = LED2, .direction = LP_OUTPUT, .initialState = GPIO_Value_Low, .invertPin = true,
	.initialise = lp_openPeripheralGpio, .name = "led2" };

static LP_PERIPHERAL_GPIO networkConnectedLed = { .pin = NETWORK_CONNECTED_LED, .direction = LP_OUTPUT, .initialState = GPIO_Value_Low, .invertPin = true,
	.initialise = lp_openPeripheralGpio, .name = "networkConnectedLed" };

static LP_PERIPHERAL_GPIO relay1 = { .pin = RELAY, .direction = LP_OUTPUT, .initialState = GPIO_Value_Low, .invertPin = false,
	.initialise = lp_openPeripheralGpio, .name = "relay1" };

// Timers
static LP_TIMER led2BlinkOffOneShotTimer = { .period = { 0, 0 }, .name = "led2BlinkOffOneShotTimer", .handler = Led2OffHandler };
static LP_TIMER networkConnectionStatusTimer = { .period = { 5, 0 }, .name = "networkConnectionStatusTimer", .handler = NetworkConnectionStatusHandler };
static LP_TIMER measureSensorTimer = { .period = { 10, 0 }, .name = "measureSensorTimer", .handler = MeasureSensorHandler };
static LP_TIMER resetDeviceOneShotTimer = { .period = { 0, 0 }, .name = "resetDeviceOneShotTimer", .handler = ResetDeviceHandler };

// Azure IoT Device Twins
static LP_DEVICE_TWIN_BINDING buttonPressed = { .twinProperty = "ButtonPressed", .twinType = LP_TYPE_STRING };
static LP_DEVICE_TWIN_BINDING desiredTemperature = { .twinProperty = "DesiredTemperature", .twinType = LP_TYPE_FLOAT, .handler = DeviceTwinSetTemperatureHandler };
static LP_DEVICE_TWIN_BINDING led1BlinkRate = { .twinProperty = "LedBlinkRate", .twinType = LP_TYPE_INT, .handler = DeviceTwinBlinkRateHandler };
static LP_DEVICE_TWIN_BINDING relay1DeviceTwin = { .twinProperty = "Relay1", .twinType = LP_TYPE_BOOL, .handler = DeviceTwinRelay1Handler };
static LP_DEVICE_TWIN_BINDING deviceResetUtc = { .twinProperty = "DeviceResetUTC", .twinType = LP_TYPE_STRING };

// Azure IoT Direct Methods
static LP_DIRECT_METHOD_BINDING resetDevice = { .methodName = "ResetMethod", .handler = ResetDirectMethodHandler };

// Initialize Sets
LP_PERIPHERAL_GPIO* peripheralGpioSet[] = { &networkConnectedLed, &led2, &relay1 };
LP_TIMER* timerSet[] = { &led2BlinkOffOneShotTimer, &networkConnectionStatusTimer, &measureSensorTimer, &resetDeviceOneShotTimer };
LP_DEVICE_TWIN_BINDING* deviceTwinBindingSet[] = { &led1BlinkRate, &buttonPressed, &desiredTemperature, &relay1DeviceTwin, &deviceResetUtc };
LP_DIRECT_METHOD_BINDING* directMethodBindingSet[] = { &resetDevice };

// Message property set
static LP_MESSAGE_PROPERTY messageAppId = { .key = "appid", .value = "hvac" };
static LP_MESSAGE_PROPERTY messageType = { .key = "type", .value = "telemetry" };
static LP_MESSAGE_PROPERTY messageFormat = { .key = "format", .value = "json" };
static LP_MESSAGE_PROPERTY messageVersion = { .key = "version", .value = "1" };
static LP_MESSAGE_PROPERTY* telemetryMessageProperties[] = { &messageAppId, &messageType, &messageFormat, &messageVersion };


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
/// Device Twin to control relay "Relay1": {"value": true }, "Relay1": {"value": false },
/// </summary>
static void DeviceTwinRelay1Handler(LP_DEVICE_TWIN_BINDING* deviceTwinBinding)
{
	if (*(bool*)deviceTwinBinding->twinState) { lp_gpioOn(&relay1); }
	else { lp_gpioOff(&relay1); }
}

/// <summary>
/// Set Blink Rate using Device Twin "LedBlinkRate": {"value": 0}
/// </summary>
static void DeviceTwinBlinkRateHandler(LP_DEVICE_TWIN_BINDING* deviceTwinBinding)
{
	// send request to Real-Time core app to change blink rate
	ic_control_block.cmd = LP_IC_BLINK_RATE;
	ic_control_block.blinkRate = *(int*)deviceTwinBinding->twinState;
	lp_sendInterCoreMessage(&ic_control_block, sizeof(ic_control_block));
}

/// <summary>
/// Turn on LED2, send message to Azure IoT and set a one shot timer to turn LED2 off
/// </summary>
static void SendMsgLed2On(char* message)
{
	lp_gpioOn(&led2);
	Log_Debug("%s\n", message);

	// optional: message properties can be used for message routing in IOT Hub
	lp_setMessageProperties(telemetryMessageProperties, NELEMS(telemetryMessageProperties));

	lp_sendMsg(message);

	// optional: clear if you are sending other message types that don't require these properties
	lp_clearMessageProperties();

	lp_setOneShotTimer(&led2BlinkOffOneShotTimer, &sendMsgLedBlinkPeriod);
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

	// send request to Real-Time core app to read temperature, pressure, and humidity
	ic_control_block.cmd = LP_IC_TEMPERATURE_PRESSURE_HUMIDITY;
	lp_sendInterCoreMessage(&ic_control_block, sizeof(ic_control_block));
}

/// <summary>
/// Callback handler for Inter-Core Messaging - Does Device Twin Update, and Event Message
/// </summary>
static void InterCoreHandler(LP_INTER_CORE_BLOCK* ic_message_block)
{
	static const char* msgTemplate = "{ \"Temperature\": \"%3.2f\", \"Humidity\": \"%3.1f\", \"Pressure\":\"%3.1f\", \"Light\":%d, \"MsgId\":%d }";
	static int msgId = 0;
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
	case LP_IC_TEMPERATURE_PRESSURE_HUMIDITY:
		len = snprintf(msgBuffer, JSON_MESSAGE_BYTES, msgTemplate, ic_message_block->temperature, ic_message_block->humidity, ic_message_block->pressure, 0, msgId++);
		break;
	case LP_IC_BLINK_RATE:
		lp_deviceTwinReportState(&led1BlinkRate, &ic_message_block->blinkRate);
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
/// Reset the Device
/// </summary>
static void ResetDeviceHandler(EventLoopTimer* eventLoopTimer)
{
	if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0)
	{
		lp_terminate(ExitCode_ConsumeEventLoopTimeEvent);
		return;
	}
	PowerManagement_ForceSystemReboot();
}

/// <summary>
/// Start Device Power Restart Direct Method 'ResetMethod' {"reset_timer":5}
/// </summary>
static LP_DIRECT_METHOD_RESPONSE_CODE ResetDirectMethodHandler(JSON_Object* json, LP_DIRECT_METHOD_BINDING* directMethodBinding, char** responseMsg)
{
	const char propertyName[] = "reset_timer";
	const size_t responseLen = 60; // Allocate and initialize a response message buffer. The calling function is responsible for the freeing memory
	static struct timespec period;

	*responseMsg = (char*)malloc(responseLen);
	memset(*responseMsg, 0, responseLen);

	if (!json_object_has_value_of_type(json, propertyName, JSONNumber))
	{
		return LP_METHOD_FAILED;
	}
	int seconds = (int)json_object_get_number(json, propertyName);

	// leave enough time for the device twin deviceResetUtc to update before restarting the device
	if (seconds > 2 && seconds < 10)
	{
		// Report Device Reset UTC
		lp_deviceTwinReportState(&deviceResetUtc, lp_getCurrentUtc(msgBuffer, sizeof(msgBuffer)));			// LP_TYPE_STRING

		// Create Direct Method Response
		snprintf(*responseMsg, responseLen, "%s called. Reset in %d seconds", directMethodBinding->methodName, seconds);

		// Set One Shot LP_TIMER
		period = (struct timespec){ .tv_sec = seconds, .tv_nsec = 0 };
		lp_setOneShotTimer(&resetDeviceOneShotTimer, &period);

		return LP_METHOD_SUCCEEDED;
	}
	else
	{
		snprintf(*responseMsg, responseLen, "%s called. Reset Failed. Seconds out of range: %d", directMethodBinding->methodName, seconds);
		return LP_METHOD_FAILED;
	}
}

/// <summary>
///  Initialize PeripheralGpios, device twins, direct methods, timers.
/// </summary>
/// <returns>0 on success, or -1 on failure</returns>
static void InitPeripheralAndHandlers(void)
{
	lp_openPeripheralGpioSet(peripheralGpioSet, NELEMS(peripheralGpioSet));
	lp_openDeviceTwinSet(deviceTwinBindingSet, NELEMS(deviceTwinBindingSet));
	lp_openDirectMethodSet(directMethodBindingSet, NELEMS(directMethodBindingSet));

	lp_startTimerSet(timerSet, NELEMS(timerSet));
	lp_startCloudToDevice();

	lp_enableInterCoreCommunications(rtAppComponentId, InterCoreHandler);  // Initialize Inter Core Communications

	ic_control_block.cmd = LP_IC_HEARTBEAT;		// Prime RT Core with Component ID Signature
	lp_sendInterCoreMessage(&ic_control_block, sizeof(ic_control_block));
}

/// <summary>
/// Close PeripheralGpios and handlers.
/// </summary>
static void ClosePeripheralAndHandlers(void)
{
	Log_Debug("Closing file descriptors\n");

	lp_stopTimerSet();
	lp_stopCloudToDevice();

	lp_closePeripheralGpioSet();
	lp_closeDeviceTwinSet();
	lp_closeDirectMethodSet();


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

	InitPeripheralAndHandlers();

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

	ClosePeripheralAndHandlers();

	Log_Debug("Application exiting.\n");
	return lp_getTerminationExitCode();
}