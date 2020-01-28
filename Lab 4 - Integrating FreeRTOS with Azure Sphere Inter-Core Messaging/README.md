# Lab 4: Integrating FreeRTOS with Azure Sphere Inter-Core Messaging

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

---

## Tutorial Overview

---

## Problems Addressed in this Tutorial

1. Internet enabling an existing FreeRTOS application
2. End to end security for telemetry and device control
3. Streamlining device provisioning

## What you will learn

1. How to run a **FreeRTOS** Real-Time application on Azure Sphere and integrate with Azure IoT.
2. How to create an Azure IoT Central Application.
3. How to integrate an [Azure Sphere](https://azure.microsoft.com/services/azure-sphere/?WT.mc_id=github-blog-dglover) application with [Azure IoT Central](https://azure.microsoft.com/services/iot-central/?WT.mc_id=github-blog-dglover).
4. How to securely control an Azure Sphere with Azure IoT Central Device **[Settings](https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-devguide-device-twins?WT.mc_id=github-blog-dglover)** and **[Commands](https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-devguide-direct-methods?WT.mc_id=github-blog-dglover)**.

If unfamiliar with Azure Sphere development then review the [Create a Secure Azure Sphere App using the Grove Shield Sensor Kit](https://github.com/gloveboxes/Create-a-Secure-Azure-Sphere-App-using-the-Grove-Shield-Sensor-Kit) tutorial before starting this tutorial.

---

## Azure Sphere Solution Architecture

There are **two** applications deployed to the Azure Sphere. 

1. The first application is a **High-Level** *Linux* application running on the **Cortex A7** core. It is responsible for sending temperature and humidity data to Azure IoT Central, processing Digital Twin and Direct Method messages from Azure IoT Central, and finally, passing on **inter-core** messages from the *FreeRTOS* application running on the Real-Time core to Azure IoT Central.
1. The second is a **Real-Time** *FreeRTOS* application running in the **Cortex M4**. It runs several FreeRTOS Tasks. The first task is to blink an LED, the second is to monitor for button presses, and the third is to send **inter-core** messages to the **High-Level** application whenever the button is pressed. **Note**, the FreeRTOS application running on the Real-Time core cannot connect directly to the network.

![](resources/azure-sphere-application-architecture.png)

---

## Azure IoT and Azure Sphere

Your Azure Sphere devices can communicate with the Azure IoT with Azure IoT Hub or Azure IoT Central. This tutorial focuses on Azure IoT Central.

This project also uses the Azure Device Provisioning Service (DPS) included with Azure IoT Central. If you are using Azure IoT Hub then you need to create an instance of [Azure Device Provisioning Service](https://docs.microsoft.com/en-us/azure-sphere/app-development/use-azure-iot).

### What is Azure IoT Central

[Azure IoT Central](https://azure.microsoft.com/en-in/services/iot-central/?WT.mc_id=github-blog-dglover) provides an easy way to connect, monitor and manage your Internet of Things (IoT) assets at scale.

![Azure IoT Central](resources/azure-iot-central.jpg)

---

## Tutorial Overview

1. Set up your development environment.
2. Deploy your first FreeRTOS Application to Azure Sphere.
3. Create an Azure IoT Central Application (Free trial).
4. Connect Azure IoT Central to your Azure Sphere Tenant.
5. Deploy an Azure IoT Central application to Azure Sphere.

---

## Setting up your Development Environment

Follow the [Install for Windows](https://docs.microsoft.com/en-gb/azure-sphere/install/overview) guide, install the Azure Sphere SDK for Visual Studio, claim your device, and configure the Azure Sphere network.

## Clone the following GitHub Repositories

1. The Azure Sphere Samples: ```git clone https://github.com/Azure/azure-sphere-samples.git```

2. This tutorial: ```git clone https://github.com/gloveboxes/Azure-Sphere-IoT-Central-HL-and-RT-Core-Integation.git```
---

## Deploy your first FreeRTOS Application to Azure Sphere

1. Start Visual Studio 2019, select **Open a local folder**, navigate to the Azure Sphere tutorial project folder you cloned from GitHub, then open the  **azure-sphere-rtcore-freertos** project.
2. Set the startup configuration. Select the **ARM-Debug** configuration and the **GDB Debugger (RTCore)** startup item.

    ![](resources/azure-sphere-rtcore-startup-config.png)
3. Press <kbd>**F5**</kbd>, this will start the build, deploy, attach debugger process. The leftmost **blue LED** on the Azure Sphere will start **blinking**.
5. Press **Button A** on the Azure Sphere to change the blink rate. 
6. You can **Remote Debug** the FreeRTOS application running on Azure Sphere Cortex M4 Core. 
    1. From Visual Studio, open the FreeRTOS application **main.c** file.
    2. Set a [Visual Studio Breakpoint](https://docs.microsoft.com/en-us/visualstudio/debugger/using-breakpoints?view=vs-2019) in the **ButtonTask** function on the line that reads ```bool pressed = !newState;```.
    3. Press **Button A** on the Azure Sphere, Visual Studio will halt the execution of the FreeRTOS application and you can step through the code. Pretty darn neat!

### Understanding the Real-Time Core Security

Applications on the Azure Sphere are locked down by default. You need to grant capabilities to the application.

From Visual Studio open the **app_manifest.json** file.

**Observe**:

1. GPIO Capabilities: This application uses two GPIO pins. Pins 10, and 12.
2. Allowed Application Connections: This is the ID of the High-Level application that this application will be partnered with. It is required for inter-core communications.

```json
{
  "SchemaVersion": 1,
  "Name": "GPIO_RTApp_MT3620_BareMetal",
  "ComponentId": "6583cf17-d321-4d72-8283-0b7c5b56442b",
  "EntryPoint": "/bin/app",
  "CmdArgs": [],
  "Capabilities": {
    "Gpio": [ 10, 12 ],
    "AllowedApplicationConnections": [ "25025d2c-66da-4448-bae1-ac26fcdd3627" ]
  },
  "ApplicationType": "RealTimeCapable"
}

```

### Declaring the Partner Application

In the **launch.js.json** file, you need to declare the ID of the High-Level Application that this Real-Time application will be communicating with.

```json
{
    ...
    "configurations": [
        ...
        "partnerComponents": [ "25025d2c-66da-4448-bae1-ac26fcdd3627" ]
    ]
}
```

---

## Getting Started with Azure IoT Central

We are going to create an Azure IoT Central application, connect Azure IoT Central to an Azure Sphere Tenant, and finally create a device.

### Step 1: Create an Azure IoT Central Application

Follow instructions to **[Create an Azure IoT Central Application](resources/azure-iot-central/azure-iot-central.md)**

### Step 2: Connect Azure IoT Central to your Azure Sphere Tenant

Any device that is claimed by your Azure Sphere tenant will be automatically enrolled when it first connects to your Azure IoT Central application.

Follow instructions to **[Set up Azure IoT Central to work with Azure Sphere](https://docs.microsoft.com/en-au/azure-sphere/app-development/setup-iot-central)**.

### Step 3: Create an Azure Sphere Device in Azure IoT Central

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

## Deploy your first High-Level Azure Sphere Application

### Understanding the High-Level Core Security

Applications on Azure Sphere are locked down by default and you must grant capabilities to the application.

High-Level Application capabilities include what hardware can be accessed, what internet services can be called (including Azure IoT Central and the Azure Device Provisioning Service), and what inter-core communications are allowed.

From Visual Studio open the **app_manifest.json** file.

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
    "DeviceAuthentication": "99999999-e021-43ce-9f2b-999999997494",
    "AllowedApplicationConnections": [ "6583cf17-d321-4d72-8283-0b7c5b56442b" ]
  },
  "ApplicationType": "Default"
}
```

**Observe**:

1. **ComponentId**: A [UUID](https://en.wikipedia.org/wiki/Universally_unique_identifier#Format) to identify the application. This Component ID is referenced in the Real-Time application app_manifest to enable inter-core communications with this High-Level application.
2. **Gpio**: This application uses three GPIO pins. Pins 0 (a relay switch), 19 (blink send status), and 21 (device twin test LED).
3. **Uart**: The Uart is used to communicate with the Seeed Studio Grove Shield.
4. **AllowedConnections**: What internet URLs can be called.
5. **DeviceAuthentication**: Your Azure Sphere Tenant ID.
6. **AllowedApplicationConnections**: This is the Component ID of the Real-Time application this application will be partnered with. It is required for inter-core communications.
7. **CmdArgs**: The first argument is the Azure IoT Central Scope ID. The second argument is the Component ID of the Real-Time application, passed to the application at startup as a convenience to keep all configuration information in one file.

### Azure IoT Central Configuration

To connect a device to Azure IoT Hub or IoT Central a Device Connection string is required. For **security** and **manageability** do **NOT** hard code the Azure IoT connection string in your High-Level application.

When you created the device in Azure IoT Central you used the **immutable (unforgeable)** Azure Sphere device ID.

This device ID along with the following information:

1. The Azure IoT Central Scope ID,
2. The Global URL for the Azure Device Provision Service,
3. The URL of your Azure IoT Central Application.

are passed to the Azure Device Provisioning Service (part of Azure IoT Central) from your Azure High-Level application to obtain the Azure IoT Connection string.

### Step 1: Open the High-Level Application with Visual Studio 2019

1. Start Visual Studio 2019, select **Open a local folder**, navigate to the Azure Sphere tutorial project folder you cloned from GitHub, then open the **azure-sphere-hlcore-iot-central** project.
2. Open the **app_manifest.json** file

### Step 2: Azure IoT Central Connection Information

1. Set the **default Azure Sphere Tenant**

    You may need to select the default Azure Sphere Tenant.

    From the **Azure Sphere Developer Command Prompt**, run the ```azsphere tenant list``` command to list available tenants, use the ```azsphere tenant select -i <guid>``` to select the default tenant.

2. Get the **Tenant ID**.

    From the **Azure Sphere Developer Command Prompt**, run the following command.

    ```bash
    azsphere tenant show-selected
    ```

    Copy the returned value and paste it into the **DeviceAuthentication** field of the **app_manifest.json**.

3. Get the **Device Provisioning Service URLs**

    From the **Azure Sphere Developer Command Prompt**, change to the *azure-sphere-samples\Samples\AzureIoT\Tools* folder you previously cloned, and run *ShowIoTCentralConfig.exe*.

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

 4.   Find and modify the following lines in your app_manifest.json with the information returned from the ```ShowIoTCentralConfig``` command:

        * "CmdArgs": [ "0ne9992KK6D" ],
        * "AllowedConnections": [ global.azure-devices-provisioning.net", "saas-iothub-9999999-f33a-4002-4444-7ca8989898989.azure-devices.net" ]


```json
{
    "SchemaVersion": 1,
    "Name": "AzureSphereIoTCentral",
    "ComponentId": "25025d2c-66da-4448-bae1-ac26fcdd3627",
    "EntryPoint": "/bin/app",
    "CmdArgs": [ "<YOUR AZURE IOT CENTRAL SCOPE ID>", "6583cf17-d321-4d72-8283-0b7c5b56442b" ],
    "Capabilities": {
        "Gpio": [ 0, 19, 21 ]
        "Uart": [ "ISU0" ],
        "AllowedConnections": [ "global.azure-devices-provisioning.net", "<YOUR AZURE IOT CENTRAL URL>" ],
        "DeviceAuthentication": "<YOUR AZURE SPHERE TENANT ID>",
        "AllowedApplicationConnections": [ "6583cf17-d321-4d72-8283-0b7c5b56442b" ]
    },
    "ApplicationType": "Default"
}
```

### Step 3: Configure Inter-Core Communications

The Azure Sphere High-Level Application requires **inter-core** communications and this configured.

For this sample this has already been set to the Component ID of the Real-Time core application.

### Step 4:  Visual Studio App Deployment Settings

Before building the application with Visual Studio ensure ARM-Debug and GDB Debugger (HLCore) options are selected.

![](resources/visual-studio-start-config.png)

### Step 5: Build, Deploy and start Debugging

To start the build, deploy and debug process either click the Visual Studio **Start Selected Item** icon or press <kbd>**F5**</kbd>. To Build and deploy without attaching the debugger, press <kbd>**Ctrl+F5**</kbd>.

![](resources/visual-studio-start-debug.png)

---

## Test the Solution

Now you have both the FreeRTOS and the High-Level applications running there are things to test:

1. Press **Button A** on the Azure Sphere. The FreeRTOS application will change the blink rate of the leftmost blue LED on the Azure Sphere and will send an inter-core message to the High-Level application which in turn will toggled the relay pin.

2. Observer the **3rd LED from the left** on the Azure Sphere. It should blink green every 10 seconds as telemetry is sent to Azure IoT Central.

3. The **Visual Studio debugger** should still be connected to the High-Level application. Set a [Visual Studio Breakpoint](https://docs.microsoft.com/en-us/visualstudio/debugger/using-breakpoints?view=vs-2019) in the **InterCoreHandler** function in the **main.c** file. Press **Button A** on the Azure Sphere, the debugger will stop code execution at the breakpoint. You can now step through the code as well as display/change variables.

---

## Azure IoT Central Integration

Now the application is running on the Azure Sphere switch across to Azure IoT Central, select the **Devices** tab, the device template you created, then the actual device. You may have to wait a moment before the telemetry is displayed in the **Measurements** panel.

![](resources/iot-central-display-measurements.png)

### Azure IoT Central Settings

To work with settings you will need to update the Device Template and add settings for **Light** and **Relay**. See the Appendix.

Switch to the settings tab on Azure IoT Central and change the toggle state and click update and observe an LED on the Azure Sphere will toggle between on and off.

![iot central device settings](resources/iot-central-display-settings.png)

---

## Finished 完了 fertig finito ख़त्म होना terminado

Congratulations you have finished the tutorial.

![](resources/finished.jpg)

---

## Appendix

### Learn about Azure Sphere

1. [Azure Sphere Documentation](https://docs.microsoft.com/en-au/azure-sphere/)
1. Using Yocto to Build an IoT OS Targeting a Crossover SoC. [Video](https://www.youtube.com/watch?v=-T7Et5qfqQQ), and [Slides](https://static.sched.com/hosted_files/ossna19/91/Crossover_ELC2019.pdf)
2. [Anatomy of a secured MCU](https://azure.microsoft.com/en-au/blog/anatomy-of-a-secured-mcu/)
3. [Azure Sphere’s customized Linux-based OS](https://azure.microsoft.com/en-au/blog/azure-sphere-s-customized-linux-based-os/)
4. [Tech Communities Blog](https://techcommunity.microsoft.com/t5/internet-of-things/bg-p/IoTBlog)
5. The [Azure IoT Central Sample](https://github.com/Azure/azure-sphere-samples/blob/master/Samples/AzureIoT/IoTCentral.md)

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