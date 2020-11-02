# Lab 3: Set the room virtual thermostat with Azure IoT Device Twins

---

**Author**: [Dave Glover](https://developer.microsoft.com/en-us/advocates/dave-glover?WT.mc_id=julyot-azd-dglover), Microsoft Cloud Developer Advocate, [@dglover](https://twitter.com/dglover)

---

## Azure Sphere Learning Path

Each module assumes you have completed the previous module.

[Home](../../../README.md)

* Lab 0: [Lab Set Up](../Lab_0_Introduction_and_Lab_Set_Up/README.md)
* Lab 1: [Introduction to Azure Sphere development](../Lab_1_Visual_Studio_and_Azure_Sphere/README.md)
* Lab 2: [Connect a room environment monitor to Azure IoT](../Lab_2_Send_Telemetry_to_Azure_IoT_Central/README.md)
* Lab 3: [Set the room virtual thermostat with Azure IoT Device Twins](../Lab_3_Control_Device_with_Device_Twins/README.md)
* Lab 4: [Remote reboot your Azure Sphere with Azure IoT Direct Methods](../Lab_4_Control_Device_with_Direct_Methods/README.md)
* Lab 5: [Integrate FreeRTOS Real-time room sensors with Azure IoT](../Lab_5_FreeRTOS_and_Inter-Core_Messaging/README.md)
* Lab 6: [Integrate Azure RTOS Real-time room sensors with Azure IoT](../Lab_6_AzureRTOS_and_Inter-Core_Messaging/README.md)
* Lab 7: [Connect and control your room environment monitor with Azure IoT](../Lab_7_Azure_IoT_Central_RTOS_Integration/README.md)
<!-- * Lab 8: [Over-the-air (OTA) Deployment](/docs/Lab_8_Over-the-air-deployment/README.md) -->

---

## What you will learn

In this unit, you will learn how to control an Azure Sphere connected heating, ventilation, and air conditioning unit (HVAC) using [Azure IoT Central](https://azure.microsoft.com/services/iot-central/?WT.mc_id=julyot-azd-dglover) Properties.

---

## Prerequisites

This lab assumes you have completed [Connect a room environment monitor to Azure IoT Central](../Lab_2_Send_Telemetry_to_Azure_IoT_Central/README.md). You will have created an Azure IoT Central application, connected Azure IoT Central to your Azure Sphere Tenant, and you have configured the **app_manifest.json** for Azure IoT Central.

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

## Understanding IoT Central properties

Azure IoT Central uses properties to represent point-in-time values. IoT Central can use properties to set state on a device, for example, set the desired room temperature. A device can also use properties to report its current state, for example, report the operating mode of an HVAC (Heating, Ventilation, and Air Conditioning) unit, is it currently heating, cooling, or turned off.

IoT Central is an Azure IoT Hub application and it sets device properties using IoT Hub device twins. Device twins are JSON documents that Azure IoT Hub keeps in the cloud for all devices and is used for storing device information, including metadata, configurations, and conditions. IoT Hub device twins are often used for long-running commands intended to put the device into a certain state and return it to that state after a device restart. For example, setting the desired room temperature.

Properties can be used in the following ways:

- Cloud-to-device updates
- Device-to-cloud updates
- Querying reported properties

<!-- ### Using device twins

- Cloud-to-device updates.

    A user sets an Azure IoT Central device property such as setting the desired temperature of the room. Azure IoT Central requests Azure IoT Hub to send a device twin message to the device. The device actions and acknowledges the request. Azure IoT Hub updates the device twin reported property and IoT Central can then query and display this reported property.

- Device-to-cloud updates.

    A device can send a device twin message for a reported property to Azure IoT Hub, such as reporting its firmware level on startup. Azure IoT Hub stores the reported property.

- Querying reported properties.

    With the reported state of device twins stored in Azure, it is possible to query the stored device twin properties on the cloud side. For example, list all devices with a firmware version less than 2.0, as these devices require an update. Or, list all rooms with a temperature setting higher than 25 degrees Celsius. -->

---

## Controlling the heating, ventilation, and air conditioning unit (HVAC) using Azure IoT Central properties

From IoT Central, the user sets the desired temperature property. Azure IoT Central requests Azure IoT Hub to send a device twin message to the device. The device actions and acknowledges the request. Azure IoT Hub updates the device twin reported property and IoT Central then queries and display this reported property.

![The illustration shows a device twin configuration pattern.](resources/azure-device-twins-c2d-pattern.png)

### Steps to controlling the HVAC unit from IoT Central

The following steps outline how Azure IoT Central uses device twins to set properties on a device:

1. A user sets the desired room temperature property in Azure IoT Central. IoT Central then requests IoT Hub to update the property.
1. Azure IoT Hub updates the device twin desired property and sends a device twin message to the device.
1. The corrosponding device twin handler function is called.
1. The device implements the desired property; in this case, turn on the heater or cooler to bring the room to the desired temperature.
1. The device acknowledges the updated configuration to Azure IoT Hub. Azure IoT Hub updates the device twin reported property.
1. IoT Central queries and displays the device twin reported property data to the user.

---

## Device twin bindings

Remember, Azure IoT Central properties are implemented using Azure IoT Hub device twins. A device twin binding maps a device twin property name with a handler function that will be called to implement the action.

The following example declares a Device Twin Binding to set the desired room temperature. This declaration maps the Azure IoT Central `DesiredTemperature` property with a handler function named `DeviceTwinSetTemperatureHandler`.

```
static LP_DEVICE_TWIN_BINDING dt_desiredTemperature = {
    .twinProperty = "DesiredTemperature",
    .twinType = LP_TYPE_FLOAT,
    .handler = DeviceTwinSetTemperatureHandler };
```

### Setting the desired temperature

The following is the implementation of the handler function `DeviceTwinSetTemperatureHandler`. The handler function is called when the device receives a `DesiredTemperature` desired property message from Azure IoT Hub.

> [!NOTE]
> As part of the [IoT Plug and Play](https://docs.microsoft.com/en-us/azure/iot-pnp/concepts-convention) conventions, the device should acknowledge the device twin update with a call to **lp_deviceTwinAckDesiredState**.

```
/// <summary>
/// Device Twin Handler to set the desired temperature value
/// </summary>
static void DeviceTwinSetTemperatureHandler(LP_DEVICE_TWIN_BINDING* deviceTwinBinding)
{
    // validate data is sensible range before applying
    if (deviceTwinBinding->twinType == LP_TYPE_FLOAT && *(float*)deviceTwinBinding->twinState >= -20.0f && *(float*)deviceTwinBinding->twinState <= 80.0f)
    {
        lp_deviceTwinAckDesiredState(deviceTwinBinding, deviceTwinBinding->twinState, LP_DEVICE_TWIN_COMPLETED);
        SetHvacStatusColour((int)previous_temperature);
    }
    else {
        lp_deviceTwinAckDesiredState(deviceTwinBinding, deviceTwinBinding->twinState, LP_DEVICE_TWIN_ERROR);
    }
}
```

---

## Reporting the current HVAC operating mode

The HVAC operating mode depends on the room temperature, it can be heating, or cooling, or turned off. We can use a device-to-cloud device twin update to report the current operating mode of the HVAC unit.

### Device-to-cloud updates

![The illustration shows a device twin configuration pattern.](resources/azure-device-twins-d2c-pattern.png)

1. The Azure Sphere detects the HVAC operating mode has changed.
1. The Azure Sphere sends a device twin message to report the new operating mode of the HVAC to Azure IoT Hub.
1. IoT Central queries and displays the updated operating mode of the HVAC unit.

The following example declares a **ReportedHvacState** device twin property of type **string**. A handler function is not required as this is a one-way device-to-cloud binding.

```
static LP_DEVICE_TWIN_BINDING dt_reportedHvacState = {
    .twinProperty = "ReportedHvacState",
    .twinType = LP_TYPE_STRING };
```

The device updates the **ReportedHvacState** property by calling the **lp_deviceTwinReportState** function. You must pass a property of the correct type.

```
lp_deviceTwinReportState(&dt_reportedHvacState, (void*)hvacState[(int)current_led]);
```

---

## How device twin messages are mapped to handlers

All declared device twin bindings must be added by reference to the deviceTwinBindingSet array. When a device twin message is received by the device from Azure IoT Hub it is checked for a matching *twinProperty* name in the deviceTwinBindingSet array. When a match is found, the corresponding handler function is called.

```
LP_DEVICE_TWIN_BINDING* deviceTwinBindingSet[] = { &dt_desiredTemperature, &dt_reportedTemperature, &dt_reportedHvacState };
```

### Opening the device twin binding set

Device twin binding sets are initialized in the **InitPeripheralsAndHandlers** function in **main.c**.

```
lp_deviceTwinSetOpen(deviceTwinBindingSet, NELEMS(deviceTwinBindingSet));
```

### Closing the device twin binding set

Device twin bindings sets are closed in the **ClosePeripheralsAndHandlers** function in **main.c**.

```
lp_deviceTwinSetClose();
```

---

## Azure IoT Central device properties

Azure IoT Central device properties are defined in device templates.

[![The illustration shows device properties.](resources/iot-central-device-template-interface-led1.png)](resources/iot-central-device-template-interface-led1.png)

### Review the IoT Central property definition 

1. From Azure IoT Central web portal, navigate to **Device templates**, and select the **Azure Sphere** template.
2. Click on **Interface** to list the interface capabilities.
3. Scroll down and expand the **Desired Temperature** capability.
4. Review the definition of **Desired Temperature**. The capability type is **Property**, the schema type is **Float**, and the property is **Writeable**. Writeable means this property is enabled for cloud-to-device updates.
5. This must match the DesiredTemperature Device Twin Binding declaration on the Azure Sphere.


---

## Open Lab 3

### Step 1: Start Visual Studio Code

![](resources/vs-code-start.png)

### Step 2: Open the lab project

1. Click **Open folder**.
2. Open the Azure-Sphere lab folder.
3. Open the **Lab_3_Device_Twins** folder.
4. Click **Select Folder** or the **OK** button to open the project.

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

### Step 4: Configure the Azure IoT Central connection information

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
        "CmdArgs": [ "--ConnectionType", "DPS", "--ScopeID", "0ne0099999D" ],
        "Capabilities": {
            "Gpio": [
                "$NETWORK_CONNECTED_LED",
                "$LED_RED",
                "$LED_GREEN",
                "$LED_BLUE"
            ],
            "I2cMaster": [
                "$I2cMaster2"
            ],
                "AllowedConnections": [
                "global.azure-devices-provisioning.net",
                "iotc-9999bc-3305-99ba-885e-6573fc4cf701.azure-devices.net",
                "iotc-789999fa-8306-4994-b70a-399c46501044.azure-devices.net",
                "iotc-7a099966-a8c1-4f33-b803-bf29998713787.azure-devices.net",
                "iotc-97299997-05ab-4988-8142-e299995acdb7.azure-devices.net",
                "iotc-d099995-7fec-460c-b717-e99999bf4551.azure-devices.net",
                "iotc-789999dd-3bf5-49d7-9e12-f6999991df8c.azure-devices.net",
                "iotc-29999917-7344-49e4-9344-5e0cc9999d9b.azure-devices.net",
                "iotc-99999e59-df2a-41d8-bacd-ebb9999143ab.azure-devices.net",
                "iotc-c0a9999b-d256-4aaf-aa06-e90e999902b3.azure-devices.net",
                "iotc-f9199991-ceb1-4f38-9f1c-13199992570e.azure-devices.net"
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

1. The WLAN LED will blink every 5 seconds when connected to Azure.
1. The RGB LED will turn red if the measured temperature is less than the desired temperature, blue if the measured temperature is greater than the desired temperature and green if the measured temperature is the same as the desired temperature.

### Seeed Studio Azure Sphere MT3620 Development Kit

![](resources/seeed-studio-azure-sphere-rdb.jpg)

1. The WLAN LED will blink every 5 seconds when connected to Azure.
1. The RGB LED will turn red if the measured temperature is less than the desired temperature, blue if the measured temperature is greater than the desired temperature and green if the measured temperature is the same as the desired temperature.

### Seeed Studio MT3620 Mini Dev Board

![](resources/seeed-studio-azure-sphere-mini.png)

1. The User LED will blink every 5 seconds when connected to Azure.

---

## Display the actual temperature property

1. Switch to Azure IoT Central in your browser.
2. Select the **Devices**, then the **Azure Sphere** template, then your device.
3. Select the **Form** tab.


    >Note, Azure IoT Central does not update immediately. It might take a minute or two for the device telemetry to be displayed.


![](resources/iot-central-display-telemetry.png)

## Set the desired temperature

Setting the desired temperature is like setting a thermostat in a room. A desired temperature device twin property message is sent to the Azure Sphere where it will be acted upon.

When the temperature sensor is read on the Azure Sphere it is compared to the desired temperature.

* If the temperature is greater than the desired temperature the RGB LED will turn blue to indicate the cooler needs be turned on.
* If the temperature is less than the desired temperature then the RGB LED will turn red to indicate the heater needs to be turned on.
* If the temperature is the same as the desired temperature then the RGB LED turns green to indicate no action required.

> The AVNET Azure Sphere Starter Kit has a built-in temperature sensor. For other boards, a random temperature 25 +/- 5 degrees celsius is generated.

To set the desired temperature:

1. Update the **Desired Temperature** value.
2. Save the Property.
    This will send the desired temperature property to the Azure Sphere. The RGB LED colour will change depending on the desired temperature chosen.

![iot central device settings](resources/iot-central-display-settings.png)

---

## Close Visual Studio

Now close **Close Visual Studio**.

---

## Finished 已完成 fertig 完了 finito समाप्त terminado

Congratulations you have finished lab 3.

![](resources/finished.jpg)

---

**[NEXT](../Lab_4_Control_Device_with_Direct_Methods/README.md)**

---
