# Lab 3: Control an Azure Sphere with Device Twins and Direct Methods

<!-- ![](resources/azure-sphere-iot-central-banner.png) -->

---

|Author|[Dave Glover](https://developer.microsoft.com/en-us/advocates/dave-glover?WT.mc_id=github-blog-dglover), Microsoft Cloud Developer Advocate, [@dglover](https://twitter.com/dglover) |
|:----|:---|
|Source Code | https://github.com/gloveboxes/Azure-Sphere-Learning-Path.git |
|Date| January  2020|

---

## Azure Sphere Learning Path

Each module assumes you have completed the previous module.

[Home](https://gloveboxes.github.io/Azure-Sphere-Learning-Path/#azure-sphere-learning-path)

* Lab 0: [Introduction Azure Sphere and Lab Set Up](/zdocs/Lab_0_Introduction_and_Lab_Set_Up/README.md)
* Lab 1: [Build your first Azure Sphere Application with Visual Studio](/zdocs/Lab_1_Visual_Studio_and_Azure_Sphere/README.md)
* Lab 2: [Send Telemetry from an Azure Sphere to Azure IoT Central](/zdocs/Lab_2_Send_Telemetry_to_Azure_IoT_Central/README.md)
* Lab 3: [Control an Azure Sphere with Device Twins and Direct Methods](/zdocs/Lab_3_Control_Device_with_Device_Twins_and_Direct_Methods/README.md)
* Lab 4: [Integrating FreeRTOS with Azure Sphere Inter-Core Messaging](/zdocs/Lab_4_FreeRTOS_and_Inter-Core_Messaging/README.md)
* Lab 5: [Integrating FreeRTOS with Azure IoT Central](/zdocs/Lab_5_FreeRTOS_and_Azure_IoT_Central/README.md)
* Lab 6: [Mass Device Deployment with Azure IoT Central](/zdocs/Lab_6_Mass_Device_Deployment/README.md)

[Next](/zdocs/Lab_4_FreeRTOS_and_Inter-Core_Messaging/README.md)

---

## What you will learn

You will learn how to control an [Azure Sphere](https://azure.microsoft.com/services/azure-sphere/?WT.mc_id=github-blog-dglover) application using [Azure IoT Central](https://azure.microsoft.com/services/iot-central/?WT.mc_id=github-blog-dglover) **Settings** ([Hub Device Twins](https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-devguide-device-twins?WT.mc_id=github-blog-dglover)) and **Commands** ([Direct Methods](https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-devguide-direct-methods?WT.mc_id=github-blog-dglover)).

---

## Prerequisites

This lab assumes you have completed [Lab 2: Send Telemetry from an Azure Sphere to Azure IoT Central](https://github.com/gloveboxes/Azure-Sphere-Learning-Path/tree/master/Lab%202%20-%20Send%20Telemetry%20from%20an%20Azure%20Sphere%20to%20Azure%20IoT%20Central). You will have created an Azure IoT Central application, connected Azure IoT Central to your Azure Sphere Tenant and you have configured the **app_manifest.json** for the Azure Device Provisioning Service.

You will need to **copy** and **paste** the Lab 2 **app_manifest.json** you created to this labs **app_manifest.json** file.

---

## Tutorial Overview

---

## Clone the following GitHub Repositories

---

## Controlling an Azure Sphere from Azure IoT Central

Now that we have created the Azure IoT Central application we are going to add support for controlling the Azure Sphere device from Azure IoT Central, along with adding *event* support.

There are two ways to control or set the state on the Azure Sphere device from IoT Central.

1. The first way to control a device is with Azure IoT Central **Settings**. Under the covers, these are implemented as [Azure IoT Device Twins](https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-devguide-device-twins). A message is sent to the device, the corresponding action is undertaken, and then the Azure Sphere sends a message to update the Azure IoT Device Twin. The Device Twin is a JSON document that mirrors the state of the device. The JSON document can be used for cloud side queries and processing.

2. The second way to control a device is with Azure IoT Central **Commands**. Under the covers, these are implemented as [Azure IoT Direct Methods](https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-devguide-direct-methods). A message is sent to the device, the action carried out, and then the device acknowledges to Azure IoT the command has completed.

---

## Open Lab 3

### Step 1: Start Visual Studio 2019

![](resources/visual-studio-open-local-folder.png)

### Step 2: Open the lab project

1. Click **Open a local folder**
2. Navigate to the folder you cloned **Azure Sphere Learning Path** into.
3. Double click to open the **Lab_3_Control_Device_with_Device_Twins_and_Direct_Methods** folder
4. Click **Select Folder** button to open the project

    <!-- ![](resources/visual-studio-open-lab3.png) -->

### Step 3: Configure the Azure IoT Central Connection Information

#### Lab Azure Sphere

If you are using an Azure Sphere in a lab environment and did not claim the Azure Sphere into your own Azure Sphere tenant then set the connection string as per the instructions in Lab 2.

#### Own Azure Sphere

If you claimed your Azure Sphere device into your own Azure Sphere tenant then set the **app_manifest.json** configuration as per Lab 2.

1. Open the **app_manifest.json** file

    ![](resources/visual-studio-open-app-manifest.png)

2. Replace the contents of this **app_manifest.json** file with the **app_manifest.json** you created in **Lab 2**.

---

## Implementing Azure IoT Central Settings and Commands

Now that Azure IoT Central **Settings** and **Commands** have been configured we need to connect those actions with code in the application.

To simplify what can easily become repetitive code there is a mini **Peripherals Framework** for connecting code running on the Azure Sphere with Azure IoT Central **Settings (Azure IoT Device Twins)**, and **Commands (Azure IoT Direct Methods)**.

There are two data structures that simplify the implementation of Device Twins and Direct Methods.

These are:

1. DeviceTwinPeripheral, and
2. DirectMethodPeripheral

### Setting up support for Azure IoT Central Settings

1. Open the **main.c** file
2. Scroll down to the the line that reads **static DeviceTwinPeripheral relay**
3. This data structure describes a generalized peripheral and what Azure IoT Central Device **Setting** this peripheral is associated with.  Azure IoT Central device settings are implemented as Azure IoT Device Twins.

    ```c
    static DeviceTwinPeripheral relay = {
        .peripheral = {
            .fd = -1, 
            .pin = RELAY_PIN, 
            .initialState = GPIO_Value_Low, 
            .invertPin = false, 
            .initialise = OpenPeripheral, 
            .name = "relay1" },
        .twinState = false,
        .twinProperty = "relay1",
        .handler = DeviceTwinHandler
    };
    ```

### Setting up support for Azure IoT Central Commands

1. Again in the **main.c** file
2. Scroll down to the the line that reads **static DirectMethodPeripheral fan**
3. This data structure describes a generalized peripheral and what Azure IoT Central Device **Command** this peripheral is associated with.  Azure IoT Central device commands are implemented as Azure IoT Direct Methods.

    ```c
    static DirectMethodPeripheral fan = {
        .peripheral = {
            .fd = -1, 
            .pin = FAN_PIN, 
            .initialState = GPIO_Value_Low, 
            .invertPin = false, 
            .initialise = InitFanPWM, 
            .name = "fan1" },
        .methodName = "fan1",
        .handler = SetFanSpeedDirectMethod
    };
    ```

### Initializing the support for IoT Central Settings and Commands

1. Again in **main.c**.
2. Scroll down to the line that reads **#pragma region define sets for auto initialisation and close**
3. In this region there are a number of C arrays that point to the **DeviceTwinPeripherals** and **DirectMethodPeripherals** defined above.
    ```c
    DeviceTwinPeripheral* deviceTwinDevices[] = { &relay, &light };
    DirectMethodPeripheral* directMethodDevices[] = { &fan };
    ```
4. In the main.c **InitPeripheralsAndHandlers** these sets (or collections) of device twins and direct methods are initialized. 
5. The goal of the peripherals framework is to make it easy to connect code running on the Azure Sphere with Azure IoT Central **Settings** and **Commands**.

<!-- ---

## Extension Exercise

Try connecting a different LED on the Azure Sphere with an Azure IoT Device Setting.

You will need to do the following steps:

1. Update the Azure IoT Central Device Template and add a new settings control.
2. Define a new **DeviceTwinPeripherals** data structure for the LED.
3. Add the new LED data structure to the **deviceTwinDevices** array. This will ensure the GPIO pin is initialized and associated with a Azure IoT Central Device Setting.
4. Update the **app_manifest.json** with the GPIO pin you want to use from your Azure Sphere application. -->

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

Select to the **Properties** tab and change the **Light** toggle state to **On**, then click **Update**. Observe that a **Red** LED lights up on the Azure Sphere developer kit.

![iot central device settings](resources/iot-central-display-settings.png)

---

## Finished 完了 fertig finito ख़त्म होना terminado

Congratulations you have finished lab 3.

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