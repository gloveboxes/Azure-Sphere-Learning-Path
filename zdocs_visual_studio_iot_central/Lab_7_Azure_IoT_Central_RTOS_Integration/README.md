# Lab 7: Connect and control your room environment monitor with Azure IoT

<!-- ![](resources/azure-sphere-iot-central-banner.png) -->

---

|Author|[Dave Glover](https://developer.microsoft.com/en-us/advocates/dave-glover?WT.mc_id=julyot-azd-dglover), Microsoft Cloud Developer Advocate, [@dglover](https://twitter.com/dglover) |
|:----|:---|
|Date| March 2020|

---

## Azure Sphere Learning Path

Each module assumes you have completed the previous module.

[Home](../../README.md)

* Lab 0: [Lab Set Up](../Lab_0_Introduction_and_Lab_Set_Up/README.md)
* Lab 1: [Introduction to Azure Sphere development](../Lab_1_Visual_Studio_and_Azure_Sphere/README.md)
* Lab 2: [Connect a room environment monitor to Azure IoT](../Lab_2_Send_Telemetry_to_Azure_IoT_Central/README.md)
* Lab 3: [Set the room virtual thermostat with Azure IoT Device Twins](../Lab_3_Control_Device_with_Device_Twins/README.md)
* Lab 4: [Remote reboot your Azure Sphere with Azure IoT Direct Methods](../Lab_4_Control_Device_with_Direct_Methods/README.md)
* Lab 5: [Integrate FreeRTOS Real-time room sensors with Azure IoT](../Lab_5_FreeRTOS_and_Inter-Core_Messaging/README.md)
* Lab 6: [Integrate Azure RTOS Real-time room sensors with Azure IoT](../Lab_6_AzureRTOS_and_Inter-Core_Messaging/README.md)
* Lab 7: [Connect and control your room environment monitor with Azure IoT](../Lab_7_Azure_IoT_Central_RTOS_Integration/README.md)
<!-- * Lab 8: [Over-the-air (OTA) Deployment](/zdocs/Lab_8_Over-the-air-deployment/README.md) -->

---

## What you will learn

You will learn how Azure Sphere **Inter-Core** messaging works.

---

## Prerequisites

1. You have completed Lab 4, and the FreeRTOS real-Time application is running the Azure Sphere device.
2. You have completed [Lab 2: Send Telemetry from an Azure Sphere to Azure IoT Central](../Lab_2_Send_Telemetry_to_Azure_IoT_Central/README.md). You will need to **copy** and **paste** the Lab 2 **app_manifest.json** you saved to Notepad to this lab's **app_manifest.json** file.

---

## Introduction to the Learning Path Labs

There are a number of Learning Path libraries that support these labs. These Learning Path C functions are prefixed with **lp_**, typedefs and enums are prefixed with **LP_**. 

The Learning Path libraries are open source and contributions are welcome.

The Learning Path libraries are built from the [Azure Sphere Samples](https://github.com/Azure/azure-sphere-samples) and aim to demonstrate best practices.

The Learning Path libraries are **not** part of the official Azure Sphere libraries or samples.

---

## Tutorial Overview

1. Deploy an **Inter-Core** messaging enabled High-Level application to the Azure Sphere.

---

## Key Concepts

As a reminder, the Azure Sphere has three application cores. The Cortex A7 runs High-Level applications, the two Cortex M4s run Real-Time applications.

In this lab, we are going to deploy a High-Level application that will accept event messages from the Azure RTOS Real-Time application and then will forward them on to Azure IoT Central.

![](resources/azure-sphere-application-architecture.png)

---

## Open Lab 7

### Step 1: Start Visual Studio 2019

![](resources/visual-studio-open-local-folder.png)

### Step 2: Open the lab project

1. Click **Open a local folder**
2. Open the Azure-Sphere lab folder
3. Open the **Lab_7_Azure_IoT_Central_RTOS_Integration** folder
4. Click **Select Folder** button to open the project

### Step 3: Set your developer board configuration

These labs support developer boards from AVNET and Seeed Studio. You need to set the configuration that matches your developer board.

The default developer board configuration is for the AVENT Azure Sphere Starter Kit. If you have this board, there is no additional configuration required.

1. Open CMakeList.txt
2. Add a # at the beginning of the set AVNET line to disable it.
3. Uncomment the **set** command that corresponds to your Azure Sphere developer board.
4. Save the file. This will auto-generate the CMake cache.

	![](resources/cmakelist-set-board-configuration.png)

### Step 4: Review the App Manifest

1. Open the **app_manifest.json** file.
2. Review the contents and note the **AllowedApplicationConnections** property setting that is populated with the Component ID of the Azure RTOS Real-Time application.

```json
{
  "SchemaVersion": 1,
  "Name": "AzureSphereIoTCentral",
  "ComponentId": "25025d2c-66da-4448-bae1-ac26fcdd3627",
  "EntryPoint": "/bin/app",
  "CmdArgs": [ "<Replace with your ID Scope>", "6583cf17-d321-4d72-8283-0b7c5b56442b" ],
  "Capabilities": {
    "Gpio": [
      "$LED2",
      "$NETWORK_CONNECTED_LED",
      "$RELAY"
    ],
    "I2cMaster": [ "$AVNET_MT3620_SK_ISU2_I2C" ],
    "PowerControls": [ "ForceReboot" ],
    "AllowedConnections": [ "global.azure-devices-provisioning.net", "<Replace with your Azure IoT Central URI>" ],
    "DeviceAuthentication": "<Replace with your Azure Sphere Tenant ID>",
    "AllowedApplicationConnections": [ "6583cf17-d321-4d72-8283-0b7c5b56442b" ]
  },
  "ApplicationType": "Default"
}
```

---

## Step 5: Set Azure IoT Central Connection Information

1. Open the **app_manifest.json** file.
2. **DO NOT** replace the contents of the **app_manifest.json** file as you did in previous labs. The GPIO pin allocation is different as pins have been allocated to the Azure RTOS Real-Time application.
3. Using the information you saved to **Notepad** property.
4. Update the **Scope ID** property.
5. Update the **Azure IoT Central URL** property.
6. Update the **DeviceAuthentication** property.

---

## Step 6: Set a Breakpoint in the InterCoreHandler Function

1. Open **main.c**
2. From the **Navigate Functions dropdown**, select the second of the two **InterCoreHandler** items listed. The first is the [function forward declaration](https://en.wikipedia.org/wiki/Forward_declaration), the second is the definition on the function. 
    ![](resources/visual-studio-function-navigate.png)
3. Set a debugger breakpoint in near the beginning of the InterCoreHandler function.
    ![](resources/visual-studio-breakpoint-inter-core-handler.png)

---

## Visual Studio Deployment Settings

Before building the application with Visual Studio, ensure ARM-Debug and GDB Debugger (HLCore) options are selected.

![](resources/visual-studio-start-config.png)

---

## Build, Deploy and start Debugging

1. To start the build, deploy, and debug process, either click the Visual Studio **Start Selected Item** icon or press <kbd>**F5**</kbd>.

    ![](resources/visual-studio-start-debug.png)

## Expected Behaviour

1. Generate a **Button Pressed** event from the Azure RTOS application running on the Real-Time core.

    * For Azure Sphere devices with the built-in buttons, press **Button A** to generate a **ButtonPressed** event.
    * For the Seeed Studio MT3620 Mini Dev Board, a **ButtonPressed** event is automatically generated every 10 seconds.

2. Visual Studio will halt execute in the **InterCoreHandler** at the breakpoint.
3. You can now step through the code by pressing <kbd>**F10**</kbd> to step over, <kbd>**F11**</kbd> to step into, or <kbd>**F5**</kbd> to continue.
4. Delete all Breakpoints. From the Visual Studio main menu, select **Debug**, then **Delete All Breakpoints**.
    ![](resources/visual-studio-breakpoints-delete-all.png)
5. Press <kbd>**F5**</kbd> to continue application execution.

---

## Close Visual Studio

1. Close both instances of Visual Studio

---

## Finished 完了 fertig finito समाप्त terminado

Congratulations you have finished lab 5.

![](resources/finished.jpg)

---

[Home](../../README.md)

---
