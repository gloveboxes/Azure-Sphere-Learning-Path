# Lab 1: Secure, Build, Deploy and Debug your first High-Level Azure Sphere Application with Visual Studio

<!-- ![Azure Sphere with shield](resources/azure-sphere-shield.png) -->

---

|Author|[Dave Glover](https://developer.microsoft.com/en-us/advocates/dave-glover?WT.mc_id=github-blog-dglover), Microsoft Cloud Developer Advocate, [@dglover](https://twitter.com/dglover) |
|:----|:---|
|Source Code | https://github.com/gloveboxes/Azure-Sphere-Learning-Path.git |
|Date| January  2020|

---

## Azure Sphere Learning Path

Each module assumes you have completed the previous module.

[Home](/README.md)

* Lab 0: [Introduction Azure Sphere and Lab Set Up](/docs/Lab_0_Introduction_and_Lab_Set_Up/README.md)
* Lab 1: [Build your first Azure Sphere Application with Visual Studio](/docs/Lab_1_Visual_Studio_and_Azure_Sphere/README.md)
* Lab 2: [Send Telemetry from an Azure Sphere to Azure IoT Central](/docs/Lab_2_Send_Telemetry_to_Azure_IoT_Central/README.md)
* Lab 3: [Control an Azure Sphere with Device Twins and Direct Methods](/docs/Lab_3_Control_Device_with_Device_Twins_and_Direct_Methods/README.md)
* Lab 4: [Integrating FreeRTOS with Azure Sphere Inter-Core Messaging](/docs/Lab_4_FreeRTOS_and_Inter-Core_Messaging/README.md)
* Lab 5: [Integrating FreeRTOS with Azure IoT Central](/docs/Lab_5_FreeRTOS_and_Azure_IoT_Central/README.md)
* Lab 6: [Mass Device Deployment with Azure IoT Central](/docs/Lab_6_Mass_Device_Deployment/README.md)

[Next](/docs/Lab_2_Send_Telemetry_to_Azure_IoT_Central/README.md)

---

## What you will learn

In this lab you will learn how to secure, build, deploy, and debug your first High-Level (HL) Azure Sphere application onto the Azure Sphere A7 Core.

The Azure Sphere [MT3620](https://www.mediatek.com/products/azureSphere/mt3620) MCU consists of three application cores. One ARM Cortex A7 High-Level Application core running Embedded Linux (built with [Yokto](https://www.yoctoproject.org/)), and two ARM Cortex M4F Real-Time cores for running FreeRTOS, Azure RTOS, or bare metal applications. The MT3620 MCU is also know as a Crossover MCU as it bridges the application world of ARM Cortex A7 with the Real time world of ARM Cortex M4.  

---

## Tutorial Overview

1. Open the lab project with Visual Studio
2. Define the High-Level (A7) Core application security requirements.
3. Application deployment and debugging

---

## Open Lab 1 Project

### Step 1: Ensure you have cloned the lab source code

	```bash
	git clone https://github.com/gloveboxes/Azure-Sphere-Learning-Path.git
	```

### Step 2: Start Visual Studio 2019

![](resources/visual-studio-open-local-folder.png)

### Step 3: Open the lab project

1. Click **Open a local folder**
2. Navigate to the folder you cloned **Azure Sphere Learning Path** into.
3. Double click to open the **Lab_1_Visual_Studio_and_Azure_Sphere** folder
4. Click **Select Folder** button to open the project

![](resources/visual-studio-open-project.png)

### Step 4: Verify Project Opened Correctly

From the **Solution Explorer**, open the **main.c** file.

![](resources/visual-studio-open-main.png)

---
<!-- 
## Create a new Visual Studio Azure Sphere Project

Start Visual Studio and create a new project in the same directory you cloned this tutorial into which includes the MT3620 Grove Shield Library.

It is important to create the Visual Studio Project in the same folder you cloned the MT3620 Grove Shield as there are relative links to this library in the application you will create.

```text
azure-sphere
	|- MT3620_Grove_Shield
	|- YourAzureSphereApplication
```

![](resources/vs-create-new-project.png)

### Select Azure Sphere Project Template

Type **sphere** in the search box and select the Azure Sphere Blink template.

![](resources/vs-select-azure-sphere-blink.png)

### Configure new Azure Sphere Project

Name the project and set the save location.

![](resources/vs-configure-new-project.png) -->

### Open the CMakeLists.txt file

**CMakelists.txt** defines the build process, the code files,  library locations, and more.

Note the references to **MT3620_Grove_Shield_Library**.

1. **add subdirectory** is the location of the MT3620_Grove_Shield_Library source code.
2. **target_link_libraries** includes the MT3620_Grove_Shield_Library into the linking process.

![](resources/vs-configure-cmakelists.png)

---

## Understanding the High-Level Core Security

Applications on Azure Sphere are locked down by default and you must grant capabilities to the application. This is key to Azure Sphere security and is also known as the [Principle of least privilege](https://en.wikipedia.org/wiki/Principle_of_least_privilege).

High-Level Application capabilities include what hardware can be accessed, what internet services can be called (including Azure IoT Central and the Azure Device Provisioning Service), and what inter-core communications are allowed.

1. Review the [Grove Shield Sensor Capabilities Quick Reference](#grove-shield-sensor-capabilities-quick-reference) to understand what capabilities are required for each sensor in the Seeed Studio Grove Shield library.
2. From Visual Studio open the **app_manifest.json** file.

![](resources/visual-studio-application-capabilities.png)

Review the defined capabilities:

```json
"Capabilities": {
    "Gpio": [ 19 ],
    "Uart": [ "ISU0" ],
    "AllowedApplicationConnections": []
  },
```

1. **"Gpio": [ 19 ]**: GPIO 19 is used to control an onboard LED
2. **"Uart": [ "ISU0" ]**: Access to the I2C SHT31 temperature/humidity sensor via the Grove Shield was built before Azure Sphere supported I2C. Hence calls to the sensor are proxied via the Uart.

---

## Review the Code

The following code is found in the **main.c** file.

1. The LED connected to **GPIO 9** is opened
2. The Grove Shield UART port is opened, then the Grove SHT31 Sensor


---

## Deploy the Application to the Azure Sphere

1. Connect the Azure Sphere to your computer via USB
2. Ensure you have [claimed](https://docs.microsoft.com/en-au/azure-sphere/install/claim-device?WT.mc_id=github-blog-dglover), [connected](https://docs.microsoft.com/en-au/azure-sphere/install/configure-wifi?WT.mc_id=github-blog-dglover), and [developer enabled](https://docs.microsoft.com/en-au/azure-sphere/install/qs-blink-application?WT.mc_id=github-blog-dglover) your Azure Sphere.

3. Select **GDB Debugger (HLCore)** from the **Select Startup** dropdown.
	![](resources/vs-start-application.png)
4. From Visual Studio, press **F5** to build, deploy, start, and attached the remote debugger to the Azure Sphere.

---

## View the Debugger Output

Open the _Output_ window to view the output from **Log_Debug** statements in _main.c_.

You can open the output window by using the Visual Studio **Ctrl+Alt+O** shortcut or click the **Output** tab found along the bottom/right of Visual Studio.

![Visual Studio View Output](resources/vs-view-output.png)

---

## Set a Visual Studio Debugger Breakpoint

Set a debugger breakpoint by clicking in the margin to the left of the line of code you want the debugger to stop at.

In the **main.c** file set a breakpoint in the margin of the line that reads the Grove temperature and pressure sensor **GroveTempHumiSHT31_Read(sht31);**.

 ![](resources/vs-set-breakpoint.png)

---

## Stop the Debugger

**Stop** the debugger by using the Visual Studio **Shift+F5** keyboard shortcut or click the **Stop Debugging** icon.

![](resources/vs-stop-debugger.png)

---

## Azure Sphere Application Cloud Deployment

Now you have learnt how to "Side Load" an application onto Azure Sphere it is time to learn how to [Connect and Send Telemetry from an Azure Sphere to Azure IoT Central](https://github.com/gloveboxes/Azure-Sphere-Learning-Path/tree/master/Lab%202%20-%20Send%20Telemetry%20from%20an%20Azure%20Sphere%20to%20Azure%20IoT%20Central).

## Finished 完了 fertig finito ख़त्म होना terminado

Congratulations, you secured, built, deployed and debugged your first Azure Sphere application.

![](resources/finished.jpg)

## Appendix

### Grove Shield Sensor Capabilities Quick Reference

| Sensors  | Socket | Capabilities |
| :------------- | :------------- | :----------- |
| Grove Light Sensor  | Analog | "Gpio": [ 57, 58 ], "Uart": [ "ISU0"] |
| Grove Rotary Sensor | Analog | "Gpio": [ 57, 58 ], "Uart": [ "ISU0"] |
| Grove 4 Digit Display | GPIO0 or GPIO4 | "Gpio": [ 0, 1 ] or "Gpio": [ 4, 5 ] |
| Grove LED Button | GPIO0 or GPIO4 |  "Gpio": [ 0, 1 ] or "Gpio": [ 4, 5 ] |
| Grove Oled Display 96x96 | I2C | "Uart": [ "ISU0"]  |
| Grove Temperature Humidity SHT31 | I2C | "Uart": [ "ISU0"] |
| Grove UART3 | UART3 | "Uart": [ "ISU3"] |
| LED 1 | Red <br/> Green <br/> Blue | "Gpio": [ 8 ] <br/> "Gpio": [ 9 ] <br/> "Gpio": [ 10 ] |
| LED 2 | Red <br/> Green <br/> Blue | "Gpio": [ 15 ] <br/> "Gpio": [ 16 ] <br/> "Gpio": [ 17 ] |
| LED 3 | Red <br/> Green <br/> Blue | "Gpio": [ 18 ] <br/> "Gpio": [ 19 ] <br/> "Gpio": [ 20 ] |
| LED 4 | Red <br/> Green <br/> Blue | "Gpio": [ 21 ] <br/> "Gpio": [ 22 ] <br/> "Gpio": [ 23 ] |

For more pin definitions see the __mt3620_rdb.h__ in the MT3620_Grove_Shield/MT3620_Grove_Shield_Library folder.

### Azure Sphere Grove Kit

| Azure Sphere   |  Image  |
| ---- | ---- |
| [Azure Sphere MT3620 Development Kit](https://www.seeedstudio.com/Azure-Sphere-MT3620-Development-Kit-US-Version-p-3052.html)|
| [Azure Sphere MT3620 Development Kit Shield](https://www.seeedstudio.com/Grove-Starter-Kit-for-Azure-Sphere-MT3620-Development-Kit.html). <br/> Note, you can also purchase the parts separately. | ![](resources/seeed-studio-grove-shield-and-sensors.jpg) |

### Azure Sphere MT3620 Developer Board Pinmap

The full Azure Sphere MT3620 Board Pinmap can be found on the [Azure Sphere MT3620 Development Kit](https://www.seeedstudio.com/Azure-Sphere-MT3620-Development-Kit-US-Version-p-3052.html) page.

![](resources/mt3620-dev-board-pinmap.png)
