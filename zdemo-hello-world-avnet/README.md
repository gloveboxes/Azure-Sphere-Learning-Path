# Azure Sphere Hello World


## Introduction

Introduce hello world demo project structure

1. Avnet Start Kit, But works with Seeed Studio Boards too
2. Azure DPS and IoT Hub
3. app_manifest.json
4. Underlying libraries
5. [Azure Sphere Developer Learning Path](http://aka.ms/azure-sphere-developer-learning-path)


Demo flow

1. app_manifest.json
   2. whitelist peripherals
   2. whitelist network endpoints
2. Set up event timer and handler
3. Add peripheral
4. Add device twin
5. F5 to run
6. Review telemetry from azure portal shell
   * az iot hub monitor-events --hub-name iot-hub-azure-sphere


## Step 0: Whitelist peripherals and network endpoints

```json
    "Gpio": [ "$LED1", "$RELAY" ],
    "I2cMaster": [ "$I2cMaster2" ],
```

### Whitelist Network Endpoints

```json
"AllowedConnections": [
    "",
    "",
    ""
],
```

### Set the Device Provisioning ID Scope

```json
"CmdArgs": [ "ID Scope" ],
```

## Step 1: Streaming Telemetry

```c
// Timer
static LP_TIMER readSensorTimer = {
	.period = { 5, 0 },
	.name = "readSensorTimer",
	.handler = ReadSensorHandler
};

static void ReadSensorHandler(EventLoopTimer* eventLoopTimer)
{
	if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0)
	{
		lp_terminate(ExitCode_ConsumeEventLoopTimeEvent);
		return;
	}

	if (lp_readTelemetry(msgBuffer, JSON_MESSAGE_BYTES) > 0)
	{

		Log_Debug("%s\n", msgBuffer);

		lp_sendMsg(msgBuffer);
	}
}
```

## Step 2: Open Single pin peripheral

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

## Step 5: Sets

```c
// Sets
static LP_TIMER* timerSet[] = { &readSensorTimer };
static LP_PERIPHERAL_GPIO* peripheralSet[] = { &led1 };
static LP_DEVICE_TWIN_BINDING* deviceTwinBindingSet[] = { &led1_C2D_DeviceTwin };
```

## Step 6: Build, Deploy, Debug

<kbd>F5</kbd>

## Step 7: Monitor Telemetry

1. From the Azure Sphere Cloud Shell
2. Run the az monitor events command
	```bash
	az iot hub monitor-events --hub-name iot-hub-azure-sphere
	```

## Step 8: Explorer a Device Twin

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
8. sawp back to Visual Studio - the code should have halted at the breakpoint