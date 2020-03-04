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

1. Windows 10 Anniversary Update or later, or Ubuntu 18.04 LTS
2. An Azure Sphere device

---

## Azure Sphere Tenant Credentials

### Personal Azure Sphere Device

If you have your own Azure Sphere device you will need the credentials you used when you claimed your device into an Azure Sphere Tenant.

### Shared Lab Azure Sphere Device

If you are using a shared lab device, then the lab instructor will tell you the Azure Sphere Tenant credentials you will need for the lab.

---

## Step 1: Software Installation

You can develop applications for Azure Sphere on Windows 10 Anniversary Update or later, or a Linux machine running Ubuntu 18.04 LTS

### Windows 10 Recommended Software

1. [Visual Studio (The free Community Edition or better)](https://visualstudio.microsoft.com/vs/?WT.mc_id=github-blog-dglover)
2. [Azure Sphere SDK for Visual Studio](https://docs.microsoft.com/en-au/azure-sphere/install/install-sdk#azure-sphere-sdk-for-visual-studio)
3. [Git for Windows](https://git-scm.com/downloads)
4. [Install Azure Sphere drivers for Windows](https://docs.microsoft.com/en-au/azure-sphere/install/install-sdk#set-up-your-dev-kit-on-a-windows-pc)

### Ubuntu 18.04 Recommended Software

1. [Visual Studio Code](https://code.visualstudio.com/?WT.mc_id=github-blog-dglover)
2. [Azure Sphere Extension for Visual Studio Code](https://marketplace.visualstudio.com/items?itemName=ms-vscode.azure-sphere-tools&WT.mc_id=github-blog-dglover).
3. [Install the Azure Sphere SDK on Linux](https://docs.microsoft.com/en-au/azure-sphere/install/install-sdk-linux)
4. [Set up your Linux system for app development](https://docs.microsoft.com/en-au/azure-sphere/install/development-environment-linux)
4. Install Git support **```apt install git```**.

For complete instructions follow the Azure Sphere [Overview of set up procedures](https://docs.microsoft.com/en-au/azure-sphere/install/overview?WT.mc_id=github-blog-dglover) guide.

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

## Step 4: Login to your Azure Sphere Tenant

You will need the credentials for the Azure Sphere Tenant your device was claimed into.

If you are using a lab device, then the lab instructor will provide you with the credentials.

1. Start the **Azure Sphere Developer Command Prompt**
	1. Press the <kbd>Windows Key</kbd>
	2. Start typing **Azure Sphere Developer Command Prompt**
	3. Select and start the **Azure Sphere Developer Command Prompt**.
2. From the **Azure Sphere Developer Command Prompt**, run **```azsphere login```**
    
    You will be prompted for the Azure Sphere Tenant credentials.

---

## Step 5: Claim your Azure Sphere

**Skip this step if you are using a lab device or you have already claimed your Azure Sphere.**

**Note**, claiming an Azure Sphere device is a one-time operation. The device will become associated with an Azure Sphere Tenant and the email address you used when you claimed the device.

You **cannot move** the device to another Azure Tenant once it has been claimed. However, you can add additional [users and roles](https://docs.microsoft.com/en-us/azure-sphere/deployment/add-tenant-users) to the Azure Sphere tenant, including users with Admin rights.

1. From the **Azure Sphere Developer Command Prompt**, run **```azsphere device claim```**

For more information visit [Claim your Azure Sphere device](https://docs.microsoft.com/en-au/azure-sphere/install/claim-device?WT.mc_id=github-blog-dglover).

## Step 6: Select the default Azure Sphere Tenant

From the **Azure Sphere Developer Command Prompt**:

1. List Azure Sphere Tenants, run **```azsphere tenant list```**
2. Select the default Azure Sphere Tenant
	If there is more than one tenant listed then you need to set the default tenant.
	
	Run **```azsphere tenant select -i <Tenant ID>```**

---

## Step 7: Configure the Azure Sphere WiFi Network

From the **Azure Sphere Developer Command Prompt**:

1. List the existing WiFi network connections. Run **```azsphere device wifi list```**
2. To add a new WiFi network connection. Run **```azsphere device wifi add -s <Your WiFi SSID> -p <Your WiFI Password>```**
3. For more information on networking, visit [Configure the Azure Sphere WiFi Settings](https://docs.microsoft.com/en-au/azure-sphere/install/configure-wifi?WT.mc_id=github-blog-dglover)

---

## Step 8: Ensure Azure Sphere OS Updated

As of March 2020, your device should be version **20.01** or better.

From the **Azure Sphere Developer Command Prompt**:

1. Check the device OS version **```azsphere dev show-os-version```**
2. If the device is not up to date, then restart to trigger the update process. Run **```azsphere dev restart```**
3. It will take a few minutes for the update to be applied. Then recheck the OS version **```azsphere dev show-os-version```**

---

## Step 9: Delete any existing applications on the Azure Sphere

From the **Azure Sphere Developer Command Prompt**, run **```azsphere device sideload delete```**

---

## Step 10: Enable High Level Core Device Debugging

From the **Azure Sphere Developer Command Prompt**, run ```azsphere device enable-development```

---

## Step 11: Enable Real Time Core Device Debugging

The **Azure Sphere Developer Command Prompt** must be run as **Administrator**.

1. Press the <kbd>Windows Key</kbd>
2. Start typing **Azure Sphere Developer Command Prompt**
3. Select **Run as administrator**
![](resources/azure-sphere-command-prompt-as-administrator.png)
4. Run **```azsphere device enable-development -r```**

---

## Appendix

### Azure Sphere Developer Kit Reference Documentation

* [Avnet Azure Sphere Start Kit - See Documents tab](https://www.avnet.com/shop/us/products/avnet-engineering-services/aes-ms-mt3620-sk-g-3074457345636825680/)
* [Seeed Studio Azure Sphere MT3620 Reference Docs](http://wiki.seeedstudio.com/Azure_Sphere_MT3620_Development_Kit/)
* [Grove Starter Kit for Azure Sphere MT3620 Development Kit](http://wiki.seeedstudio.com/Grove_Starter_Kit_for_Azure_Sphere_MT3620_Development_Kit/)
* [Seeed Studio Azure Sphere MT3620 Mini Dev Board](http://wiki.seeedstudio.com/MT3620_Mini_Dev_Board/)
* [MT3620 Grove Breakout](http://wiki.seeedstudio.com/MT3620_Grove_Breakout/)

---

**[NEXT](../Lab_1_Visual_Studio_and_Azure_Sphere/README.md)**

---
