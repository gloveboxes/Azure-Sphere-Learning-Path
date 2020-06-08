# Azure Sphere Learning Path

Lab 4: [Integrating FreeRTOS with Azure Sphere Inter-Core Messaging](../../zdocs/Lab_4_FreeRTOS_and_Inter-Core_Messaging/README.md)

<!-- 
## Azure Sphere RTcore FreeRTOS demo

This sample demonstrates the latest freertos 10.2.1 port on Azure Sphere Realtime core (Cortex-M4F). It performs the same function as the official [GPIO_RTApp_MT3620_BareMetal]([../GPIO_HighLevelApp/README.md](https://github.com/Azure/azure-sphere-samples/tree/master/Samples/GPIO/GPIO_RTApp_MT3620_BareMetal)) example.

- It provides access to one of the LEDs on the MT3620 development board using GPIO.
- It uses a button to change the blink rate of the LED.


To use this sample, clone the repository locally if you haven't already done so:

```
git clone https://github.com/xiongyu0523/azure-sphere-rtcore-freertos.git
```

## FreeRTOS port NOTE

1. The Cortex-M4F core is running at 26MHz out of reset, the example boost core frequency to 197.6MHz before we start FreeRTOS kernel. 
   
   ```
   uint32_t val = ReadReg32(IO_CM4_RGU, 0);
   val &= 0xFFFF00FF;
   val |= 0x00000200;
   WriteReg32(IO_CM4_RGU, 0, val);
   ```
   
2. To reuse the official GCC Cortex-M4F port, need modify the gcc compiler flag to use FPU instructions. Just copy the *AzureSphereRTCoreToolchainVFP.cmake* file into the Azure Sphere SDK install folder. (Default path is *C:\Program Files (x86)\Microsoft Azure Sphere SDK\CMakeFiles*)

    The underlayer work is to replace line 45:

    `SET(CMAKE_C_FLAGS_INIT "-mcpu=cortex-m4")`

    with

    `SET(CMAKE_C_FLAGS_INIT "-mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16")`

3. Cortex-m4F core in Azure Sphere implement 8 interrupt priority levels in 3-bits. In FreeRTOSConfig.h, below macros reflect the priority setting.

    ```
    #define configPRIO_BITS       		                3        
    #define configLIBRARY_LOWEST_INTERRUPT_PRIORITY		7
    ```

4. In FreeRTOSConfig.h, set the interrupt priority 2 as the highest priority that can call FreeRTOS interrupt-safe API. (Interrupt ISR set to priority number 0 and 1 are not allowed to do so)
   
   ```
   #define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY 2
   ```


5. The most important factor of verfiying a RTOS port is to test context switching at both task level and interrupt level. The example code have the minimum converage for both case using semaphore and delay API.

## To build and run the sample

### Prep your device

1. Ensure that your Azure Sphere device is connected to your PC, and your PC is connected to the internet.
2. Even if you've performed this set up previously, ensure that you have Azure Sphere SDK version 19.09. In an Azure Sphere Developer Command Prompt, run **azsphere show-version** to check. Download and install the [latest SDK](https://aka.ms/AzureSphereSDKDownload) as needed.
3. Right-click the Azure Sphere Developer Command Prompt shortcut and select **More > Run as administrator**.
4. At the command prompt issue the following command:

   ```
   azsphere dev prep-debug --enablertcoredebugging
   ```

   This command must be run as administrator when you enable real-time core debugging because it installs USB drivers for the debugger.
5. Close the window after the command completes because administrator privilege is no longer required.  

### Build and deploy the application

1. Start Visual Studio.
2. From the **File** menu, select **Open > CMake...** and navigate to the folder that contains the sample.
3. Select the file CMakeLists.txt and then click **Open**. 
4. In Solution Explorer, right-click the CMakeLists.txt file, and select **Generate Cache for Azure_Sphere_RTcore_FreeRTOS**. This step performs the cmake build process to generate the native ninja build files. 
5. In Solution Explorer, right-click the *CMakeLists.txt* file, and select **Build** to build the project and generate .imagepackage target.
6. Double click *CMakeLists.txt* file and press F5 to start the application with debugging. LED1 will blink red. Press button A to change the blink rate.
7. The demo will print message "FreeRTOS demo" after boot via IO0_TXD (Header3-6) and receive input from ISU0_RXD (Header2-1). Once a character is received, Azure Sphere will print to terminal through IO0_TXD. 
8. Demo timing is measured by LA. Perfect 125ms / 250ms / 500ms square wave can be observed on GPIO0 (Header1-4) -->
