# Lab 0: Lab Set Up

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

## Acknowledgements

### Azure Sphere Samples

These **Azure Sphere Learning Path** labs build on the [Azure Sphere Samples](https://github.com/Azure/azure-sphere-samples). You will find full scenario and API coverage in the samples documentation.

---

## Documentation

### Azure Sphere Documentation

Full [Azure Sphere Documentation](https://azure.microsoft.com/en-au/services/azure-sphere/?WT.mc_id=julyot-azd-dglover).

### Azure Sphere (azsphere) Command-Line Reference

[Overview of azsphere command-line utility](https://docs.microsoft.com/en-us/azure-sphere/reference/overview?WT.mc_id=julyot-azd-dglover).

---

## Troubleshooting

1. If you have trouble connecting to the Azure Sphere over USB be sure to disable any VPNs you might have enabled.
2. The **TAP-Windows Adapter V9** installed with VPN clients, including OpenVPN client is not compatible with the **TAP-Windows Adapter V9** required and installed by the Azure Sphere SDK for Visual Studio. You will need to uninstall the VPN client and reinstall the Azure Sphere SDK for Visual Studio.

---

## Prerequisites

1. Windows 10 Anniversary Update or later, Ubuntu 18.04 LTS, or Ubuntu 20.04 LTS
2. An Azure Sphere device

---

## Azure Sphere Tenant Credentials

### Personal Azure Sphere Device

If you have your own Azure Sphere device you will need the credentials you used when you claimed your device into an Azure Sphere Tenant.

### Shared Lab Azure Sphere Device

If you are using a shared lab device, then the lab instructor will tell you the Azure Sphere Tenant credentials you will need for the lab.

---

## Step 1: Set up your development environment

Follow the [Quickstarts to set up your Azure Sphere device](https://docs.microsoft.com/en-us/azure-sphere/install/install-sdk?pivots=vs-code&WT.mc_id=julyot-azd-dglover)

Follow the steps to:

1. Install the SDK and developer tools
2. Choose a tenant and claim your device
3. Configure networking and update the device OS
4. Subscribe to Azure Sphere notifications

---

## Step 2: Delete any existing applications on the Azure Sphere

1. From the **Azure Sphere Developer Command Prompt**, run

    ```bash
    azsphere device sideload delete
    ```

2. Restart the Azure Sphere. From the **Azure Sphere Developer Command Prompt**, run

    ```bash
    azsphere device restart
    ```

---

## Step 3: Enable High Level Core Device Debugging

From the **Azure Sphere Developer Command Prompt**, run

```bash
azsphere device enable-development
```

---

## Step 4: Clone the Azure Sphere Learning Path

**WARNING**: On Windows, clone the *Azure Sphere Learning Path* to a directory close to the root directory on your computer. For example **c:\lab**. The reason is the Azure Sphere build process uses CMake which does not support Windows 10 long path/folder names.

```bash
git clone --depth 1 https://github.com/gloveboxes/Azure-Sphere-Learning-Path.git Azure-Sphere
```

---

## Step 5: Clone the Azure Sphere Samples

The Azure Sphere samples repository includes a tool required for whitelisting IoT Central network endpoint URLs.

Clone this repository into the same directory you cloned the Azure Sphere Developer Learning Path into.

```bash
git clone --depth 1 https://github.com/Azure/azure-sphere-samples.git
```

**[NEXT](../Lab_1_Visual_Studio_and_Azure_Sphere/README.md)**

---
