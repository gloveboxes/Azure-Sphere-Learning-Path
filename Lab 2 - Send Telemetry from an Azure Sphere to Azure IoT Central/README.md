# Lab 2: Send Telemetry from an Azure Sphere to Azure IoT Central

<!-- ![](resources/azure-sphere-iot-central-banner.png) -->

---

|Author|[Dave Glover](https://developer.microsoft.com/en-us/advocates/dave-glover?WT.mc_id=github-blog-dglover), Microsoft Cloud Developer Advocate, [@dglover](https://twitter.com/dglover) |
|:----|:---|
|Source Code | https://github.com/gloveboxes/Azure-Sphere-Learning-Path.git |
|Date| January  2020|

---

## Azure Sphere Learning Path

Each module assumes you have completed the previous module.

[Home](https://gloveboxes.github.io/Azure-Sphere-Learning-Path/)

* Lab 0: [Introduction Azure Sphere and Lab Set Up](https://github.com/gloveboxes/Azure-Sphere-Learning-Path/tree/master/Lab%200%20-%20Introduction%20and%20Lab%20Set%20Up)
* Lab 1: [Build your first Azure Sphere Application with Visual Studio](https://github.com/gloveboxes/Azure-Sphere-Learning-Path/tree/master/Lab%201%20-%20Build%20your%20first%20Azure%20Sphere%20Application%20with%20Visual%20Studio)
* Lab 2: [Send Telemetry from an Azure Sphere to Azure IoT Central](https://github.com/gloveboxes/Azure-Sphere-Learning-Path/tree/master/Lab%202%20-%20Send%20Telemetry%20from%20an%20Azure%20Sphere%20to%20Azure%20IoT%20Central)
* Lab 3: [Control an Azure Sphere with Device Twins and Direct Methods](https://github.com/gloveboxes/Azure-Sphere-Learning-Path/tree/master/Lab%203%20-%20Control%20an%20Azure%20Sphere%20with%20Device%20Twins%20and%20Direct%20Methods)
* Lab 4: [Integrating FreeRTOS with Azure Sphere Inter-Core Messaging](https://github.com/gloveboxes/Azure-Sphere-Learning-Path/tree/master/Lab%204%20-%20Integrating%20FreeRTOS%20with%20Azure%20Sphere%20Inter-Core%20Messaging)
* Lab 5: [Automating Azure Sphere Registration with Azure IoT Central](https://github.com/gloveboxes/Azure-Sphere-Learning-Path/tree/master/Lab%205%20-%20Automating%20Azure%20Sphere%20Registration%20with%20Azure%20IoT%20Central)

---

## What you will learn

You will learn how to build a High Level [Azure Sphere](https://azure.microsoft.com/services/azure-sphere/?WT.mc_id=github-blog-dglover) application that connects and sends telemetry to [Azure IoT Central](https://azure.microsoft.com/services/iot-central/?WT.mc_id=github-blog-dglover).

---

## Prerequisites

The lab assumes you understand the content from [Lab 1: Secure, Build, Deploy and Debug your first Azure Sphere High-Level Application with Visual Studio](https://github.com/gloveboxes/Azure-Sphere-Learning-Path/tree/master/Lab%201%20-%20Build%20your%20first%20Azure%20Sphere%20Application%20with%20Visual%20Studio)

## Tutorial Overview

1. Create an Azure IoT Central Application (Free trial)
2. Connect Azure IoT Central to your Azure Sphere Tenant
3. Deploy an Azure IoT Central enabled application to Azure Sphere

---

## Azure IoT

Your Azure Sphere devices can securely connect and communicate with cloud services. Azure Sphere includes built-in library support for both Azure IoT Hub and Azure IoT Central. This tutorial focuses on Azure IoT Central.

This project leverages the [Azure IoT Hub Device Provisioning Service (PDS)](https://docs.microsoft.com/en-us/azure-sphere/app-development/use-azure-iot) which is part of Azure IoT Central. The Device Provisioning Service (DPS) enables zero-touch, just-in-time, large scale device provisioning.

### What is Azure IoT Central

[Azure IoT Central](https://azure.microsoft.com/en-in/services/iot-central/?WT.mc_id=github-blog-dglover) provides an easy way to connect, monitor and manage your Internet of Things (IoT) assets at scale.

![Azure IoT Central](resources/azure-iot-central.jpg)

---

## Ensure you have cloned the Azure Sphere Learning Path source code

```bash
git clone https://github.com/gloveboxes/Azure-Sphere-Learning-Path.git
```

---

## Getting Started with Azure IoT Central

We are going to create an Azure IoT Central application, connect Azure IoT Central to your Azure Sphere Tenant, and finally create a device.

### Step 1: Create an Azure IoT Central Application

Follow instructions to **[Create an Azure IoT Central Application](resources/azure-iot-central/azure-iot-central.md)**

### Step 2: Connect Azure IoT Central to your Azure Sphere Tenant

Any device that is claimed by your Azure Sphere tenant will be automatically enrolled when it first connects to your Azure IoT Central application.

Follow instructions to **[Set up Azure IoT Central to work with Azure Sphere](https://docs.microsoft.com/en-au/azure-sphere/app-development/setup-iot-central)**.

### Step 3: Create an Azure IoT Central Device

From the **Azure Sphere Developer Command Prompt**, type the following command.

```bash
azsphere device show-attached
```

Note: The Create New Device dialog box in Azure IoT Central requires that the device ID be in lowercase characters. From the Azure Sphere Developer Command Prompt, enter the following command, which gets the ID of the attached device and converts it to lowercase:

```bash
powershell -Command ((azsphere device show-attached)[0] -split ': ')[1].ToLower()
```

#### Switch back to Azure IoT Central Web Portal

1. Select Devices from the side menu, then the Device Template previously created.
![](resources/iot-central-create-device.png)
2. Click **+ New** to add a new device

    Paste in the **device ID** you generated with the PowerShell command into the **Device ID** field and give your device a friendly name.

![](resources/iot-central-create-new-device.png)
3. Click **Create** to create the new device.

---

## Preparing the High-Level Azure Sphere Application

### Understanding the High-Level Core Security

Applications on Azure Sphere are locked down by default and you must grant capabilities to the application.

High-Level Application capabilities include what hardware can be accessed, what internet services can be called (including Azure IoT Central and the Azure Device Provisioning Service), and what inter-core communications are allowed.

The following is an example of an **app_manifest.json** capabilities definition.

```json
{
  "SchemaVersion": 1,
  "Name": "AzureSphereIoTCentral",
  "ComponentId": "25025d2c-66da-4448-bae1-ac26fcdd3627",
  "EntryPoint": "/bin/app",
  "CmdArgs": [ "0ne0999999D", "6583cf17-d321-4d72-8283-0b7c5b56442b" ],
  "Capabilities": {
    "Gpio": [ 0, 19, 21 ]
    "Uart": [ "ISU0" ],
    "AllowedConnections": [ "global.azure-devices-provisioning.net", "saas-iothub-99999999-f33a-4002-a44a-9999991b00b6.azure-devices.net" ],
    "DeviceAuthentication": "99999999-e021-43ce-9f2b-999999997494"
  },
  "ApplicationType": "Default"
}
```

**Review the Capabilities section**:


2. **Gpio**: This application uses three GPIO pins. Pins 0 (a relay switch), 19 (blink send status), and 21 (device twin test LED).
3. **Uart**: The Uart is used to communicate with the Seeed Studio Grove Shield.
4. **AllowedConnections**: What internet URLs can be called.
5. **DeviceAuthentication**: Your Azure Sphere Tenant ID.
6. **AllowedApplicationConnections**: This is the Component ID of the Real-Time application this application will be partnered with. It is required for inter-core communications.
7. **CmdArgs**: The command line argument passed to the application at startup is the Azure IoT Central Scope ID.

---

## Azure IoT Central Configuration

To connect a device to Azure IoT Hub or IoT Central a Device Connection string is required. For **security** and **manageability** do **NOT** hard code the Azure IoT connection string in your High-Level application.

When you created the device in Azure IoT Central you used the **immutable (unforgeable)** Azure Sphere Device ID that was burnt into the device when it was manufactured.

This Device ID along with the following information:

1. The Azure IoT Central Scope ID,
2. The Global URL for the Azure Device Provisioning Service,
3. The URL of your Azure IoT Central Application,
4. Your Azure Sphere Tenant ID

are passed to the Azure Device Provisioning Service from your Azure High-Level application to obtain the Azure IoT Connection string.

### Step 1: Start Visual Studio 2019

![](resources/visual-studio-open-local-folder.png)

### Step 2: Open the lab project

1. Click **Open a local folder**
2. Navigate to the folder you cloned **Azure Sphere Learning Path** into.
3. Open **Lab 2 - Send Telemetry from an Azure Sphere to Azure IoT Central**
4. Select folder  **azure-sphere-hlcore-telemetry-iot-central**
5. Click **Select Folder** button.

    ![](resources/visual-studio-open-lab2.png)

### Step 3: Azure IoT Central Connection Information

1. Open the **app_manifest.json** file
    ![](resources/visual-studio-open-app-manifest.png)

1. Set the **default Azure Sphere Tenant**

    You may need to select the default Azure Sphere Tenant.

    Press <kbd>Windows Key</kbd>, type **Azure Sphere**. Select and start the **Azure Sphere Developer Command Prompt**.
    <br/>
    
    Run the following commands:
    * List available tenants ```azsphere tenant list``` 
    * Set the default tenant ```azsphere tenant select -i <guid>```
    <br/>
2. Get the **Tenant ID**.

    From the **Azure Sphere Developer Command Prompt**, run the following command.

    ```bash
    azsphere tenant show-selected
    ```

    Copy the returned value and paste it into the **DeviceAuthentication** field of the **app_manifest.json**.
    <br/>
3. Get the **Device Provisioning Service URLs**

    From the **Azure Sphere Developer Command Prompt**, navigate to the folder you cloned Azure Sphere Learning Path. Change to the *tools* folder and run the *ShowIoTCentralConfig.exe* command.

    ```bash
    ShowIoTCentralConfig
    ```

    When prompted, log in with the credentials you use for Azure IoT Central. The output of this command will be similar to the following:

    ```text
    Are you using a Work/School account to sign in to your IoT Central Application (Y/N) ?

    Getting your IoT Central applications
    You have one IoT Central application 'yourappname-iot-central'.
    Getting the Device Provisioning Service (DPS) information.
    Getting a list of IoT Central devices.

    Find and modify the following lines in your app_manifest.json:
    "CmdArgs": [ "0ne9992KK6D" ],
    "AllowedConnections": [ "global.azure-devices-provisioning.net", "saas-iothub-9999999-f33a-4002-4444-7ca8989898989.azure-devices.net" ],
    "DeviceAuthentication": "--- YOUR AZURE SPHERE TENANT ID--- ",
    ```
    <br/>
 4.   Find and modify the **CmdArgs**, **AllowedConnections**, and the **DeviceAuthentication** properties in the app_manifest.json with the information returned from the ```ShowIoTCentralConfig``` command.

 Your **manifest_app.json** file will look similar to the following when you are done.

```json
{
    "SchemaVersion": 1,
    "Name": "AzureSphereIoTCentral",
    "ComponentId": "25025d2c-66da-4448-bae1-ac26fcdd3627",
    "EntryPoint": "/bin/app",
    "CmdArgs": [ "0ne0099999D" ],
    "Capabilities": {
        "Gpio": [ 0, 19, 21 ]
        "Uart": [ "ISU0" ],
        "AllowedConnections": [ "global.azure-devices-provisioning.net", "saas-iothub-99999999-f33a-9999-a44a-7c99999900b6.azure-devices.net" ],
        "DeviceAuthentication": "9d7e79eb-9999-43ce-9999-fa8888888894"
    },
    "ApplicationType": "Default"
}
```

---

## Configure Visual Studio App Deployment Settings

Before building the application with Visual Studio ensure ARM-Debug and GDB Debugger (HLCore) options are selected.

![](resources/visual-studio-start-config.png)

### Build, Deploy, and start Debugging

To start the build, deploy, debug process either click the Visual Studio **Start Selected Item** icon or press **F5**. To Build and deploy without attaching the debugger, simply press **Ctrl+F5**.

![](resources/visual-studio-start-debug.png)

---

## Azure IoT Central Integration

Now the application is running on the Azure Sphere switch across to Azure IoT Central, select the **Devices** tab, the device template you created, then the actual device. You may have to wait a moment before the telemetry is displayed in the **Measurements** panel.

![](resources/iot-central-display-measurements.png)

---

## Finished 完了 fertig finito ख़त्म होना terminado

Congratulations you have finished the tutorial.

![](resources/finished.jpg)

---

## Appendix

### Understanding Tutorial Solution

From Visual Studio, open the main.c.

Review the following functions.

1. InitPeripheralsAndHandlers
2. GetTelemetry
3. TwinCallback
4. AzureDirectMethodHandler

#### InitPeripheralsAndHandlers

Sets up SIGTERM termination handler, initialize peripherals, and set up event handlers.

```c
static int InitPeripheralsAndHandlers(void)
{
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = TerminationHandler;
    sigaction(SIGTERM, &action, NULL);

    epollFd = CreateEpollFd();
    if (epollFd < 0) {
        return -1;
    }

    OpenPeripheral(&sending);
    OpenPeripheral(&relay);
    OpenPeripheral(&light);

    // Initialize Grove Shield and Grove Temperature and Humidity Sensor
    GroveShield_Initialize(&i2cFd, 115200);
    sht31 = GroveTempHumiSHT31_Open(i2cFd);

    StartTimer(&iotClientDoWork);
    StartTimer(&iotClientMeasureSensor);

    return 0;
}
```

#### GetTelemetry

Reads sensor telemetry and returns the data as a JSON object.

```c
static int ReadTelemetry(char eventBuffer[], size_t len) {
    GroveTempHumiSHT31_Read(sht31);
    float temperature = GroveTempHumiSHT31_GetTemperature(sht31);
    float humidity = GroveTempHumiSHT31_GetHumidity(sht31);

    static const char* EventMsgTemplate = "{ \"Temperature\": \"%3.2f\", \"Humidity\": \"%3.1f\", \"MsgId\":%d }";
    return snprintf(eventBuffer, len, EventMsgTemplate, temperature, humidity, msgId++);
}
```

#### TwinCallback

Handles [Azure IoT Hub Device Twins](https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-devguide-device-twins?WT.mc_id=github-blog-dglover).

In Azure IoT Central, Azure IoT Hub Device Twins are exposed in the user interface as *Settings*.

![](resources/iot-central-device-settings.png)

```c
static void TwinCallback(DEVICE_TWIN_UPDATE_STATE updateState, const unsigned char* payload,
    size_t payloadSize, void* userContextCallback)
{
    JSON_Value* root_value = NULL;
    JSON_Object* root_object = NULL;

    char* payLoadString = (char*)malloc(payloadSize + 1);
    if (payLoadString == NULL) {
        goto cleanup;
    }

    memcpy(payLoadString, payload, payloadSize);
    payLoadString[payloadSize] = 0; //null terminate string

    root_value = json_parse_string(payLoadString);
    if (root_value == NULL) {
        goto cleanup;
    }

    root_object = json_value_get_object(root_value);
    if (root_object == NULL) {
        goto cleanup;
    }


    JSON_Object* desiredProperties = json_object_dotget_object(root_object, "desired");
    if (desiredProperties == NULL) {
        desiredProperties = root_object;
    }

    SetDesiredState(desiredProperties, &relay);
    SetDesiredState(desiredProperties, &light);

cleanup:
    // Release the allocated memory.
    if (root_value != NULL) {
        json_value_free(root_value);
    }
    free(payLoadString);
}
```

#### AzureDirectMethodHandler

Handles [Azure IoT Hub Direct Methods](https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-devguide-direct-methods?WT.mc_id=github-blog-dglover)

In Azure IoT Central, Azure IoT Hub Direct Methods are exposed in the user interface as *Commands*.

![](resources/iot-central-device-commands.png)

```c
static int AzureDirectMethodHandler(const char* method_name, const unsigned char* payload, size_t payloadSize,
    unsigned char** responsePayload, size_t* responsePayloadSize, void* userContextCallback) {

    const char* onSuccess = "\"Successfully invoke device method\"";
    const char* notFound = "\"No method found\"";

    const char* responseMessage = onSuccess;
    int result = 200;
    JSON_Value* root_value = NULL;
    JSON_Object* root_object = NULL;

    // Prepare the payload for the response. This is a heap allocated null terminated string.
    // The Azure IoT Hub SDK is responsible of freeing it.
    *responsePayload = NULL;  // Response payload content.
    *responsePayloadSize = 0; // Response payload content size.

    char* payLoadString = (char*)malloc(payloadSize + 1);
    if (payLoadString == NULL) {
        responseMessage = "payload memory failed";
        result = 500;
        goto cleanup;
    }

    memcpy(payLoadString, payload, payloadSize);
    payLoadString[payloadSize] = 0; //null terminate string

    root_value = json_parse_string(payLoadString);
    if (root_value == NULL) {
        responseMessage = "Invalid JSON";
        result = 500;
        goto cleanup;
    }

    root_object = json_value_get_object(root_value);
    if (root_object == NULL) {
        responseMessage = "Invalid JSON";
        result = 500;
        goto cleanup;
    }

    if (strcmp(method_name, "fanspeed") == 0)
    {
        int speed = (int)json_object_get_number(root_object, "speed");
        Log_Debug("Set fan speed %d", speed);
    }
    else
    {
        responseMessage = notFound;
        result = 404;
    }

cleanup:

    // Prepare the payload for the response. This is a heap allocated null terminated string.
    // The Azure IoT Hub SDK is responsible of freeing it.
    *responsePayloadSize = strlen(responseMessage);
    *responsePayload = (unsigned char*)malloc(*responsePayloadSize);
    strncpy((char*)(*responsePayload), responseMessage, *responsePayloadSize);

    if (root_value != NULL) {
        json_value_free(root_value);
    }
    free(payLoadString);

    return result;
}
```

### Azure IoT Central Template Settings for Device Twins

![](resources/iot-central-template-settings-relay.png)

![](resources/iot-central-template-settings-light.png)