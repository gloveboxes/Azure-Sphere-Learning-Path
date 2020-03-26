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

In Lab 1, **Peripherals** and **Event Timers** were introduced to simplify and effectively describe GPIO pins and Timers and their interactions.

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

In **main.c** there is a variable declared named **led1BlinkRate** of type **DeviceTwinBinding**. Variables of type **DeviceTwinBinding** declare a generalized model to define the relationship between an Azure IoT Device Twin and a handler function.

### Declaring a Device Twin Binding

The following example associates an Azure IoT Device Twin named **LedBlinkRate**, of type **TYPE_INT**, that will invoke a **handler** function named **DeviceTwinBlinkRateHandler**.

When the Device Property is updated in Azure IoT Central, the handler function will be called and LED1 blink rate will be changed.

```c
static DeviceTwinBinding led1BlinkRate = { .twinProperty = "LedBlinkRate", .twinType = TYPE_INT, .handler = DeviceTwinBlinkRateHandler };
```

### Device Twin Binding Handler Functions

This is the implementation on the **DeviceTwinBlinkRateHandler**. The handler function is called when the device receives a Device Twin desired state message for the **LedBlinkRate** property from Azure IoT Central.

```c
/// <summary>
/// Set Blink Rate using Device Twin "LedBlinkRate": {"value": 0}
/// </summary>
static void DeviceTwinBlinkRateHandler(DeviceTwinBinding* deviceTwinBinding) {
	switch (deviceTwinBinding->twinType) {
	case TYPE_INT:
		Log_Debug("\nInteger Value '%d'\n", *(int*)deviceTwinBinding->twinState);

		Led1BlinkIntervalIndex = *(int*)deviceTwinBinding->twinState % led1BlinkIntervalsCount;
		ChangeTimer(&led1BlinkTimer, &led1BlinkIntervals[Led1BlinkIntervalIndex]);

		break;
	case TYPE_BOOL:
		Log_Debug("\nBoolean Value '%d'\n", *(bool*)deviceTwinBinding->twinState);
		// Your implementation goes here - for example turn in light
	case TYPE_FLOAT:
		Log_Debug("\nFloat Value '%f'\n", *(float*)deviceTwinBinding->twinState);
		// Your implementation goes here - for example set a threshold
		break;
	case TYPE_STRING:
		Log_Debug("\nString Value '%s'\n", (char*)deviceTwinBinding->twinState);
		// Your implementation goes here - for example update display
		break;
	default:
		break;
	}
}
```

### Mapping Azure IoT Central Interface Properties with Device Twin Bindings

This maps to the **LedBlinkRate** _property_ of _schema type_ **Integer** defined in the Azure IoT Central Device template.

![](resources/iot-central-device-template-interface-led1.png)

### Device to Cloud Device Twin Bindings

You can also define a one-way Device to Cloud update DeviceTwinBinding. The following example binds the Device Twin property ButtonPressed. Note, there is no handler function as this is a one-way binding only.

```c
static DeviceTwinBinding buttonPressed = { .twinProperty = "ButtonPressed", .twinType = TYPE_STRING };
```

To update the Device Twin, call the DeviceTwinReportState function. You must pass in a property of the correct type, in this case **TYPE_STRING** (aka char*). This example can be found in the **ButtonPressCheckHandler** handler function. 

```c
DeviceTwinReportState(&buttonPressed, "ButtonA");   // TwinType = TYPE_STRING
```

### Initializing Device Twin Bindings

Like Peripherals and Timers, Device Twin Bindings can be automatically opened, initialized, and closed if they are added to the deviceTwinDevices array, also referred to as a **set** of device twin bindings.

```c
DeviceTwinBinding* deviceTwinBindings[] = { &led1BlinkRate, &buttonPressed, &relay1DeviceTwin };
```

---

## Direct Method Bindings

Direct methods represent a request-reply interaction with a device similar to an HTTP call in that they succeed or fail immediately (after a user-specified timeout).

When you invoke a *Direct Method* from Azure IoT, a message is sent to the device. This message includes the name of the direct method and a payload. The device will action the request and then respond with an HTTP status code to indicate success or failure along with an optional message.

### Declaring a Direct Method Binding

Direct Method Bindings associate Azure IoT Direct Methods with a handler function. In the following example, when the device receives Azure IoT Direct Method named **ResetMethod**, the **ResetDirectMethod** handler function will be called.

```c
static DirectMethodBinding resetDevice = { .methodName = "ResetMethod", .handler = ResetDirectMethod };
```

### Direct Method Handler Function

The **ResetDirectMethod** handler function found in **main.c** implements the DirectMethodBinding. The function is passed a JSON object *{"reset_timer":5}*, this is deserialized, range checked, a One Shot Timer is set to do the device reset. This leaves enough time for the application to respond to Azure IoT Central with a response message and an HTTP status code before resetting.

```c
/// <summary>
/// Start Device Power Restart Direct Method 'ResetMethod' {"reset_timer":5}
/// </summary>
static DirectMethodResponseCode ResetDirectMethod(JSON_Object* json, DirectMethodBinding* directMethodBinding, char** responseMsg) {
	const char propertyName[] = "reset_timer";
	const size_t responseLen = 60; // Allocate and initialize a response message buffer. The calling function is responsible for the freeing memory
	static struct timespec period;

	*responseMsg = (char*)malloc(responseLen);
	memset(*responseMsg, 0, responseLen);

	if (!json_object_has_value_of_type(json, propertyName, JSONNumber)) {
		return METHOD_FAILED;
	}

	int seconds = (int)json_object_get_number(json, propertyName);

	if (seconds > 1 && seconds < 10) {

		period = (struct timespec){ .tv_sec = seconds, .tv_nsec = 0 };
		SetOneShotTimer(&resetDeviceOneShotTimer, &period);

		snprintf(*responseMsg, responseLen, "%s called. Reset in %d seconds", directMethodBinding->methodName, seconds);
		return METHOD_SUCCEEDED;
	}
	else {
		snprintf(*responseMsg, responseLen, "%s called. Reset Failed. Seconds out of range: %d", directMethodBinding->methodName, seconds);
		return METHOD_FAILED;
	}
}
```

### Mapping Azure IoT Central Interface Command with Direct Method Bindings

This maps to the **ResetMethod** command defined in the Azure IoT Central Device template.

![](resources/iot-central-device-template-interface-fan1.png)

#### ResetMethod Object Schema

The **ResetMethod** schema is of type **Object**. Clicking on the **view** button will display the object definition. The Object definition describes the shape of the JSON payload sent when the Direct Method is invoked.

The **ResetMethod** handler function is expecting a JSON payload like this {"reset_timer":5}.

![](resources/iot-central-device-template-interface-command-schema.png)

### Initializing and Closing Direct Method Bindings

Like Peripherals, Timers, and Device Twins, Direct Method Bindings can be automatically initialized and closed if they are added to the directMethodDevices array. An array of direct method bindings is also referred to as a set of direct method bindings.

```c
DirectMethodBinding* directMethodBindings[] = { &resetDevice };
```

The Direct Method Bindings are initialized in the **InitPeripheralsAndHandlers** function found in **main.c**.

```c
OpenDirectMethodSet(directMethodBindings, NELEMS(directMethodBindings));
```

The Direct Method Bindings are closed in the **ClosePeripheralsAndHandlers** function found in **main.c**.

```c
CloseDirectMethodSet();
```

---

## Open Lab 3

### Step 1: Start Visual Studio 2019

![](resources/visual-studio-open-local-folder.png)

### Step 2: Open the lab project

1. Click **Open a local folder**
2. Open the Azure-Sphere lab folder
3. Open the **folder name** that corresponds to the **Azure Sphere board** you are using
4. Open the **Lab_3_Cloud_to_Device_Control_Twins_Methods** folder
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
