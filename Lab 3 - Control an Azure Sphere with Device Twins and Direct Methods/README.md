# Control an Azure Sphere from Azure IoT Central

<!-- ![](resources/azure-sphere-iot-central-banner.png) -->

---

Follow me on Twitter [@dglover](https://twitter.com/dglover)

|Author|[Dave Glover](https://developer.microsoft.com/en-us/advocates/dave-glover?WT.mc_id=github-blog-dglover), Microsoft Cloud Developer Advocate |
|:----|:---|
|Cloud Services | Azure IoT Central |
|Hardware | [Seeed Studio Azure Sphere MT3620](https://www.seeedstudio.com/Azure-Sphere-MT3620-Development-Kit-US-Version-p-3052.html), [Seeed Studio Grove Shield](https://www.seeedstudio.com/MT3620-Grove-Shield.html), and the [Grove Temperature and Humidity Sensor (SHT31)](https://www.seeedstudio.com/Grove-Temperature-Humidity-Sensor-SHT31.html) |
|Source Code | https://github.com/gloveboxes/Azure-Sphere-Learning-Path.git |
|Language| C|
|Date| January  2020|

---

## Azure Sphere Learning Path

Each module assumes you have completed the previous module.

* Lab 0: [Introduction Azure Sphere and Lab Set Up](https://github.com/gloveboxes/Azure-Sphere-Learning-Path/tree/master/Lab%200%20-%20Introduction%20and%20Lab%20Set%20Up)
* Lab 1: [Build your first Azure Sphere Application with Visual Studio](https://github.com/gloveboxes/Azure-Sphere-Learning-Path/tree/master/Lab%201%20-%20Build%20your%20first%20Azure%20Sphere%20Application%20with%20Visual%20Studio)
* Lab 2: [Send Telemetry from an Azure Sphere to Azure IoT Central](https://github.com/gloveboxes/Azure-Sphere-Learning-Path/tree/master/Lab%202%20-%20Send%20Telemetry%20from%20an%20Azure%20Sphere%20to%20Azure%20IoT%20Central)
* Lab 3: [Control an Azure Sphere with Device Twins and Direct Methods](https://github.com/gloveboxes/Azure-Sphere-Learning-Path/tree/master/Lab%203%20-%20Control%20an%20Azure%20Sphere%20with%20Device%20Twins%20and%20Direct%20Methods)
* Lab 4: [Azure Sphere Inter-Core Communications](https://github.com/gloveboxes/Azure-Sphere-Learning-Path/tree/master/Lab%204%20-%20Integrating%20FreeRTOS%20with%20Azure%20Sphere%20Inter-Core%20Communications)
* Lab 5: [Automating Azure Sphere Registration with Azure IoT Central](https://github.com/gloveboxes/Azure-Sphere-Learning-Path/tree/master/Lab%205%20-%20Automating%20Azure%20Sphere%20Registration%20with%20Azure%20IoT%20Central)

---

## What you will learn

You will learn how to control an [Azure Sphere](https://azure.microsoft.com/services/azure-sphere/?WT.mc_id=github-blog-dglover) application using [Azure IoT Central](https://azure.microsoft.com/services/iot-central/?WT.mc_id=github-blog-dglover) **Settings** ([Hub Device Twins](https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-devguide-device-twins?WT.mc_id=github-blog-dglover)) and **Commands** ([Direct Methods](https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-devguide-direct-methods?WT.mc_id=github-blog-dglover)).

---



---

## Tutorial Overview

---

## Clone the following GitHub Repositories

1. The Azure Sphere Samples: ```git clone https://github.com/Azure/azure-sphere-samples.git```

2. This tutorial: ```git clone https://github.com/gloveboxes/Azure-Sphere-IoT-Central-HL-and-RT-Core-Integation.git```
---

## Controlling an Azure Sphere from Azure IoT Central

Now that we have created the Azure IoT Central application we are going to add support for controlling the Azure Sphere device from Azure IoT Central, along with adding *event* support.

There are two ways to control or set the state on the Azure Sphere device from IoT Central.

1. The first way to control a device is with Azure IoT Central **Settings**. Under the covers, these are implemented as [Azure IoT Device Twins](https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-devguide-device-twins). A message is sent to the device, the corresponding action is undertaken, and then the Azure Sphere sends a message to update the Azure IoT Device Twin. The Device Twin is a JSON document that mirrors the state of the device. The JSON document can be used for cloud side queries and processing.

2. The second way to control a device is with Azure IoT Central **Commands**. Under the covers, these are implemented as [Azure IoT Direct Methods](https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-devguide-direct-methods). A message is sent to the device, the action carried out, and then the device acknowledges to Azure IoT the command has completed.

---

## Adding Azure IoT Central Settings and Commands

To work with settings you will need to update the Device Template and add settings for **Light** and **Relay**. See the Appendix.


### Step 1: Open Azure IoT Central

### Step 2: Open Template

add settings and commands



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

---

## Visual Studio App Deployment Settings

Before building the application with Visual Studio ensure ARM-Debug and GDB Debugger (HLCore) options are selected.

![](resources/visual-studio-start-config.png)

---

## Build, Deploy and start Debugging

To start the build, deploy and debug process either click the Visual Studio **Start Selected Item** icon or press <kbd>**F5**</kbd>. To Build and deploy without attaching the debugger, press <kbd>**Ctrl+F5**</kbd>.

![](resources/visual-studio-start-debug.png)

---

## Test the Solution

Now the application is running on the Azure Sphere switch across to Azure IoT Central, select the **Devices** tab, the device template you created, then the actual device.

Select to the **Settings** tab and change the **Relay** toggle state and click **Update** and observe an LED on the Azure Sphere will toggle between on and off.

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

<!-- #### TwinCallback

Handles [Azure IoT Hub Device Twins](https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-devguide-device-twins?WT.mc_id=github-blog-dglover).

In Azure IoT Central, Azure IoT Hub Device Twins are exposed in the user interface as *Settings*.

![](resources/iot-central-device-settings.png)



#### AzureDirectMethodHandler

Handles [Azure IoT Hub Direct Methods](https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-devguide-direct-methods?WT.mc_id=github-blog-dglover)

In Azure IoT Central, Azure IoT Hub Direct Methods are exposed in the user interface as *Commands*.

![](resources/iot-central-device-commands.png)


### Azure IoT Central Template Settings for Device Twins

![](resources/iot-central-template-settings-relay.png)

![](resources/iot-central-template-settings-light.png) -->