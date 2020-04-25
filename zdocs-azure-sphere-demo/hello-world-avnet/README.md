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
6. Review telemetry from azure portal shel
   * az iot hub monitor-events --hub-name iot-hub-azure-sphere


## Step 0: Whitelist peripherals and network endpoints

```json
"Gpio": [ "$RELAY" ],
"I2cMaster": [ "$AVNET_MT3620_SK_ISU2_I2C" ],
"Adc": [ "$AVNET_MT3620_SK_ADC_CONTROLLER0" ],
```

### Whitelist Network Endpoints

```json
"AllowedConnections": [
    "",
    "",
    ""
],
```


## Step 1: Streaming Telemetry

```c
// Timer
static Timer readSensorTimer = {
	.period = { 5, 0 },
	.name = "readSensorTimer",
	.handler = ReadSensorHandler
};

static void ReadSensorHandler(EventLoopTimer* eventLoopTimer) {
	if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0) {
		Terminate();
		return;
	}

	if (ReadTelemetry(msgBuffer, JSON_MESSAGE_BYTES) > 0) {

		Log_Debug("%s\n", msgBuffer);

		SendMsg(msgBuffer);
	}
}
```

## Step 2: Open Single pin peripheral

```c
static Peripheral led1 = {
	.pin = RELAY,
	.direction = OUTPUT,
	.initialState = GPIO_Value_Low,
	.invertPin = false,
	.initialise = OpenPeripheral,
	.name = "led1"
};
```

## Step 3 Device Twins

```c
// Cloud to Device
static DeviceTwinBinding led1_C2D_DeviceTwin = {
	.twinProperty = "Relay1",
	.twinType = TYPE_BOOL,
	.handler = led1ControlHandler
};


static void led1ControlHandler(DeviceTwinBinding* deviceTwinBinding) {
	if (*(bool*)deviceTwinBinding->twinState) {
		Gpio_On(&led1);
	}
	else {
		Gpio_Off(&led1);
	}
}
```

## Step 5: Sets

```c
// Sets
static Timer* timerSet[] = { &readSensorTimer };
static Peripheral* peripheralSet[] = { &led1 };
static DeviceTwinBinding* deviceTwinBindingSet[] = { &led1_C2D_DeviceTwin };
```

## Step 6: Build, Deploy, Debug

<kbd>F5</kbd>

