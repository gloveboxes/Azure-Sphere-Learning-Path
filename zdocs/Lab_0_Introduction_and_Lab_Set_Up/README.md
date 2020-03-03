# Lab 0: Lab set up, installation and configuration

|Author|[Dave Glover](https://developer.microsoft.com/en-us/advocates/dave-glover?WT.mc_id=github-blog-dglover), Microsoft Cloud Developer Advocate, [@dglover](https://twitter.com/dglover) |
|:----|:---|
|Source Code | https://github.com/gloveboxes/Azure-Sphere-Learning-Path.git |
|Date| February  2020|

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

## Troubleshooting

1. If you have trouble connecting to the Azure Sphere over USB be sure to disable any VPNs you might have enabled.

---

## Prerequisites

1. Computer running up to date installation of Windows 10 or Ubuntu 18.04
2. An Azure Sphere device

---

## Azure Sphere Tenant Credentials

### Personal Azure Sphere Device

If you have your own Azure Sphere device you will need the credentials you used when you claimed your device into an Azure Sphere Tenant.

### Shared Lab Azure Sphere Device

If you are using a shared lab device, then the lab instructor will tell you the Azure Sphere Tenant credentials you will need for the lab.

---

## Step 1: Software Installation

For complete instructions follow the Azure Sphere [Overview of set up procedures](https://docs.microsoft.com/en-au/azure-sphere/install/overview?WT.mc_id=github-blog-dglover) guide.

The following is the recommended software for Windows 10 and Ubuntu 18.04.

### Windows 10 Recommended Software

1. [Visual Studio (The free Community Edition or better)](https://visualstudio.microsoft.com/vs/?WT.mc_id=github-blog-dglover) is recommended
2. [Azure Sphere SDK for Visual Studio](https://docs.microsoft.com/en-au/azure-sphere/install/install-sdk#azure-sphere-sdk-for-visual-studio)
3. [Git for Windows](https://git-scm.com/downloads)
4. [Install Azure Sphere drivers for Windows](https://docs.microsoft.com/en-au/azure-sphere/install/install-sdk#set-up-your-dev-kit-on-a-windows-pc)

### Ubuntu 18.04 Recommended Software

1. [Visual Studio Code](https://code.visualstudio.com/?WT.mc_id=github-blog-dglover)
2. [Azure Sphere Extension for Visual Studio Code](https://marketplace.visualstudio.com/items?itemName=ms-vscode.azure-sphere-tools&WT.mc_id=github-blog-dglover).
3. [Install the Azure Sphere SDK on Linux](https://docs.microsoft.com/en-au/azure-sphere/install/install-sdk-linux)
4. [Set up your Linux system for app development](https://docs.microsoft.com/en-au/azure-sphere/install/development-environment-linux)
4. Install Git support ```apt install git```.

---

## Step 2: Clone the Azure Sphere Learning Path

**WARNING**: On Windows, do not clone the *Azure Sphere Learning Path* to a directory with a long folder name as CMake can have problems building with long path/folder names.

Ideally clone (or download) to a directory close to the root directory. For example **c:\lab** on Windows, **~/lab** on Linux.

```bash
git clone https://github.com/gloveboxes/Azure-Sphere-Learning-Path.git Azure-Sphere
```

---

## Step 3: Review the Azure Learning Path Directory Layout

The labs support the three currently available developer kits from Avnet and Seeed Studio.

**Ensure you use the labs that match your hardware.**

Lab directories are named by Azure Sphere device manufacturer.

    Azure-Sphere
    |- AVNET
    |- SEEED
    |- SEEED-MINI

---

## Step 4: Claim your Azure Sphere

Skip this step if you are using a lab device or you have already claimed.

[Claim your Azure Sphere device](https://docs.microsoft.com/en-au/azure-sphere/install/claim-device?WT.mc_id=github-blog-dglover)

## Step 5: Configure the Azure Sphere WiFi Networking

[Configure the Azure Sphere WiFi Settings](https://docs.microsoft.com/en-au/azure-sphere/install/configure-wifi?WT.mc_id=github-blog-dglover)

---

**[NEXT](../Lab_1_Visual_Studio_and_Azure_Sphere/README.md)**

---

## Appendix

### Azure Sphere Developer Kit Reference Documentation

* [Avnet Azure Sphere Start Kit - See Documents tab](https://www.avnet.com/shop/us/products/avnet-engineering-services/aes-ms-mt3620-sk-g-3074457345636825680/)
* [Seeed Studio Azure Sphere MT3620 Reference Docs](http://wiki.seeedstudio.com/Azure_Sphere_MT3620_Development_Kit/)
* [Grove Starter Kit for Azure Sphere MT3620 Development Kit](http://wiki.seeedstudio.com/Grove_Starter_Kit_for_Azure_Sphere_MT3620_Development_Kit/)
* [Seeed Studio Azure Sphere MT3620 Mini Dev Board](http://wiki.seeedstudio.com/MT3620_Mini_Dev_Board/)
* [MT3620 Grove Breakout](http://wiki.seeedstudio.com/MT3620_Grove_Breakout/)
