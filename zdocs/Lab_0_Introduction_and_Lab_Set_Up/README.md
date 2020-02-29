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

## Troubleshooting

1. If you have trouble connecting to the Azure Sphere over USB be sure to disable any VPNs you might have enabled.

---

## Tutorial Overview

---

## Azure Sphere Tenant Credentials

### Personal Azure Sphere Device

If you have your own Azure Sphere device you will need the credentials you used when you claimed your device into an Azure Sphere Tenant.

### Shared Lab Azure Sphere Device

If you are using a shared lab device, then the lab instructor will tell you the Azure Sphere Tenant credentials you will need for the lab.

---

## STEP 1: Install the Developer Tools

These labs are supported on Windows 10, Ubuntu 18.04, and on macOS running the Azure Development experience in a Virtual Box Ubuntu 18.04 Virtual Machine.

On Windows 10, [Visual Studio (The free Community Edition or better)](https://visualstudio.microsoft.com/vs/?WT.mc_id=github-blog-dglover) is recommended

On Ubuntu 18.04, install the latest version of [Visual Studio Code](https://code.visualstudio.com/) and the [Azure Sphere Extension](https://marketplace.visualstudio.com/items?itemName=ms-vscode.azure-sphere-tools).

---

## STEP 2: Clone the Azure Sphere Learning Path

**WARNING**: On Windows, do not clone the *Azure Sphere Learning Path* to a directory with a long folder name as CMake can have problems building with long path/folder names.

Ideally clone to a directory close to the root directory. For example **c:\lab** on Windows, **~/lab** on Linux.

```bash
git clone https://github.com/gloveboxes/Azure-Sphere-Learning-Path.git Azure-Sphere
```

---

## STEP 3: Under Azure Learning Path Layout

The labs support the three currently available developer kits from Avnet and Seeed Studio.

**Ensure you use the labs that match your hardware.**

Lab folders by Azure Sphere Developer kit:

**Azure-Sphere
|- AVNET
|- SEEED
|- SEEED-MINI**

---

## STEP 4: Set up Azure Sphere SDK, Claim the Azure Sphere, and Configure Networking

Follow the Azure Sphere [Overview of set up procedures](https://docs.microsoft.com/en-au/azure-sphere/install/overview?WT.mc_id=github-blog-dglover) guide.

---

## Azure Sphere Developer Kit Reference Documentation

* [Avnet Azure Sphere Start Kit - See Documents tab](https://www.avnet.com/shop/us/products/avnet-engineering-services/aes-ms-mt3620-sk-g-3074457345636825680/)
* [Seeed Studio Azure Sphere MT3620 Reference Docs](http://wiki.seeedstudio.com/Azure_Sphere_MT3620_Development_Kit/)
* [Grove Starter Kit for Azure Sphere MT3620 Development Kit](http://wiki.seeedstudio.com/Grove_Starter_Kit_for_Azure_Sphere_MT3620_Development_Kit/)
* [Seeed Studio Azure Sphere MT3620 Mini Dev Board](http://wiki.seeedstudio.com/MT3620_Mini_Dev_Board/)
* [MT3620 Grove Breakout](http://wiki.seeedstudio.com/MT3620_Grove_Breakout/)

