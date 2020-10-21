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

1. Really important concept for Azure Sphere - the platform is secure by default
2. Apps have no access to any resources unless explicity decalred - hardware and network endpoints 
3. app_manifest.json
   1. ID Scope
   2. Capabilities have to be expliciticity declared
    	- peripherals
   		- allows network endpoints
1. Discuss main.c structure
1. Set up event timer and handler
1. Add peripheral
1. Add device twin
1. Add Direct method

---

## Step 1: Declare peripherals and network endpoints

```json
    "Gpio": [ "$LED1", "$RELAY" ],
    "I2cMaster": [ "$I2cMaster2" ],
```

### Decalre Network Endpoints

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
		if (snprintf(msgBuffer, JSON_MESSAGE_BYTES, MsgTemplate, environment.temperature, environment.humidity, environment.pressure, msgId++) > 0)
		{
			Log_Debug(msgBuffer);
			lp_azureMsgSendWithProperties(msgBuffer, telemetryMessageProperties, NELEMS(telemetryMessageProperties));
		}
	}
}
```

### Send telemetry messages to Azure IoT

1. Set breakpoint at beginning of ReadSensorHandler function
1. Press <kbd>F5</kbd> to run
1. From Azure IoT Hub CLI or Azure IoT Explorer view telemetry
1. Step through the breakpoint and discuss

> Notes: CLI example
    - HUB_NAME={your hub name}
    - CLI Usage example: az iot hub monitor-events --hub-name $HUB_NAME -p app -o table

---

## Step 3: Remote control lab light

**Add GPIO declaration**

```c
static LP_GPIO relay = {
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

	lp_gpioStateSet(&relay, state);

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

> Notes: CLI example
    - DEVICE_ID={your Azure Sphere device ID}
    - HUB_NAME={your hub name}
    - az iot hub invoke-device-method --method-name "LightControl" --method-payload '{"light_state":true}' --device-id "\${DEVICE_ID,,}" -n $HUB_NAME --timeout 120


### Remote Control Serverless Solution

Invoke direct method via serverless web app and Azure Functions

[Azure Sphere Lab Light Control with Azure IoT Direct Methods](https://aka.ms/learn/iothub/demo)

> **Important**. Ensure correct Azure Function *IOT_HUB_SERVICE_CONNECTION_STRING* and *AZURE_SPHERE_IOT_HUB_DEVICE_ID* app settings set for demo.  

---

## Step 4: Set Sensor Sample Rate 

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

> Notes: CLI example
    - DEVICE_ID={your Azure Sphere device ID}
    - HUB_NAME={your hub name}
    - az iot hub device-twin update --device-id "\${DEVICE_ID,,}" -n $HUB_NAME --set properties.desired='{"SampleRateSeconds":{"value":8}}'

---


