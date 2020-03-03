# Lab 2: Send Telemetry from an Azure Sphere to Azure IoT Central

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

## What you will learn

You will learn how to build a High Level [Azure Sphere](https://azure.microsoft.com/services/azure-sphere/?WT.mc_id=github-blog-dglover) application that connects and sends telemetry to [Azure IoT Central](https://azure.microsoft.com/services/iot-central/?WT.mc_id=github-blog-dglover).

---

## Prerequisites

The lab assumes you understand the content from [Lab 1: Secure, Build, Deploy and Debug your first Azure Sphere High-Level Application with Visual Studio](../Lab_1_Visual_Studio_and_Azure_Sphere/README.md)

## Tutorial Overview

1. Create an Azure IoT Central Application (Free trial)
2. Link your Azure IoT Central to your Azure Sphere Tenant
3. Deploy an Azure IoT Central enabled application to Azure Sphere

---

## Key Concepts

### Azure IoT

Your Azure Sphere device can securely connect and communicate with cloud services. Azure Sphere includes built-in library support for both Azure IoT Hub and Azure IoT Central. This lab focuses on Azure IoT Central.

This project leverages the [Azure IoT Hub Device Provisioning Service (PDS)](https://docs.microsoft.com/en-us/azure-sphere/app-development/use-azure-iot?WT.mc_id=github-blog-dglover) which is included with Azure IoT Central. The Device Provisioning Service (DPS) enables zero-touch, just-in-time, large scale device provisioning.

### Azure IoT Central

[Azure IoT Central](https://azure.microsoft.com/en-in/services/iot-central/?WT.mc_id=github-blog-dglover) provides an easy way to connect, monitor and manage your Internet of Things (IoT) assets at scale.

![Azure IoT Central](resources/azure-iot-central.jpg)

---

## Getting Started with Azure IoT Central

We are going to create an Azure IoT Central application from an application template.

---

## Step 1: Create an Azure IoT Central Application

For this lab, we are going to create an Azure IoT Central Application from a predefined template.

Right mouse click [Azure Sphere Learning Path Azure IoT Central Template](https://apps.azureiotcentral.com/build/new/107ea05b-c031-4866-a0f3-60c934551ac7) to open in a new browser tab.

### Configure the Azure IoT Central Template

1. Name your application
2. Select the **Free** pricing plan

![](resources/iot-central-create-from-template.png)

When you have connected your Azure Sphere to Azure IoT Central you will be able to visualize the device telemetry.

![](resources/iot-central-azure-sphere-dashboad.png)

<!-- Follow instructions to **[Create an Azure IoT Central Application](resources/azure-iot-central/azure-iot-central.md)** -->

---

## Link Azure IoT Central with your Azure Sphere Tenant

You must **link** your Azure IoT Central Application with the Azure Sphere Tenant that your Azure Sphere was claimed into.

**Right mouse click, and open in a new tab the [instructions to set up Azure IoT Central to work with Azure Sphere](https://docs.microsoft.com/en-au/azure-sphere/app-development/setup-iot-central?WT.mc_id=github-blog-dglover#step-2-download-the-tenant-authentication-ca-certificate)**.

---

## Azure IoT Central Connection Information

The Azure Sphere device requires the following information to connect to your Azure IoT Central Application.

1. The **Azure IoT Central Application**:
    * **Scope ID**, and the **Application URI**.
2. The **Azure Sphere Tenant ID**.

### Azure IoT Central Application Information

You are going to run a command that returns information from Azure IoT Central. You will be asked a series of questions. 

The following describes where you will find the answers to those questions.

1. **Are you using a legacy (2018) IoT Central application (Y/N)**. Answer **N**.
2. **URL of your Azure IoT Central Application**. From your browser, open your Azure IoT Central Application and copy the **URL** from the web browser address bar.
![](resources/iot-central-application-url.png)
3. **Enter your Azure IoT Central application API Token**.
    * From your Azure IoT Central Application, select the **Administration** Tab, then **API Tokens**, then click **+ Generate token**.
    * Name the token, for example **myapp**.
    ![](resources/iot-central-admin-api-tokens-add.png).
4. **Enter the ID Scope from the IoT Central App (Administration | Device connection)**
    * From your Azure IoT Central Application, select the **Administration** Tab, then **Device connection**.
    ![](resources/iot-central-admin-device-connection-scope-id.png)

1. Start a Windows or Linux **Command Prompt**
2. From the **command prompt**, navigate to the directory you cloned the **Azure Sphere** lab into.
3. Change to the **tools** directory for your platform. On Windows, ```cd tools\win-x64```, on Linux, ```cd tools/linux-x64```.
4. Run the **```ShowIoTCentralConfig.exe```** command. On completion the command will return information similar to the following:
    ```text
    Find and modify the CmdArgs, AllowedConnections and DeviceAuthentication lines in your app_manifest.json so each includes the content from the below:
    "CmdArgs": [ "0ne000BDC00" ],
    "Capabilities": {
        "AllowedConnections": [ "global.azure-devices-provisioning.net", "iotc-99999999bc-9999-4cba-999e-6573fc4cf701.azure-devices.net" ],
        "DeviceAuthentication": "--- YOUR AZURE SPHERE TENANT ID---",
    }

    Obtain your Azure Sphere Tenant ID by opening an Azure Sphere Developer Command Prompt/Terminal and typing the following command:
    'azsphere tenant show-selected'
    ```
   
5. Open **Notepad**
6. Copy **two lines** from the output of _ShowIoTCentralConfig_ to **Notepad**. The following lines are required. Note, your values will be different.
    * ```"CmdArgs": [ "0ne9992KK6D" ]```
    * ```"AllowedConnections": [ "global.azure-devices-provisioning.net", "iotc-99999999bc-9999-4cba-999e-6573fc4cf701.azure-devices.net" ]```

    The **CmdArgs** value is the **Scope ID** of your Azure IoT Central Application. The **Allowed connections** values includes the global URI for the Device Provision Service, followed by the your **Azure IoT Central Application URI**.

### Azure Sphere Tenant ID

1. From the **Azure Sphere Developer Command Prompt**, run **```azsphere tenant show-selected```**.
    * The output of this command will look similar to the following.
        ```text
        Default Azure Sphere tenant ID is 'yourSphereTenant' (99999999-e021-43ce-9999-fa9999499994).
        ```
    * The **Tenant ID** is the numeric value inside the parentheses.
2. **Copy the Tenant ID to _Notepad_** as you will need it soon.

---

## Open Lab 2

### Step 1: Start Visual Studio 2019

![](resources/visual-studio-open-local-folder.png)

### Step 2: Open the lab project

1. Click **Open a local folder**
2. Open the Azure-Sphere lab folder
3. Open the **folder name** that corresponds to the **Azure Sphere board** you are using
4. Open the **Lab_2_Send_Telemetry_to_Azure_IoT_Central** folder
5. Click **Select Folder** button to open the project

### Step 3: Configure the Azure Sphere Application

1. Open the **app_manifest.json** file
    ![](resources/visual-studio-open-app-manifest.png)


2.  Update the **app_manifest.json** with the information you saved to **notepad**:

    * **CmdArgs**: Update with your Azure IoT Central **ID Scope**.
    * **AllowedConnections**: Update with your **Azure IoT Central Application URL**.
    * **DeviceAuthentication**: Your **Azure Sphere Tenant ID**. Remember, this was the numeric value output from the ```azsphere tenant show-selected``` command.

3. Review your **manifest_app.json** file. It should look similar to the following when you have updated it.

    ```json
    {
        "SchemaVersion": 1,
        "Name": "AzureSphereIoTCentral",
        "ComponentId": "25025d2c-66da-4448-bae1-ac26fcdd3627",
        "EntryPoint": "/bin/app",
        "CmdArgs": [ "Fake", "0ne0099999D" ],
        "Capabilities": {
            "Gpio": [ "$AVNET_MT3620_SK_GPIO0", "$AVNET_MT3620_SK_APP_STATUS_LED_YELLOW", "$AVNET_MT3620_SK_WLAN_STATUS_LED_YELLOW" ],
            "Uart": [],
            "I2cMaster": [ "$AVNET_MT3620_SK_ISU2_I2C" ],
            "Adc": [ "$AVNET_MT3620_SK_ADC_CONTROLLER0" ],
            "AllowedConnections": [ "global.azure-devices-provisioning.net", "saas-iothub-99999999-f33a-9999-a44a-7c99999900b6.azure-devices.net" ],
            "DeviceAuthentication": "9d7e79eb-9999-43ce-9999-fa8888888894"
        },
        "ApplicationType": "Default"
    }
    ```

4. **IMPORTANT**. Copy the contents of the **app_manifest.json** file to **notepad** as you will need this configuration information for the next labs.

---

## Build, Deploy and Debug the Azure Sphere Application

Before building the application with Visual Studio ensure ARM-Debug and GDB Debugger (HLCore) options are selected.

![](resources/visual-studio-start-config.png)

### Sending Telemetry to Azure IoT Central

In **main.c**, scroll down to the **MeasureSensorHandler** C Function. In this function there is a call to **SendMsg(msgBuffer);**. This will send the JSON telemetry to Azure IoT Central.

```c
static void MeasureSensorHandler(EventLoopTimer* eventLoopTimer)
{
	if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0) {
		Terminate();
		return;
	}

	GPIO_ON(builtinLed); // blink send status LED

	if (ReadTelemetry(msgBuffer, JSON_MESSAGE_BYTES) > 0) {
		Log_Debug("%s\n", msgBuffer);
		SendMsg(msgBuffer);
	}

	GPIO_OFF(builtinLed);
}
```

### Start the Azure Sphere Application

To start the build, deploy, debug process, either click the Visual Studio **Start Selected Item** icon or press **<kbd>F5</kbd>**. To Build and deploy without attaching the debugger, simply press **<kbd>Ctrl+F5</kbd>**.

![](resources/visual-studio-start-debug.png)

---

## Migrate your device to an Azure IoT Central Template

1. Open the Azure IoT Central Application you created.
2. Open the **Devices** tab. **You will need to wait a minute or two before the new device listed. It will have a numeric name.**
This numeric name is the ID of your Azure Sphere Device. You can check this by running the  ```azsphere device show``` command from the *Azure Sphere Command Prompt*.
4. Select this new device and click **Migrate**
    ![](resources/iot-central-migrate-device.png)
5. Migrate the Azure Sphere device to the **Azure Sphere** Template
    ![](resources/iot-central-migrate-select-template.png)

---

## Azure IoT Central Dashboard

You may need to wait a minute or two before the telemetry is displayed on the **Overview** tab.

Note, you can also rename the device but clicking on the **Rename** button.

![](resources/iot-central-display-measurements.png)

## Close Visual Studio

Now close **Close Visual Studio**.

---

## Finished 完了 fertig finito ख़त्म होना terminado

Congratulations you have finished Lab 2.

![](resources/finished.jpg)

---

**[NEXT](../Lab_3_Control_Device_with_Device_Twins_and_Direct_Methods/README.md)**
