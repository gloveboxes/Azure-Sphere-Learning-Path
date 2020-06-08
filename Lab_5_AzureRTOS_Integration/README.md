# Introduction 
This is a Visual Studio project to run ThreadX demo on Azure Sphere.

# Getting Started

1. Install Visual Studio 2019, version 16.4 or later.
2. Install [Azure Sphere SDK for Visual Studio](https://aka.ms/AzureSphereSDKDownload)
3. Plug in the device and open Azure Sphere Developer Command Prompt in the Start menu under Azure Sphere. Then run following commands:
- Login: 'azsphere login' or 'azsphere login --newuser \<email-address\>'
- Create tenant: 'azsphere tenant create --name \<my-tenant\>'
- Enable app development: 'azsphere device enable-development'
- Claim your device: 'azsphere device claim' (Note: Claiming is a one-time operation that you cannot undo)
- You may need to update the OS by using 'azsphere device recover' if you have an older device.
4. Open Azure Sphere Developer Command Prompt with administrator privilege and run 'azsphere device enable-development --enablertcoredebugging'.

Note: Skip the above steps if you already did it before.

5. Connect PC UART(or USB-to-serial adapter) Rx to Seeed MT3620 Development Kit GPIO 26 / TXD0 (ISU0_UART_TX), connect Ground pins together. See [MT3620_RTApp_BareMetal_HelloWorld](https://github.com/MediaTek-Labs/mt3620_m4_software/blob/master/MT3620_M4_Sample_Code/BareMetal/MT3620_RTApp_BareMetal_HelloWorld/README.md) for details.

# Build and Test

1. Start Visual Studio 2019 and select 'Open a local folder'.
2. Browse and select 'demo_threadx_azure_sphere' folder.
3. Expend 'tx' in Solution Explorer, right click 'CMakeLists.txt' and select Build.
4. Expend 'MT3620_lib' in Solution Explorer, right click 'CMakeLists.txt' and select Build.
5. Expend 'demo_threadx' in Solution Explorer, right click 'CMakeLists.txt' and select Build.
6. Right click 'CMakeLists.txt' under 'demo_threadx' and select 'Set as Startup Item'
7. Open UART port by using a terminal app, set baud rate to 115200.
8. In Visual Studio, press F5 to start debugging.
9. View outputs in the terminal.


# Reference

- [Set up your Windows PC for app development](https://docs.microsoft.com/en-ca/azure-sphere/install/development-environment-windows)
- [Claim your device](https://docs.microsoft.com/en-ca/azure-sphere/install/claim-device)
- [Update the OS on an early dev kit](https://docs.microsoft.com/en-ca/azure-sphere/resources/update-old-seeed-os)
 
