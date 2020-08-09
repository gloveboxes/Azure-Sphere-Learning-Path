# Lab 3: Set the room virtual thermostat with Azure IoT Device Twins

---

|Author|[Dave Glover](https://developer.microsoft.com/en-us/advocates/dave-glover?WT.mc_id=julyot-azd-dglover), Microsoft Cloud Developer Advocate, [@dglover](https://twitter.com/dglover) |
|:----|:---|
|Date| June 2020|

---

## Azure Sphere Learning Path

Each module assumes you have completed the previous module.

[Home](../../../README.md)

* Lab 0: [Lab Set Up](../Lab_0_Introduction_and_Lab_Set_Up/README.md)
* Lab 1: [Introduction to Azure Sphere development](../Lab_1_Visual_Studio_and_Azure_Sphere/README.md)
* Lab 2: [Connect a room environment monitor to Azure IoT](../Lab_2_Send_Telemetry_to_Azure_IoT_Hub/README.md)
* Lab 3: [Set the room virtual thermostat with Azure IoT Device Twins](../Lab_3_Control_Device_with_Device_Twins/README.md)
* Lab 4: [Remote reboot your Azure Sphere with Azure IoT Direct Methods](../Lab_4_Control_Device_with_Direct_Methods/README.md)
* Lab 5: [Integrate FreeRTOS Real-time room sensors with Azure IoT](../Lab_5_FreeRTOS_and_Inter-Core_Messaging/README.md)
* Lab 6: [Integrate Azure RTOS Real-time room sensors with Azure IoT](../Lab_6_AzureRTOS_and_Inter-Core_Messaging/README.md)
* Lab 7: [Connect and control your room environment monitor with Azure IoT](../Lab_7_Azure_IoT_Central_RTOS_Integration/README.md)
<!-- * Lab 8: [Over-the-air (OTA) Deployment](/docs/Lab_8_Over-the-air-deployment/README.md) -->

---

## What you will learn

You will learn how to control an [Azure Sphere](https://azure.microsoft.com/services/azure-sphere/?WT.mc_id=julyot-azd-dglover) application using [Azure IoT Hub Device Twins](https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-devguide-device-twins?WT.mc_id=julyot-azd-dglover).

---

## Prerequisites

This lab assumes you have completed [Connect a room environment monitor to Azure IoT](../Lab_2_Send_Telemetry_to_Azure_IoT_Central/README.md). You will have created an Azure IoT Hub, a Device Provisioning Service, connected your Azure IoT Device Provisioning Service to your Azure Sphere Tenant, and you have configured the **app_manifest.json** for Azure IoT.

---

## Introduction to the Learning Path Labs

There are a number of Learning Path libraries that support these labs. These Learning Path C functions are prefixed with **lp_**, typedefs and enums are prefixed with **LP_**. 

The Learning Path libraries are open source and contributions are welcome.

The Learning Path libraries are built from the [Azure Sphere Samples](https://github.com/Azure/azure-sphere-samples) and aim to demonstrate best practices.

The Learning Path libraries are **not** part of the official Azure Sphere libraries or samples.

---

## Tutorial Overview

There are three options for Azure IoT cloud to device communications: 

1. [Direct Methods](https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-devguide-direct-methods?WT.mc_id=julyot-azd-dglover) for communications that require immediate confirmation of the result. Direct methods are often used for interactive control of devices, such as turning on a fan.

2. [Device Twins](https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-devguide-device-twins?WT.mc_id=julyot-azd-dglover) are for long-running commands intended to put the device into a certain desired state. For example, set the sample rate for a sensor to every 30 minutes.

3. [Cloud-to-device](https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-devguide-messages-c2d?WT.mc_id=julyot-azd-dglover) messages are for one-way notifications to the device app.

This lab will cover Azure IoT Device Twins.

---

## Key Concepts

In Lab 1, **Peripherals** and **Event Timers** were introduced to simplify and describe GPIO pins and Event Timers and their interactions.

In this lab, **DeviceTwinBindings** are introduced to simplify the implementation of Azure IoT [Device Twins](https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-devguide-device-twins?WT.mc_id=julyot-azd-dglover).

---

## Azure IoT Device Twins

Device twins are JSON documents that store device information, including metadata, configurations, and conditions. Azure IoT Hub keeps a device twin for each device that you connect to IoT Hub.

You can use device twins as follows:

1. Cloud to device updates.

	Typically an IoT Hub application would set the value of a device twin. For example, to set the temperature of the room. IoT Hub sends a desired property device twin message to the device. The device actions the desired property, and then the device responds with a reported property device twin message. Azure IoT Hub stores the reported property.
2. Device to cloud updates.

	A device can send a reported property device twin message to Azure. For example, a device could report its firmware level on startup.  Azure IoT Hub stores the reported property. 

3. Querying reported properties.

	With device twins reported state stored in Azure, it is possible to query the stored device twin properties cloud side. For example, list all devices with a firmware version less than 2.0, as these devices require an update. Or, list all rooms with a temperature setting higher than 25 degrees Celsius.

The following outlines how Azure IoT Hub uses Device Twins to set properties on a device: 

1. Typically an Azure IoT Hub application sets the value of a device twin. For example, set the desired room temperature.
2. Azure IoT Hub sends a desired property message to the device.
3. The device implements the desired property; in this case, turn on the heater or cooler to bring the room to the desired temperature.
4. The device sends a reported property message back to Azure IoT. In this example, the device would report the new desired temperate.
5. Your application can then query the device twin report property cloud side.

![](resources/device-twin-configuration-pattern.png)

For more information, refer to the [Understand and use device twins in IoT Hub](https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-devguide-device-twins?WT.mc_id=julyot-azd-dglover) article.

---

## Device Twin Bindings

A Device Twin Bindings maps a device twin with a device property and a handler function that implements the action.

### Cloud to Device Updates

The following example declares a variable named **desiredTemperature** of type **LP_DEVICE_TWIN_BINDING**. This variable maps the Azure IoT Hub device twin **DesiredTemperature** with a handler function named **DeviceTwinSetTemperatureHandler**.

```c
static LP_DEVICE_TWIN_BINDING desiredTemperature = { 
	.twinProperty = "DesiredTemperature", 
	.twinType = LP_TYPE_FLOAT, 
	.handler = DeviceTwinSetTemperatureHandler 
};
```

The following is the implementation of the handler function **DeviceTwinSetTemperatureHandler**. The handler function is called when the device receives a **DesiredTemperature** desired property message from Azure IoT Hub.

```c
/// <summary>
/// Device Twin Handler to set the desired temperature value
/// </summary>
static void DeviceTwinSetTemperatureHandler(LP_DEVICE_TWIN_BINDING* deviceTwinBinding)
{
	if (deviceTwinBinding->twinType == LP_TYPE_FLOAT)
	{
		desired_temperature = *(float*)deviceTwinBinding->twinState;
		SetTemperatureStatusColour(last_temperature);
	}
}
```

### Device to Cloud Updates

The following example declares an **actualTemperature** device twin property of type float. There is no handler function registered as this is a one-way device to cloud binding.

```c
static LP_DEVICE_TWIN_BINDING actualTemperature = {
		.twinProperty = "ActualTemperature",
		.twinType = LP_TYPE_FLOAT 
};
```

The ActualTemperature reported property message is sent to IoT Hub by calling the DeviceTwinReportState function. You must pass a property of the correct type.

```c
lp_deviceTwinReportState(&actualTemperature, &last_temperature); // TwinType = LP_TYPE_FLOAT
```

---

## Working with Device Twins

Device twin bindings must be added to the **deviceTwinBindingSet**. When a device twin message is received from Azure, this set is checked for a matching *twinProperty* name. When a match is found, the corresponding handler function is called.

```c
LP_DEVICE_TWIN_BINDING* deviceTwinBindingSet[] = { &desiredTemperature, &actualTemperature };
```

### Opening

Sets are initialized in the **InitPeripheralsAndHandlers** function found in **main.c**.

```c
lp_openDeviceTwinSet(deviceTwinBindingSet, NELEMS(deviceTwinBindingSet));
```

### Dispatching

When a Device Twin message is received, the deviceTwinBindingSet is checked for a matching **twinProperty** name. When a match is found, the corresponding handler function is called.

### Closing

Sets are closed in the **ClosePeripheralsAndHandlers** function found in **main.c**.

```c
lp_closeDeviceTwinSet();
```

---

## Open Lab 3

### Step 1: Start Visual Studio Code

![](resources/vs-code-start.png)

### Step 2: Open the lab project

1. Click **Open folder**.
2. Open the Azure-Sphere lab folder.
3. Open the **Lab_3_Device_Twins** folder.
4. Click **Select Folder** button to open the project.

    <!-- ![](resources/visual-studio-open-lab3.png) -->

### Step 3: Set your developer board configuration

These labs support developer boards from AVNET and Seeed Studio. You need to set the configuration that matches your developer board.

The default developer board configuration is for the AVENT Azure Sphere Starter Kit. If you have this board, there is no additional configuration required.

1. Open CMakeList.txt
	![](resources/vs-code-open-cmake.png)
2. Add a # at the beginning of the set AVNET line to disable it.
3. Uncomment the **set** command that corresponds to your Azure Sphere developer board.

	```text
	set(AVNET TRUE "AVNET Azure Sphere Starter Kit")                
	# set(SEEED_STUDIO_RDB TRUE "Seeed Studio Azure Sphere MT3620 Development Kit (aka Reference Design Board or rdb)")
	# set(SEEED_STUDIO_MINI TRUE "Seeed Studio Azure Sphere MT3620 Mini Dev Board")
	```	

4. Save the file. This will auto-generate the CMake cache.

### Step 4: Configure the Azure IoT connection information

1. Open the **app_manifest.json** file.
2. You will need to redo the settings for the **app_manifest.json** file. Either copy from **Notepad** if you still have it open or copy from the **app_manifest.json** file you created in lab 2.
3. Paste the contents of the clipboard into **app_manifest.json** and save the file.
4. **Review** your updated manifest_app.json file. It should look similar to the following.

    ```json
    {
        "SchemaVersion": 1,
        "Name": "AzureSphereIoTCentral",
        "ComponentId": "25025d2c-66da-4448-bae1-ac26fcdd3627",
        "EntryPoint": "/bin/app",
        "CmdArgs": [ "0ne0099999D" ],
        "Capabilities": {
            "Gpio": [
                "$BUTTON_A",
                "$BUTTON_B",
                "$LED2",
                "$NETWORK_CONNECTED_LED",
                "$LED_RED",
                "$LED_GREEN",
                "$LED_BLUE"
            ],
            "I2cMaster": [ "$I2cMaster2" ],
            "PowerControls": [ "ForceReboot" ],
            "AllowedConnections": [
                "global.azure-devices-provisioning.net",
                "<Your Device Provisioning Service Endpoint>",
                "<Your Azure IoT Hub Endpoint>"
            ],
            "DeviceAuthentication": "9d7e79eb-9999-43ce-9999-fa8888888894"
        },
        "ApplicationType": "Default"
    }
    ```

---

### Step 5: Start the app build deploy process

1. Ensure main.c is open.
2. Select **CMake: [Debug]: Ready** from the Visual Studio Code Status Bar.

	![](resources/vs-code-start-application.png)

3. From Visual Studio Code, press <kbd>F5</kbd> to build, deploy, start, and attached the remote debugger to the application now running the Azure Sphere device.

---

## Expected Device Behaviour

### Avnet Azure Sphere MT3620 Starter Kit

![](resources/avnet-azure-sphere.jpg)

1. The RGB LED5 will start to blink. In the next section, you will be setting the desired temperature which will determine the RGB LED colour.
2. LED4 will turn yellow when connected to Azure.
3. LED3 will blink when telemetry is sent to IoT Hub
4. Press **Button A** or **Button B** on the device to change the blink rate of LED5 and to update the **Actual Temperature** device twin property in IoT Hub.

### Seeed Studio Azure Sphere MT3620 Development Kit

![](resources/seeed-studio-azure-sphere-rdb.jpg)

1. The RGB LED 1 will start to blink. In the next section, you will be setting the desired temperature which will determine the RGB LED colour.
2. The network LED will turn red when connected to Azure.
3. LED 4 will blink blue when telemetry is sent to IoT Hub
4. Press **Button A** or **Button B** on the device to change the blink rate of RGB LED 1 and to update the **Actual Temperature** device twin property in IoT Hub.

### Seeed Studio MT3620 Mini Dev Board

![](resources/seeed-studio-azure-sphere-mini.png)

1. The green LED closest to the USB connector will start to blink
2. Given this device has no builtin buttons then virtual **Button A** and **Button B** presses will be generated every 10 seconds. The blink rate will change and the **Actual Temperature** device twin property in IoT Hub will be updated.

---

## Display the actual temperature device twin

We are going to use the Azure command-line tool to view and update the IoT Hub device twins. For more information see [az iot hub device-twin](https://docs.microsoft.com/en-us/cli/azure/ext/azure-cli-iot-ext/iot/hub/device-twin?view=azure-cli-latest&WT.mc_id=julyot-azd-dglover).

Follow these steps to show the actual temperature recorded by the device:

1. For Azure Sphere devices with buttons, press Button A or B. Virtual button presses are generated for devices without buttons
2. You need the name of the Azure IoT Hub you created. You can get the name from the Azure web portal.
    ![](resources/azure-iot-resources.png)
3. You need the Azure Sphere device ID. Run the following command from the **Azure Sphere Developer Command Prompt**.

    ```bash
    azsphere dev show-attached
    ```

4. Open the Azure Cloud shell by right mouse clicking on the following link and opening in a new tab "[https://shell.azure.com](https://shell.azure.com)".
5. In the Cloud Shell, run the [az extension add](https://docs.microsoft.com/cli/azure/extension?view=azure-cli-latest&WT.mc_id=julyot-azd-dglover) command to add the Microsoft Azure IoT Extension to your CLI shell. The IoT Extension adds IoT Hub, IoT Edge, and IoT Device Provisioning Service (DPS) specific commands to Azure CLI.

    ```bash
    az extension add --name azure-iot
    ```

6. Set a bash variable in the Cloud Shell for your Azure Sphere Device ID.  Make sure there are no spaces either side of the **=** character.

    ```bash
    DEVICE_ID={your Azure Sphere device ID}
    ```

7. Set a bash variable in the Cloud Shell for your IoT Hub name. Make sure there are no spaces either side of the **=** character.

    ```bash
    HUB_NAME={your hub name}
    ```

8. Show the latest state of the device twin JSON document. Review the **reported** section for the **ActualTemperature** property.

    ```
    az iot hub device-twin show --device-id "${DEVICE_ID,,}" -n $HUB_NAME
    ```

    > Note, IoT Hub requires Device IDs to be lowercase. The bash command **"${DEVICE_ID,,}"** in the device twin show command converts the device id to lowercase.

9. Review the device twin **reported** section and look for the updated **ActualTemperature** property.

    ```json
    {
      "authenticationType": "selfSigned",
      "capabilities": {
        "iotEdge": false
      },
      "cloudToDeviceMessageCount": 0,
      "connectionState": "Connected",
      "deviceEtag": "NjgyOTY2MjU4",
      "deviceId": "3ea5ef99999999999999999999999999999990247a0bc6d37029999999996b8183ce07c1817d02cdd95b0d8b999991a201a8c34137935",
      "etag": "AAAAAAAAAAE=",
      "lastActivityTime": "0001-01-01T00:00:00Z",
      "properties": {
        "desired": {
          "$metadata": {
            "$lastUpdated": "2020-06-22T05:52:42.7416771Z"
          },
          "$version": 1
        },
        "reported": {
          "$metadata": {
            "$lastUpdated": "2020-06-22T05:52:58.8264962Z",
            "ActualTemperature": {
              "$lastUpdated": "2020-06-22T05:52:58.8264962Z"
            }
          },
          "$version": 2,
          "ActualTemperature": 29.0
        }
      },
      "status": "enabled",
      "statusUpdateTime": "0001-01-01T00:00:00Z",
      "version": 3,
      "x509Thumbprint": {
        "primaryThumbprint": "188109118C6F979078C9999999999D9B231DA41BA4706EC",
        "secondaryThumbprint": "1881091180000000000999650AE3250B4313385DD9B231DA41BA4706EC"
      }
    }
    ```

10. Leave the Cloud Shell open for the next section.

---

## Set the desired temperature device twin

Setting the desired temperature is like setting a thermostat in a room. A desired temperature device twin property message is sent to the Azure Sphere where it will be acted upon.

When the temperature sensor is read on the Azure Sphere device it is compared to the desired temperature.

* If the temperature is greater than the desired temperature the blinking LED will turn blue to indicate the cooler needs be turned on.
* If the temperature is less than the desired temperature then the blinking LED will turn red to indicate the heater needs to be turned on.
* If the temperature is the same as the desired temperature then the blinking LED turns green to indicate no action required.

> The AVNET Azure Sphere Starter Kit has a built-in temperature sensor. For other boards, a random temperature 25 +/- 5 degrees celsius is generated.

To set the desired temperature:

1. The **DEVICE_ID** and **HUB_NAME** bash variables were set the previous section.

2. Run the iot hub device twin update.

    ```bash
    az iot hub device-twin update --device-id "${DEVICE_ID,,}" -n $HUB_NAME --set properties.desired='{"DesiredTemperature":{"value":25}}'
    ```

    > Note, IoT Hub requires Device IDs to be lowercase. The bash command **"${DEVICE_ID,,}"** in the device twin show command converts the device id to lowercase.

3. Observe the blinking LED on the Azure Sphere may change colour depending on the desired temperature you set.
4. Show the latest state of the device twin. Review the **desired** and **reported** sections for the **DesiredTemperature** property.

    ```
    az iot hub device-twin show --device-id "${DEVICE_ID,,}" -n $HUB_NAME
    ```

5. Try setting more extreme desired temperatures. For example, try 0 or 100. Again, observe the blinking LED will change colour.
6. Close the Cloud Shell by typing ```exit```.

---

## Close Visual Studio

Now close **Close Visual Studio**.

---

## Finished 完了 fertig finito समाप्त terminado

Congratulations you have finished lab 3.

![](resources/finished.jpg)

---

**[NEXT](../Lab_4_Control_Device_with_Direct_Methods/README.md)**

---
