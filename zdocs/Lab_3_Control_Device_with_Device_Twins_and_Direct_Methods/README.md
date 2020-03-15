# Lab 3: Control an Azure Sphere with Device Twins and Direct Methods

<!-- ![](resources/azure-sphere-iot-central-banner.png) -->

---

|Author|[Dave Glover](https://developer.microsoft.com/en-us/advocates/dave-glover?WT.mc_id=github-blog-dglover), Microsoft Cloud Developer Advocate, [@dglover](https://twitter.com/dglover) |
|:----|:---|
|Source Code | https://github.com/gloveboxes/Azure-Sphere-Learning-Path.git |
|Date| March 2020|

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

You will learn how to control an [Azure Sphere](https://azure.microsoft.com/services/azure-sphere/?WT.mc_id=github-blog-dglover) application using [Azure IoT Central](https://azure.microsoft.com/services/iot-central/?WT.mc_id=github-blog-dglover) Properties ([Azure IoT Hub Device Twins](https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-devguide-device-twins?WT.mc_id=github-blog-dglover)) and Commands ([Azure IoT Hub Direct Methods](https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-devguide-direct-methods?WT.mc_id=github-blog-dglover)).

---

## Prerequisites

This lab assumes you have completed [Lab 2: Send Telemetry from an Azure Sphere to Azure IoT Central](../Lab_2_Send_Telemetry_to_Azure_IoT_Central/README.md). You will have created an Azure IoT Central application, connected Azure IoT Central to your Azure Sphere Tenant and you have configured the **app_manifest.json** for the Azure Device Provisioning Service.

You will need to **copy** and **paste** the Lab 2 **app_manifest.json** you created to this lab's **app_manifest.json** file.

---

## Tutorial Overview

There are three ways that Azure IoT cloud applications can communicate with devices. 

[Direct Methods](https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-devguide-direct-methods) for communications that require immediate confirmation of the result. Direct methods are often used for interactive control of devices such as turning on a fan.

[Device Twins](https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-devguide-device-twins) for long-running commands intended to put the device into a certain desired state. For example, set the telemetry send interval to 30 minutes.

[Cloud-to-device](https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-devguide-messages-c2d) messages for one-way notifications to the device app.

This lab will cover Azure Iot Device Twins and Direct Methods.

---

## Key Concepts

In Lab 1, **Peripherals** and **Timers** were introduced to simplify and effectively describe GPIO pins and Timers and their interactions.

In this lab, **DeviceTwinBindings**, and **DirectMethodBindings** are introduced to simplify the implementation of Azure IoT [Device Twins](https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-devguide-device-twins?WT.mc_id=github-blog-dglover) and Azure IoT [Direct Methods](https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-devguide-direct-methods?WT.mc_id=github-blog-dglover) on the device.

Both Device Twins and Direct Methods provide a mechanism to invoke functionality on a device from a custom Azure IoT Hub application or from an Azure IoT Central Application. For example, you may want to turn on a light, start a fan, or change the rate a sensor is sampled.

### Azure IoT Device Twins

When you set a Device Twin property in Azure IoT you are setting the *desired* state of a property on the device. Azure IoT will send a desired state message to the device, the device then actions the request, for example, turn on a LED, and the device will then send a *reported* state message back to Azure IoT. Azure IoT then stores the *reported* state in the Azure IoT.

Azure IoT Central uses this *reported* state to display the last synced state of a property.

For more information refer to the [Understand and use device twins in IoT Hub](https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-devguide-device-twins?WT.mc_id=github-blog-dglover) article.

### Azure IoT Direct Methods

Azure IoT Direct Methods are simpler. When you invoke a *Direct Method* from Azure, a message is sent to the device. This message includes the name of the direct method and a data payload. The device will action the request and then respond with an HTTP status code to indicate success or failure along with an optional message.

For more information refer to the [Understand and invoke direct methods from IoT Hub](https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-devguide-direct-methods?WT.mc_id=github-blog-dglover) article.

---

## Device Twin Bindings

In **main.c** there is a variable declared named **light** of type **DeviceTwinBinding**. Variables of type **DeviceTwinBinding** declare a generalized model to define the relationship between an Azure IoT Device Twin and optionally a Peripheral.

The following example associates an Azure IoT Device Twin named **led1**, of type **TYPE_BOOL**, with a GPIO **peripheral**, that will invoke a **handler** function named **DeviceTwinHandler**.

The handler will be called when the Device Property is updated in Azure IoT Central, and an LED will be turned on or off.

```c
static DeviceTwinBinding light = {
	.peripheral = {
		.fd = -1, .pin = LIGHT_PIN, .initialState = GPIO_Value_High, .invertPin = true, .initialise = OpenPeripheral, .name = "led1" },
	.twinProperty = "led1",
	.twinType = TYPE_BOOL,
	.handler = DeviceTwinHandler
};
```

This maps to the **led1** _property_ of _schema type_ **Boolean** defined in the Azure IoT Central Device template.

![](resources/iot-central-device-template-interface-led1.png)

---

You can also define a DeviceTwinBinding without a Peripheral. The example below associates an Azure IoT Device Twin with a handler. This handler might do something like change the sampling rate for a sensor or set a threshold on the device.

```c
static DeviceTwinBinding sensorSampleRate = {
	.twinProperty = "sensorsamplerate",
	.twinType = TYPE_INT,
	.handler = SensorSampleRateTwinHandler
};
```

Like Peripherals and Timers, Device Twin Bindings can be automatically opened, initialized, and closed if they are added to the deviceTwinDevices array, also referred to as a **set** of device twin bindings.

```c
DeviceTwinBinding* deviceTwinBindings[] = { &light, &sensorSampleRate };
```

---

## Direct Method Bindings

```c
static DirectMethodBinding feedFish = {
    	.peripheral = { .fd = -1, .pin = FEED_FISH_PIN, .initialState = GPIO_Value_High, .invertPin = true, .initialise = OpenPeripheral, .name = "feedfish" },
	.methodName = "feedfish",
	.handler = FeedFishDirectMethod
};

```

You will find the following example in **main.c**.

```c
static DirectMethodBinding fan = {
	.methodName = "fan1",
	.handler = SetFanSpeedDirectMethod
};
```

This maps to the **fan1** _command_ defined in the Azure IoT Central Device template.

![](resources/iot-central-device-template-interface-fan1.png)

### Setting DirectMethodBindings

Like Peripherals, Timers, Device Twin Peripherals, Direct Method Bindings can be automatically opened, initialized, and closed if they are added to the directMethodDevices array, also referred to as a set of direct method bindings.

```c
DirectMethodBinding* directMethodBinding[] = { &feedFish, &SetFanSpeedDirectMethod };
```

---

## Open Lab 3

### Step 1: Start Visual Studio 2019

![](resources/visual-studio-open-local-folder.png)

### Step 2: Open the lab project

1. Click **Open a local folder**
2. Open the Azure-Sphere lab folder
3. Open the **folder name** that corresponds to the **Azure Sphere board** you are using
4. Open the **Lab_3_Control_Device_with_Device_Twins_and_Direct_Methods** folder
5. Click **Select Folder** button to open the project

    <!-- ![](resources/visual-studio-open-lab3.png) -->

### Step 3: Configure the Azure IoT Central Connection Information

1. Open the **app_manifest.json** file
2. You will need to redo the settings for the **app_manifest.json** file. Either copy from **Notepad** if you still have open or copy from the **app_manifest.json** file you created in lab 2.

    ![](resources/visual-studio-open-app-manifest.png)

3. Update the contents of this **app_manifest.json**.

---

## Support for Azure IoT Central Properties

1. Open the **main.c** file
2. Scroll down to the line that reads **static DeviceTwinBinding relay**
    ```c
    static DeviceTwinBinding relay = {
        .peripheral = {
            .fd = -1, .pin = RELAY_PIN, .initialState = GPIO_Value_Low, .invertPin = false, .initialise = OpenPeripheral, .name = "relay1" },
        .twinProperty = "relay1",
        .twinType = TYPE_BOOL,
        .handler = DeviceTwinHandler
    };
    ```
3. This data structure describes the Device Twin. Defined is the Azure IoT Central Device **Property** this peripheral is associated with, the data type, and the **handler**, in this case, named **DeviceTwinHandler**. The handler is the name of the C function that will be called when the device receives a property update from Azure IoT Central.
4. Right mouse click on **DeviceTwinHandler**, and select **Go To Definition**.
    ![](resources/visual-studio-go-to-definition.png)
5. This will jump you to the function named **DeviceTwinHandler**. Review the implementation of the handler.

    ```c
    static void DeviceTwinHandler(DeviceTwinBinding* DeviceTwinBinding) {
        switch (DeviceTwinBinding->twinType)
        {
        case TYPE_BOOL:
            if (*(bool*)DeviceTwinBinding->twinState) {
                GPIO_ON(DeviceTwinBinding->peripheral);
            }
            else {
                GPIO_OFF(DeviceTwinBinding->peripheral);
            }
            break;
        case TYPE_INT:
            Log_Debug("\nInteger Value '%d'\n", *(int*)DeviceTwinBinding->twinState);
            // Your implementation goes here - for example change the sensor measure rate
            break;
        case TYPE_FLOAT:
            Log_Debug("\nFloat Value '%f'\n", *(float*)DeviceTwinBinding->twinState);
            // Your implementation goes here - for example set a threshold
            break;
        case TYPE_STRING:
            Log_Debug("\nString Value '%s'\n", (char*)DeviceTwinBinding->twinState);
            // Your implementation goes here - for example update display
            break;
        default:
            break;
        }
    }
    ```

### Support for Azure IoT Central Commands

1. Again, in the **main.c** file
2. Scroll up to around line 40. The line reads **static DirectMethodBinding fan**.
3. This data structure describes a generalized peripheral and what Azure IoT Central Device **Command** this peripheral is associated with.  Azure IoT Central device commands are implemented as Azure IoT Direct Methods.

    ```c
    static DirectMethodBinding fan = {
        .methodName = "fan1",
        .handler = SetFanSpeedDirectMethod
    };
    ```

4. Again, right mouse click the **SetFanSpeedDirectMethod** handler, and select **Go To Definition**, and review the handler implementation.

    ```c
    static MethodResponseCode SetFanSpeedDirectMethod(JSON_Object* json, DirectMethodBinding* directMethodBinding) {
        // Sample implementation - doesn't do anything other than returning a response message and status

        // Allocate and initialize a response message buffer. The calling function is responsible for the freeing memory
        const size_t responseLen = 40;
        directMethodBinding->responseMessage = (char*)malloc(responseLen);
        memset(directMethodBinding->responseMessage, 0, responseLen);

        int speed = (int)json_object_get_number(json, "speed");

        if (speed >= 0 && speed <= 100) {
            snprintf(directMethodBinding->responseMessage, responseLen, "%s succeeded, speed set to %d", directMethodBinding->methodName, speed);
            Log_Debug("\nDirect Method Response '%s'\n", directMethodBinding->responseMessage);
            return METHOD_SUCCEEDED;
        }
        else
        {
            snprintf(directMethodBinding->responseMessage, responseLen, "%s FAILED, speed out of range %d", directMethodBinding->methodName, speed);
            Log_Debug("\nDirect Method Response '%s'\n", directMethodBinding->responseMessage);
            return METHOD_FAILED;
        }
    }
    ```

### Support for IoT Central Properties and Commands

1. Again, in **main.c**.
2. Scroll up to the line that reads **#pragma region define sets for auto initialization and close**
3. In this region, there are several arrays that point to the **DeviceTwinBindings** and **DirectMethodBinding** defined above.
    ```c
    DeviceTwinBinding* deviceTwinBindings[] = { &relay, &light };
    DirectMethodBinding* directMethodBindings[] = { &fan };
    ```
4. In the main.c **InitPeripheralsAndHandlers** function these sets of device twins and direct methods are opened and initialized.
    ```c
    static int InitPeripheralsAndHandlers(void)
    {
        InitializeDevKit();  // Avnet Starter Kit

        OpenPeripheralSet(peripherals, NELEMS(peripherals));
        OpenDeviceTwinSet(deviceTwinBindings, NELEMS(deviceTwinBindings));
        OpenDirectMethodSet(directMethodBindings, NELEMS(directMethodBindings));

        StartTimerSet(timers, NELEMS(timers));

        StartCloudToDevice();

        return 0;
    }
    ```

---

## Visual Studio Deployment Settings

Before building the application with Visual Studio ensure ARM-Debug and GDB Debugger (HLCore) options are selected.

![](resources/visual-studio-start-config.png)

---

## Build, Deploy and start Debugging

To start the build, deploy and debug process, either click the Visual Studio **Start Selected Item** icon or press <kbd>**F5**</kbd>. To Build and deploy without attaching the debugger, press <kbd>**Ctrl+F5**</kbd>.

![](resources/visual-studio-start-debug.png)

---

## Test Azure IoT Central Property Updates

Now the application is running on the Azure Sphere switch across to Azure IoT Central, select the **Devices** tab, then the **Azure Sphere** template, then the actual device.

Select the **Properties** tab and change the **Light** toggle state to **On**, then click **Save**.

![iot central device settings](resources/iot-central-display-settings.png)

### Expected Behaviour

The expected behaviour will differ depending on what Azure Sphere device you have.

* **Azure Sphere MT3620 Starter Kit**: See that an **Orange** LED lights up on the Azure Sphere.
* **Seeed Studio Azure Sphere MT3620 Development Kit**: See that a **Red** LED lights up on the Azure Sphere.
* **Seeed Studio Azure Sphere MT3620 Mini Dev Board**: See that a **Green** LED lights up on the Azure Sphere.

---

## Test Azure IoT Central Commands

1. With the application still is running on the Azure Sphere, select the Azure IoT Central **Commands** tab.
2. Set the fan speed to 30 and click **Run**.
    ![](resources/iot-central-device-command-run.png)
3. Click the **View History** button
    ![](resources/iot-central-device-command-view-history.png)
4. The command history will be similar to the following:
    ![](resources/iot-central-device-commands-view-history.png)
5. Try testing with numbers great that 100, and review the command history.
6. From Visual Studio, try setting a breakpoint in the **SetFanSpeedDirectMethod** handler and then from Azure IoT Central rerun the command. You should see that the Visual Studio debugger has halted the code execution in the handler.

---

## Close Visual Studio

Now close **Close Visual Studio**.

---

## Finished 完了 fertig finito ख़त्म होना terminado

Congratulations you have finished lab 3.

![](resources/finished.jpg)

---

**[NEXT](../Lab_4_FreeRTOS_and_Inter-Core_Messaging/README.md)**

---
