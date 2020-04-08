# Azure Sphere Learning Path

# Azure Sphere Hello World



## Introduction

Introduce hello world demo project structure

1. Avnet Start Kit, But works with Seeed Studio Boards too
1. app_manifest.json
2. CMakelist
3. Underlying libraries
4. [Azure Sphere Developer Learning Path](http://aka.ms/azure-sphere-developer-learning-path)


## Step 1: Streaming Telemetry

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
		DeviceTwinReportState(&lastReadUtc_D2C_DeviceTwin, GetCurrentUtc(msgBuffer, sizeof(msgBuffer)));
	}
}

## Step 2: Open Single pin peripheral

static Peripheral led1 = {
	.pin = RELAY,
	.direction = OUTPUT,
	.initialState = GPIO_Value_Low,
	.invertPin = false,
	.initialise = OpenPeripheral,
	.name = "led1"
};

## Step 3 Device Twins

// Cloud to Device
static DeviceTwinBinding led1_C2D_DeviceTwin = {
	.twinProperty = "Relay1",
	.twinType = TYPE_BOOL,
	.handler = led1ControlHandler
};

// Device to Cloud
static DeviceTwinBinding lastReadUtc_D2C_DeviceTwin = {
	.twinProperty = "LastReadUTC",
	.twinType = TYPE_STRING
};

## Step 4: Device Twin - Cloud to Device

static void led1ControlHandler(DeviceTwinBinding* deviceTwinBinding) {

	if (*(bool*)deviceTwinBinding->twinState) {
		Gpio_On(&led1);
	}
	else {
		Gpio_Off(&led1);
	}
}

## Step 5: Sets

// Sets
static Timer* timerSet[] = { &readSensorTimer };
static Peripheral* peripheralSet[] = { &led1 };
static DeviceTwinBinding* deviceTwinBindingSet[] = { &led1_C2D_DeviceTwin, &lastReadUtc_D2C_DeviceTwin };


## Step 6: Build, Deploy, Debug

<kbd>F5</kbd>


## Demo 2 Device to Cloud Telemetry Message

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

```c
static Peripheral* peripheralSet[] = { &led1 };
```

```c
static Timer readSensorTimer = {
	.period = { 5, 0 },
	.name = "readSensorTimer",
	.handler = ReadSensorHandler
};
```

```c
static Timer* timerSet[] = { &readSensorTimer };
```

```c
// Read Sensor
static void ReadSensorHandler(EventLoopTimer* eventLoopTimer) {
	if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0) {
		Terminate();
		return;
	}

	if (ReadTelemetry(msgBuffer, JSON_MESSAGE_BYTES) > 0) {
		SendMsg(msgBuffer);
	}
}
```

# Demo 3 Device Twin: Device to Cloud

```c
static DeviceTwinBinding lastReadUtc = {
	.twinProperty = "LastReadUTC",
	.twinType = TYPE_STRING
};
```

```c
static DeviceTwinBinding* deviceTwinBindingSet[] = { &lastReadUtc };
```

```c
// Read Sensor
static void ReadSensorHandler(EventLoopTimer* eventLoopTimer) {
	if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0) {
		Terminate();
		return;
	}

	if (ReadTelemetry(msgBuffer, JSON_MESSAGE_BYTES) > 0) {
		SendMsg(msgBuffer);

		DeviceTwinReportState(&lastReadUtc, GetCurrentUtc(msgBuffer, sizeof(msgBuffer)));
	}
}
```

## Demo 4 Device Twin: Cloud to Device

```c
static DeviceTwinBinding sensorSampleRate = { 
	.twinProperty = "SensorSampleRate", 
	.twinType = TYPE_INT, 
	.handler = SensorSampleRate 
};
```

```c
static DeviceTwinBinding* deviceTwinBindingSet[] = { &lastReadUtc, &sensorSampleRate };
```

```
static void SensorSampleRate(DeviceTwinBinding* deviceTwinBinding) {
	int sampleRate = *(int*)deviceTwinBinding->twinState;

	if (sampleRate > 0 && sampleRate < 100) {
		ChangeTimer(&readSensorTimer, sampleRate);
	}
}
```

Demo 4 Alternative: Device Twin: Cloud to Device

```c
static DeviceTwinBinding led1DeviceTwin = { 
	.twinProperty = "Relay1", 
	.twinType = TYPE_BOOL, 
	.handler = led1ControlHandler 
};
```

```c
static DeviceTwinBinding* deviceTwinBindingSet[] = { &lastReadUtc, &led1DeviceTwin };
```

```
static void led1ControlHandler(DeviceTwinBinding* deviceTwinBinding) {

	if (*(bool*)deviceTwinBinding->twinState) {
		Gpio_On(&led1);
	}
	else {
		Gpio_Off(&led1);
	}
}
```




