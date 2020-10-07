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

---

## Step 2: Streaming Telemetry

**Add timer**

```c
// Timer
static LP_TIMER readSensorTimer = {
	.period = {5, 0},
	.name = "readSensorTimer",
	.handler = ReadSensorHandler };
```

**Update gpio set**

```c
static LP_TIMER* timerSet[] = { &readSensorTimer };
```

**Add handler**

```c
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
			lp_sendMsg(msgBuffer);
		}
	}
}
```

### Send telemetry messages to Azure IoT

1. Set breakpoint at beginning of ReadSensorHandler function
1. Press <kbd>F5</kbd> to run
1. From Azure IoT Hub CLI or Azure IoT Explorer view telemetry
1. Step through the breakpoint and discuss

---

## Step 3: Set Sensor Sample Rate 

**Add device twin**

```c
// Device Twin to set sample rate 
static LP_DEVICE_TWIN_BINDING sampleRate_DeviceTwin = {
	.twinProperty = "SampleRateSeconds",
	.twinType = LP_TYPE_INT,
	.handler = SampleRateDeviceTwinHandler };
```

**Update device twin set**

```c
static LP_DEVICE_TWIN_BINDING* deviceTwinBindingSet[] = { &sampleRate_DeviceTwin };
```

**Add handler code**

```c
/// <summary>
/// This handler called when device twin desired 'SampleRateSeconds' recieved
/// </summary>
static void SampleRateDeviceTwinHandler(LP_DEVICE_TWIN_BINDING* deviceTwinBinding)
{
	struct timespec sampleRateSeconds = { *(int*)deviceTwinBinding->twinState, 0 };

	if (sampleRateSeconds.tv_sec > 0 && sampleRateSeconds.tv_sec < (5 * 60)) // check sensible range
	{
		lp_changeTimer(&readSensorTimer, &sampleRateSeconds);
		lp_deviceTwinReportState(deviceTwinBinding, deviceTwinBinding->twinState);
	}
}
```

### Set sensor sample rate run instructions

1. Set breakpoint at beginning of SampleRateDeviceTwinHandler function
1. Press <kbd>F5</kbd> to run
1. Wait for first telemsg sent so you know you are Azure IoT connected
1. From Azure IoT Hub CLI or Azure IoT Explorer set desired twin

    ```json
    "SensorRate": { "value": 6 }
    ```

1. Step through the breakpoint and discuss

---

## Step 4: Remote control lab light

**Add GPIO declaration**

```c
static LP_PERIPHERAL_GPIO relay = {
	.pin = RELAY,
	.direction = LP_OUTPUT,
	.initialState = GPIO_Value_Low,
	.invertPin = false,
	.initialise = lp_openPeripheralGpio,
	.name = "relay" };
```

**Update GPIO set**

```c
static LP_PERIPHERAL_GPIO* peripheralSet[] = { &relay };
```

**Add direct method declaration**

```c
static LP_DIRECT_METHOD_BINDING lightControl = {
	.methodName = "LightControl",
	.handler = LightControlDirectMethodHandler };
```

**Update Direct Method set**

```c
static LP_DIRECT_METHOD_BINDING* directMethodBindingSet[] = { &lightControl };
```

**Add direct method handler**

```c
/// <summary>
/// Remote control lab light Direct Method 'LightControl' {"light_state":true}
/// </summary>
static LP_DIRECT_METHOD_RESPONSE_CODE LightControlDirectMethodHandler(JSON_Object* json, LP_DIRECT_METHOD_BINDING* directMethodBinding, char** responseMsg)
{
	const char propertyName[] = "light_state";

	if (!json_object_has_value_of_type(json, propertyName, JSONBoolean))
	{
		return LP_METHOD_FAILED;
	}
	
	bool state = (bool)json_object_get_boolean(json, propertyName);

	if (state)
	{
		lp_gpioOn(&relay);
	}
	else
	{
		lp_gpioOff(&relay);
	}

	return LP_METHOD_SUCCEEDED;
}
```

### Remote control light run instructions

1. Set breakpoint at beginning of LightControlDirectMethodHandler function
1. Press <kbd>F5</kbd> to run
1. Wait for first telemsg sent so you know you are Azure IoT connected
1. From Azure IoT Hub CLI or Azure IoT Explorer
    * Method name: **LightControl**
  	* Payload: **{"light_state":true}**
1. Step through the breakpoint and discuss

---

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
   "SampleRateSeconds": {
     "value": 2
   },
	```
6. Set breakpoint in device twin handler
7. Back in the portal save the device twin
8. swap back to Visual Studio Code - the code should have halted at the breakpoint
