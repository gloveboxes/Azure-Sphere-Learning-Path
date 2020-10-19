# Azure Sphere Hello World


## Introduction

Introduce hello world demo project structure

1. Avnet Start Kit, But works with Seeed Studio Boards too
2. Azure DPS and IoT Hub
3. app_manifest.json
4. Learning Path underlying libraries
5. [Azure Sphere Developer Learning Path](http://aka.ms/azure-sphere-developer-learning-path)

---

## Set up

1. Open Azure Portal
2. Start Azure Portal Command Shell
3. Start IoT Hub monitor
	```bash
	az iot hub monitor-events --hub-name iot-hub-azure-sphere
	```
---

Demo flow

1. app_manifest.json
   1. ID Scope
   2. whitelist peripherals
   3. whitelist network endpoints
2. Set up event timer and handler
3. Add peripheral
4. Add device twin
5. F5 to run
6. Review telemetry from azure portal shell
   * az iot hub monitor-events --hub-name iot-hub-azure-sphere

---

## Step 1: Whitelist peripherals and network endpoints

```json
    "Gpio": [ "$LED1", "$RELAY" ],
    "I2cMaster": [ "$I2cMaster2" ],
```

### Whitelist Network Endpoints

```json
"AllowedConnections": [
    "global.azure-devices-provisioning.net",
    "",
    ""
],
```

### Set the Device Provisioning ID Scope

```json
"CmdArgs": [ "ID Scope" ],
```

## Step 2: Streaming Telemetry

```c
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
			lp_azureMsgSend(msgBuffer);
		}
	}
}
```

## Step 3: Open Single pin peripheral

```c
static LP_PERIPHERAL_GPIO led1 = {
	.pin = RELAY,
	.direction = LP_OUTPUT,
	.initialState = GPIO_Value_Low,
	.invertPin = false,
	.initialise = lp_openPeripheralGpio,
	.name = "led1"
};


// Cloud to Device
static LP_DEVICE_TWIN_BINDING led1_C2D_DeviceTwin = {
	.twinProperty = "Relay1",
	.twinType = LP_TYPE_BOOL,
	.handler = led1ControlHandler
};


static void led1ControlHandler(LP_DEVICE_TWIN_BINDING* deviceTwinBinding)
{
	if (*(bool*)deviceTwinBinding->twinState)
	{
		lp_gpioOn(&led1);
	}
	else
	{
		lp_gpioOff(&led1);
	}
}
```

## Step 4: Sets

```c
// Sets
static LP_TIMER* timerSet[] = { &readSensorTimer };
static LP_PERIPHERAL_GPIO* peripheralSet[] = { &led1 };
static LP_DEVICE_TWIN_BINDING* deviceTwinBindingSet[] = { &led1_C2D_DeviceTwin };
```

## Step 5: Build, Deploy, Debug

<kbd>F5</kbd>

## Step 6: Monitor Telemetry

1. From the Azure Sphere Cloud Shell
2. Run the az monitor events command
	```bash
	az iot hub monitor-events --hub-name iot-hub-azure-sphere
	```

## Step 7: Explorer a Device Twin

1. From IoT Hub Portal
2. Select IoT Devices
3. Open your device
4. Select device twins
5. add/update desired property
	```json
   "Relay1": {
     "value": false
   },
	```
6. Set breakpoint in device twin handler
7. Back in the portal save the device twin
8. swap back to Visual Studio - the code should have halted at the breakpoint

## Extension sample - Direct Methods

```c
static LP_DIRECT_METHOD_BINDING resetDevice = { .methodName = "ResetMethod", .handler = ResetDirectMethodHandler };
static LP_TIMER resetDeviceOneShotTimer = { .period = {0, 0}, .name = "resetDeviceOneShotTimer", .handler = ResetDeviceHandler };

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
	static struct timespec period;

	if (!json_object_has_value_of_type(json, propertyName, JSONNumber))
	{
		return LP_METHOD_FAILED;
	}
	int seconds = (int)json_object_get_number(json, propertyName);

	// leave enough time for the device twin deviceResetUtc to update before restarting the device
	if (seconds > 2 && seconds < 10)
	{
		// Set One Shot LP_TIMER
		period = (struct timespec){ .tv_sec = seconds, .tv_nsec = 0 };
		lp_setOneShotTimer(&resetDeviceOneShotTimer, &period);

		return LP_METHOD_SUCCEEDED;
	}
	else
	{
		return LP_METHOD_FAILED;
	}
}
```

```c
// Sets
static LP_TIMER* timerSet[] = { &readSensorTimer, &resetDeviceOneShotTimer };
static LP_PERIPHERAL_GPIO* peripheralSet[] = { &led1 };
static LP_DEVICE_TWIN_BINDING* deviceTwinBindingSet[] = { &led1_C2D_DeviceTwin };
LP_DIRECT_METHOD_BINDING* directMethodBindingSet[] = { &resetDevice };
```
