# Lab 5: Integrating FreeRTOS with Azure IoT Central

<!-- ![](resources/azure-sphere-iot-central-banner.png) -->

---

|Author|[Dave Glover](https://developer.microsoft.com/en-us/advocates/dave-glover?WT.mc_id=github-blog-dglover), Microsoft Cloud Developer Advocate, [@dglover](https://twitter.com/dglover) |
|:----|:---|
|Source Code | https://github.com/gloveboxes/Azure-Sphere-Learning-Path.git |
|Date| January  2020|

---

## Azure Sphere Learning Path

Each module assumes you have completed the previous module.

[Home](https://github.com/gloveboxes/Azure-Sphere-Learning-Path/blob/master/README.md)

* Lab 0: [Introduction Azure Sphere and Lab Set Up](/zdocs/Lab_0_Introduction_and_Lab_Set_Up/README.md)
* Lab 1: [Build your first Azure Sphere Application with Visual Studio](/zdocs/Lab_1_Visual_Studio_and_Azure_Sphere/README.md)
* Lab 2: [Send Telemetry from an Azure Sphere to Azure IoT Central](/zdocs/Lab_2_Send_Telemetry_to_Azure_IoT_Central/README.md)
* Lab 3: [Control an Azure Sphere with Device Twins and Direct Methods](/zdocs/Lab_3_Control_Device_with_Device_Twins_and_Direct_Methods/README.md)
* Lab 4: [Integrating FreeRTOS with Azure Sphere Inter-Core Messaging](/zdocs/Lab_4_FreeRTOS_and_Inter-Core_Messaging/README.md)
* Lab 5: [Integrating FreeRTOS with Azure IoT Central](/zdocs/Lab_5_FreeRTOS_and_Azure_IoT_Central/README.md)
* Lab 6: [Mass Device Deployment with Azure IoT Central](/zdocs/Lab_6_Mass_Device_Deployment/README.md)

---

## Prerequisites

This lab assumes you have completed [Lab 2: Send Telemetry from an Azure Sphere to Azure IoT Central](https://github.com/gloveboxes/Azure-Sphere-Learning-Path/tree/master/Lab%202%20-%20Send%20Telemetry%20from%20an%20Azure%20Sphere%20to%20Azure%20IoT%20Central). You will have created an Azure IoT Central application, connected Azure IoT Central to your Azure Sphere Tenant and you have configured the **app_manifest.json** for the Azure Device Provisioning Service.

You will need to **copy** and **paste** the Lab 2 **app_manifest.json** you created to this labs **app_manifest.json** file.

---

## Problems Addressed in this Tutorial

1. Internet enabling an existing FreeRTOS application
2. End to end security for telemetry and device control
3. Streamlining device provisioning

## What you will learn

1. How to integrate an [Azure Sphere](https://azure.microsoft.com/services/azure-sphere/?WT.mc_id=github-blog-dglover) application with [Azure IoT Central](https://azure.microsoft.com/services/iot-central/?WT.mc_id=github-blog-dglover).
2. How to securely control an Azure Sphere with Azure IoT Central Device **[Settings](https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-devguide-device-twins?WT.mc_id=github-blog-dglover)** and **[Commands](https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-devguide-direct-methods?WT.mc_id=github-blog-dglover)**.

If unfamiliar with Azure Sphere development then review the [Create a Secure Azure Sphere App using the Grove Shield Sensor Kit](https://github.com/gloveboxes/Create-a-Secure-Azure-Sphere-App-using-the-Grove-Shield-Sensor-Kit) tutorial before starting this tutorial.

---

## Azure Sphere Solution Architecture

There are **two** applications deployed to the Azure Sphere.

1. The first application is a **High-Level** *Linux* application running on the **Cortex A7** core. It is responsible for sending temperature and humidity data to Azure IoT Central, processing Digital Twin and Direct Method messages from Azure IoT Central, and finally, passing on **inter-core** messages from the *FreeRTOS* application running on the Real-Time core to Azure IoT Central.
1. The second is a **Real-Time** *FreeRTOS* application running in the **Cortex M4**. It runs several FreeRTOS Tasks. The first task is to blink an LED, the second is to monitor for button presses, and the third is to send **inter-core** messages to the **High-Level** application whenever the button is pressed. **Note**, the FreeRTOS application running on the Real-Time core cannot connect directly to the network.

![](resources/azure-sphere-application-architecture.png)

---

## Tutorial Overview

1. Set up your development environment.
2. Deploy your first FreeRTOS Application to Azure Sphere.
3. Create an Azure IoT Central Application (Free trial).
4. Connect Azure IoT Central to your Azure Sphere Tenant.
5. Deploy an Azure IoT Central application to Azure Sphere.

---

## Ensure you have cloned the Azure Sphere Learning Path source code

```bash
git clone https://github.com/gloveboxes/Azure-Sphere-Learning-Path.git
```

---

## Step 1: Start Visual Studio 2019

![](resources/visual-studio-open-local-folder.png)

## Step 2: Open the lab project

1. Click **Open a local folder**
2. Navigate to the folder you cloned **Azure Sphere Learning Path** into.
3. Double click to open the **Lab_5_FreeRTOS_and_Azure_IoT_Central** folder
4. Click **Select Folder** button to open the project

---

## Integrate FreeRTOS Real Time Core app with Azure IoT Central

//TODO: Docs to be expanded

1. Configure Azure IoT Central
2. Set Debug Option
3. Deploy the app
4. Set a break point in Inter-Core Handler

<!-- 
1. Set the startup configuration. Select the **ARM-Debug** configuration and the **GDB Debugger (RTCore)** startup item.

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
    "AllowedApplicationConnections": [ "25025d2c-66da-4 -->