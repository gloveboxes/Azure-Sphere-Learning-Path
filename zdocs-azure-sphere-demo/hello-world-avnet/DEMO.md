# Azure Sphere Hello World

Avnet Start Kit

## Demo 1 

Introduce hello world demo project structure

1. app_manifest.json
2. CMakelist
3. Underlying libraries
4. [Azure Sphere Developer Learning Path](http://aka.ms/azure-sphere-developer-learning-path)

## Demo 2

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

# Demo 3 Device Twin

```c
static DeviceTwinBinding lastReadUtc = {
	.twinProperty = "lastReadUtc",
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

# Demo 4 Blink LED

```c
static Peripheral led1 = {
	.pin = RELAY, 
	.direction = OUTPUT, 
	.initialState = GPIO_Value_Low, 
	.invertPin = true,
	.initialise = OpenPeripheral, 
	.name = "led1"
};
```

```c
static Peripheral* peripheralSet[] = { &led1 };
```

```c
// Read Sensor
static void ReadSensorHandler(EventLoopTimer* eventLoopTimer) {
	if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0) {
		Terminate();
		return;
	}

	if (ReadTelemetry(msgBuffer, JSON_MESSAGE_BYTES) > 0) {
		Gpio_On(&led1);
		SendMsg(msgBuffer);
		DeviceTwinReportState(&lastReadUtc, GetCurrentUtc(msgBuffer, sizeof(msgBuffer)));
		Gpio_On(&led1);
	}
}
```

## Demo 5

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

