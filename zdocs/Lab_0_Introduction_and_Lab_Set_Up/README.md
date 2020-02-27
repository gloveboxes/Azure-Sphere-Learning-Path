# Lab 0: Introduction to Azure Sphere and Lab set up

<!-- ![Azure Sphere with shield](resources/azure-sphere-shield.png) -->

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

[Next](/zdocs/Lab_2_Send_Telemetry_to_Azure_IoT_Central/README.md)

---

## What you will learn

---

## Tutorial Overview

---

## Set up your Development Environment

### STEP 1: Install Developer Tools

This tutorial assumes Windows 10 and [Visual Studio (The free Community Edition or better)](https://visualstudio.microsoft.com/vs/?WT.mc_id=github-blog-dglover). For now, Azure Sphere templates are only available for Visual Studio. However, you can clone and open this solution on Windows and Ubuntu 18.04 with [Visual Studio Code](https://code.visualstudio.com/?WT.mc_id=github-blog-dglover).

### STEP 2: Clone Azure Sphere Learning Path

```bash
git clone https://github.com/gloveboxes/Azure-Sphere-Learning-Path.git
```

### STEP 3: Set up Azure Sphere SDK, Claim the Azure Sphere, and Configure Networking

This tutorial targets the [Azure Sphere SDK 20.01](https://docs.microsoft.com/en-us/azure-sphere/resources/release-notes-2001?WT.mc_id=github-blog-dglover) or better which uses the CMake Build System.

Follow the Azure Sphere [Overview of set up procedures](https://docs.microsoft.com/en-au/azure-sphere/install/overview?WT.mc_id=github-blog-dglover) guide.

---

## Appendix

---

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

---

### Azure Sphere Grove Kit

| Azure Sphere   |  Image  |
| ---- | ---- |
| [Azure Sphere MT3620 Development Kit](https://www.seeedstudio.com/Azure-Sphere-MT3620-Development-Kit-US-Version-p-3052.html)|
| [Azure Sphere MT3620 Development Kit Shield](https://www.seeedstudio.com/Grove-Starter-Kit-for-Azure-Sphere-MT3620-Development-Kit.html). <br/> Note, you can also purchase the parts separately. | ![](resources/seeed-studio-grove-shield-and-sensors.jpg) |

---

### Azure Sphere MT3620 Developer Board Pinmap

The full Azure Sphere MT3620 Board Pinmap can be found on the [Azure Sphere MT3620 Development Kit](https://www.seeedstudio.com/Azure-Sphere-MT3620-Development-Kit-US-Version-p-3052.html) page.

![](resources/mt3620-dev-board-pinmap.png)
