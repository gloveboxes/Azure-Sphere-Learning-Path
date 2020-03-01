# Lab 1: Build your first Azure Sphere Application with Visual Studio

<!-- ![Azure Sphere with shield](resources/azure-sphere-shield.png) -->

---

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

## What you will learn

In this lab you will learn how to secure, build, deploy, and debug your first High-Level (HL) Azure Sphere application onto the Azure Sphere A7 Core.

---

## Prerequisites

This lab assumes you have completed **Lab 0: Lab set up, installation and configuration**

---

## Tutorial Overview

1. Understand Peripheral and Timer concepts.
2. Open the lab project with Visual Studio
3. Review the High-Level (A7) Core application security requirements.
4. Understand device manufacture pin mappings.
5. Application deployment and debugging

---

## Key Concepts

Lab 1 introduces two data structures used to greatly simplify and effectively describe in  code how [GPIO](https://en.wikipedia.org/wiki/General-purpose_input/output) and Timers will work.

### Peripherals

In **main.c** there is a variable named **builtinLed** of type **Peripheral**. Variables of type **Peripheral** declare a generalized GPIO **output** peripheral.

This variable holds the Operating System file descriptor, the GPIO pin number, the initial state of the pin when it is opened, whether the pin logic needs to be inverted for turning a pin on and off, and the C Function to be called to open and initialize the GPIO output pin.

```c
static Peripheral builtinLed = {
	.fd = -1, // The OS reference to the GPIO pin
	.pin = BUILTIN_LED, // The GPIO pin number
	.initialState = GPIO_Value_High,  // Set the initial state on the pin when opened
	.invertPin = true,  // Should the switching logic be reverse for on/off, high/low
	.initialise = OpenPeripheral,  // The name of C function to be called to open the Pin. The OpenPeripheral implementation is provided in peripheral.c.
	.name = "SendStatus"  // An arbitrary name for the sensor.
};

```

### Timers

In **main.c** there is another variable named **measureSensorTimer** of type **Timer**. Variables of type **Timer** declare a generalized Timer object. A Timer is a regularly occurring event. For example, you may want to blink an LED every second, or perhaps read data from a sensor every 5 seconds.

```c
static Timer measureSensorTimer = {
	.period = { 5, 0 },  // Fire the timer event every 5 seconds + zero nanoseconds.
	.name = "MeasureSensor",  // An arbitrary name for the timer, used for error handling
	.timerEventHandler = MeasureSensorHandler // The address of the C function to be called when the timer fires.
};
```

In this example, the function named **MeasureSensorHandler** is called every 5 seconds. There are two values used to initialize the **.period** variable, the first is the number of seconds, followed by the number of nanoseconds.

If you wanted the timer to fire every half a second (500 milliseconds), you would set the .period to be { 0, 500000000 }

### Automatic Initialization of Peripherals and Timers

The peripherals and timers array variables are initialized with the addresses of the peripheral and timer objects that are declared above.

```c
Peripheral* peripherals[] = { &builtinLed };
Timer* timers[] = { &measureSensorTimer };
```

Peripherals and timers are added to their respective arrays are referred to as **sets**. Any peripheral or timer referenced in a set will be automatically opened, initialized, and closed.

These sets are referenced when calling **OpenPeripheralSet**, and **StartTimerSet** from the **InitPeripheralsAndHandlers** function. The sets are also referenced when closing the peripheral and timer sets in the **ClosePeripheralsAndHandlers** function.

```c
static int InitPeripheralsAndHandlers(void)
{
	InitializeDevKit();  // Avnet Starter kit

	OpenPeripheralSet(peripherals, NELEMS(peripherals));
	StartTimerSet(timers, NELEMS(timers));

	return 0;
}
```

The **measureSensorTimer** timer is called every 5 seconds.

```c
static void MeasureSensorHandler(EventLoopTimer* eventLoopTimer)
{
	if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0) {
		Terminate();
		return;
	}

	GPIO_ON(builtinLed); // blink built in LED

	if (ReadTelemetry(msgBuffer, JSON_MESSAGE_BYTES) > 0) {
		Log_Debug("%s\n", msgBuffer);
	}

	GPIO_OFF(builtinLed);
}
```

### Easy to Extend

This model makes it easy to declare another peripheral or timer and add them to the **peripherals** or **timers** arrays. The following is an example of adding a GPIO output peripheral.

```c
static Peripheral fanControl = {
	.fd = -1, // The OS reference to the GPIO pin
	.pin = 43, // The GPIO pin number
	.initialState = GPIO_Value_High,  // Set the initial state on the pin when opened
	.invertPin = true,  // Should the switching logic be reverse for on/off, high/low
	.initialise = OpenPeripheral,  // The name of C function to be called to open the Pin. The OpenPeripheral implementation is provided in peripheral.c.
	.name = "FanControl"  // An arbitrary name for the senor.
};

```

Remember to add this new peripheral to the **peripherals** set so it will be automatically opened, initialized, and closed.

```c
Peripheral* peripherals[] = { &builtinLed, &fanControl };
```

---

## Enable Azure Sphere Device Development

You will need the credentials for the Azure Sphere Tenant your device was claimed into.

If you are using a lab device, then the lab instructor will provide you with the credentials.

### Login to your Azure Sphere Tenant

1. Start the **Azure Sphere Developer Command Prompt**
	1. Press the <kbd>Windows Key</kbd>
	2. Start typing **Azure Sphere Developer Command Prompt**
	3. Select and start the **Azure Sphere Developer Command Prompt**.
2. **Login to the Azure Sphere Tenant**

	From the **Azure Sphere Developer Command Prompt**, run ```azsphere login```. You will be prompted for the tenant credentials.
3. **List Azure Sphere Tenants**

	From the **Azure Sphere Developer Command Prompt**, run ```azsphere tenant list```.
4. Select the **default Azure Sphere Tenant**
	If there is more than one tenant listed then you need to set the default tenant.
	
	From the **Azure Sphere Developer Command Prompt**, run ```azsphere tenant select -i <Tenant ID>```.

---

## Enable Azure Sphere Device Debugging

From the **Azure Sphere Developer Command Prompt**, run ```azsphere device enable-development``` to enable local debugging.

---

## Open Lab 1

### Step 1: Ensure you have cloned the lab source code

Follow the [lab set-up guide](../Lab_0_Introduction_and_Lab_Set_Up/#step-2-clone-the-azure-sphere-learning-path) if you have not yet cloned the labs to your computer.

### Step 2: Start Visual Studio 2019

![](resources/visual-studio-open-local-folder.png)

### Step 3: Open the lab project

1. Click **Open a local folder**
2. Open the Azure-Sphere lab folder
3. Open the **folder name** that corresponds to the **Azure Sphere board** you are using
4. Open the **Lab_1_Visual_Studio_and_Azure_Sphere** folder
5. Click **Select Folder** button to open the project

![](resources/visual-studio-open-project.png)

### Step 4: Verify the Project Opened Correctly

From the **Solution Explorer**, open the **main.c** file.

![](resources/visual-studio-open-main.png)

---

### Check CMake Cache Builds Correctly

The generation of the CMake cache runs automatically when you open a CMake project. But given this is the first lab to be opened it is a good idea to rerun the **CMake Cache Generator** to check that it is building correctly.

1. Right mouse click the **CMakeLists.txt** file and select **Generate Cache for AzureSphereIoTCentral**.

	![](resources/visual-studio-cmake-generate.png)

2. Check the **Output** window that the CMake generation was successful. There should be a message to say **CMake generation finished**.

	![](resources/visual-studio-cmake-generate-completed.png)

	If the CMake Generation fails, then there is a good chance the lab has been cloned to a directory with a long file path and name. This can be a problem for CMake. Try moving the labs closer to the root directory on your local drive.

---

## Understanding Azure Sphere Security

Applications on Azure Sphere are locked down by default and you must grant capabilities to the application. This is key to Azure Sphere security and is also known as the [Principle of least privilege](https://en.wikipedia.org/wiki/Principle_of_least_privilege).

Application capabilities include what hardware can be accessed, what internet services can be called (including Azure IoT Central and the Azure Device Provisioning Service), and what inter-core communications are allowed.

### Open the Application Manifest File

From Visual Studio open the **app_manifest.json** file.

![](resources/visual-studio-application-capabilities.png)

This application can only access the resources listed in the **Capabilities** section.

```json
  "Capabilities": {
    "Gpio": [ "$AVNET_MT3620_SK_GPIO0", "$AVNET_MT3620_SK_APP_STATUS_LED_YELLOW", "$AVNET_MT3620_SK_WLAN_STATUS_LED_YELLOW" ],
    "Uart": [],
    "I2cMaster": [ "$AVNET_MT3620_SK_ISU2_I2C" ],
    "Adc": [ "$AVNET_MT3620_SK_ADC_CONTROLLER0" ]
  },
```

## Understand Pin Mappings

Each Azure Sphere manufacturer maps pins differently.  To understand how the pins are mapped for your developer board then follow these steps.

1. Ensure you have the **main.c** file open. Place the cursor on the line that reads **#include "../shared/oem/board.h"**, then press <kbd>F12</kbd>. This will open the **board.h** header file.

	![](resources/visual-studio-open-board.png)

	Review the pin mappings that are being used for the lab. These will vary depending on what developer kit you are using.

	```c
	#define BUILTIN_LED		AVNET_MT3620_SK_APP_STATUS_LED_YELLOW
	#define LIGHT_PIN		AVNET_MT3620_SK_WLAN_STATUS_LED_YELLOW
	#define RELAY_PIN		AVNET_MT3620_SK_GPIO0
	```

2. Review the manufactures pin mappings. From the **board.h** file, place the cursor on the line that starts with **#include "../Hardware/**, and press <kbd>F12</kbd>. This will open the manufacturer's board definition header file. In this file, you can see how the pins are mapped.

3. Next, click on the **main.c tab** to bring **main.c** to the foreground.

	![](resources/visual-studio-open-main-tab.png)

---

## Deploy the Application to the Azure Sphere

1. Connect the Azure Sphere to your computer via USB
2. Ensure you have [claimed](https://docs.microsoft.com/en-au/azure-sphere/install/claim-device?WT.mc_id=github-blog-dglover), [connected](https://docs.microsoft.com/en-au/azure-sphere/install/configure-wifi?WT.mc_id=github-blog-dglover), and [developer enabled](https://docs.microsoft.com/en-au/azure-sphere/install/qs-blink-application?WT.mc_id=github-blog-dglover) your Azure Sphere.
3. Ensure you have enabled developer mode on the Azure Sphere
4. Select **GDB Debugger (HLCore)** from the **Select Startup** dropdown.
	![](resources/vs-start-application.png)
5. From Visual Studio, press <kbd>F5</kbd> to build, deploy, start, and attached the remote debugger to the Azure Sphere.

---

## View the Debugger Output

Open the **Output** window to view the output from **Log_Debug** statements in the code.

You can open the output window by using the Visual Studio <kbd>Ctrl+Alt+O</kbd> shortcut or click the **Output** tab found along the bottom/right of Visual Studio.

![Visual Studio View Output](resources/vs-view-output.png)

---

## Set a Debugger Breakpoint

Set a debugger breakpoint by clicking in the margin to the left of the line of code you want the debugger to stop at.

1. In the **main.c** file, scroll down until you find the C function named **MeasureSensorHandler**. 

2. Set a breakpoint in the margin of the line that reads **ConsumeEventLoopTimerEvent**.

	![](resources/vs-set-breakpoint.png)

3. When the next timer event fires, the debugger will stop at the line you set the breakpoint on.
4. You can inspect variable values, step over code <kbd>F10</kbd>, step into code <kbd>F11</kbd>, and continue code execution <kbd>F5</kbd>. 
5. For more information on debugging then read [First look at the Visual Studio Debugger](https://docs.microsoft.com/en-us/visualstudio/debugger/debugger-feature-tour?view=vs-2019&WT.mc_id=github-blog-dglover)

---

## Stop the Debugger

**Stop** the debugger by using the Visual Studio <kbd>Shift+F5</kbd> keyboard shortcut or click the **Stop Debugging** icon.

![](resources/vs-stop-debugger.png)

---

## Finished 完了 fertig finito ख़त्म होना terminado

Congratulations, you secured, built, deployed and debugged your first Azure Sphere application.

![](resources/finished.jpg)

---

**[NEXT](../Lab_2_Send_Telemetry_to_Azure_IoT_Central/README.md)**
