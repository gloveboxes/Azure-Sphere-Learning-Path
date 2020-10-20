# Azure Sphere High level with Real time integration plus IoT Central


## Introduction

Introduce hello world demo project structure

1. Avnet Start Kit, But works with Seeed Studio Boards too
2. Azure DPS and IoT Hub
3. app_manifest.json
4. Learning Path underlying libraries

---

## Set up

1. Open pre-created IoT Central app

---

Demo flow

1. Really important concept for Azure Sphere - the platform is secure/locked down by default
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

### Declare Network Endpoints

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
/// <summary>
/// Read sensor and send to Azure IoT
/// </summary>
static void ReadSensorHandler(EventLoopTimer* eventLoopTimer)
{
    if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0) {
        lp_terminate(ExitCode_ConsumeEventLoopTimeEvent);
    }
    else {
        // send request to Real-Time core app to read temperature, pressure, and humidity
        ic_control_block.cmd = LP_IC_ENVIRONMENT_SENSOR;
        lp_interCoreSendMessage(&ic_control_block, sizeof(ic_control_block));
    }
}
```

### Send telemetry messages to Azure IoT Central

View charts and raw data

---

## Step 3: Remote control lab light

**Add GPIO declaration**

```c
static LP_GPIO relay = {
    .pin = RELAY,
    .direction = LP_OUTPUT,
    .initialState = GPIO_Value_Low,
    .invertPin = false,
    .name = "relay" };
```

**Update GPIO set**

```c
static LP_GPIO* gpioSet[] = { &relay };
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
/// Remote control lab light Direct Method 'LightControl' true or false
/// </summary>
static LP_DIRECT_METHOD_RESPONSE_CODE LightControlDirectMethodHandler(JSON_Value* json, LP_DIRECT_METHOD_BINDING* directMethodBinding, char** responseMsg)
{
    if (json_value_get_type(json) != JSONBoolean) { return LP_METHOD_FAILED; }

    bool state = (bool)json_value_get_boolean(json);

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
    int sampleRate = *(int*)deviceTwinBinding->twinState;

    if (sampleRate > 0 && sampleRate < (5 * 60)) // check sensible range
    {
        lp_timerChange(&readSensorTimer, &(struct timespec){sampleRate, 0});
        lp_deviceTwinAckDesiredState(deviceTwinBinding, deviceTwinBinding->twinState, LP_DEVICE_TWIN_COMPLETED);
    }
    else {
        lp_deviceTwinAckDesiredState(deviceTwinBinding, deviceTwinBinding->twinState, LP_DEVICE_TWIN_ERROR);
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

## Network status indicator

```c
static LP_TIMER azureIotConnectionStatusTimer = {
    .period = {5, 0},
    .name = "azureIotConnectionStatusTimer",
    .handler = AzureIotConnectionStatusHandler };
```

```c
static LP_GPIO azureIotConnectedLed = {
    .pin = NETWORK_CONNECTED_LED,
    .direction = LP_OUTPUT,
    .initialState = GPIO_Value_Low,
    .invertPin = true,
    .name = "azureIotConnectedLed" };
```

### Update GPIO and Timer sets**

```c
static LP_TIMER* timerSet[] = { &readSensorTimer, &azureIotConnectionStatusTimer };
static LP_GPIO* gpioSet[] = { &relay, &azureIotConnectedLed };
```

### Add network status handler code**

```c
/// <summary>
/// Check status of connection to Azure IoT
/// </summary>
static void AzureIotConnectionStatusHandler(EventLoopTimer* eventLoopTimer)
{
	static bool toggleConnectionStatusLed = true;

	if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0) {
		lp_terminate(ExitCode_ConsumeEventLoopTimeEvent);
	}
	else {
		if (lp_azureConnect()) {
			lp_gpioStateSet(&azureIotConnectedLed, toggleConnectionStatusLed);
			toggleConnectionStatusLed = !toggleConnectionStatusLed;
		}
		else {
			lp_gpioStateSet(&azureIotConnectedLed, false);
		}
	}
}
```