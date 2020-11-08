# Improve health, wellbeing, and productivity by tracking your home workspace CO2 levels

How to build a healthier working environment by monitoring CO2, temperature, and humidity levels with an [Azure Sphere](https://azure.microsoft.com/en-gb/services/azure-sphere/?WT.mc_id=julyot-co2-dglover), an [SDC30 sensor](https://www.seeedstudio.com/Grove-CO2-Temperature-Humidity-Sensor-SCD30-p-2911.html), and [Azure IoT Central](https://azure.microsoft.com/en-gb/services/iot-central/?WT.mc_id=julyot-co2-dglover).

![](https://raw.githubusercontent.com/gloveboxes/CO2-levels-making-you-happy-grumpy-sleepy-or-sad/master/resources/banner.png)

---

## #JulyOT

This is part of the #JulyOT [IoT Tech Community](http://aka.ms/julyot) series, a collection of blog posts, hands-on-labs, and videos designed to demonstrate and teach developers how to build projects with Azure Internet of Things (IoT) services. Please also follow [#JulyOT](https://twitter.com/hashtag/JulyOT) on Twitter.

---

## Source code and learning resources

Source code: [Track CO2 levels in your workspace to improve health, wellbeing, and productivity](https://gloveboxes.github.io/Track-CO2-levels-in-your-workspace-to-improve-health-wellbeing-and-productivity/).

Learning resources: [Azure Sphere Developer Learning Path](https://github.com/gloveboxes/Azure-Sphere-Learning-Path).

---

## Are CO2 levels making you grumpy, sleepy, or sad

Working from home it is easy to close the door to shut out the noise of everyday life while we get on with work. Closing the door can lead to a build-up of CO2 gas, a by-product of our breathing, which can impact our wellbeing, concentration, and productivity levels.

Check out "[Indoor carbon dioxide levels could be a health hazard, scientists warn](https://www.theguardian.com/environment/2019/jul/08/indoor-carbon-dioxide-levels-could-be-a-health-hazard-scientists-warn)".

The problem is we cannot see or smell Carbon Dioxide, it just keeps building up, and we have no way of knowing it is happening other than getting tired or a headache. So, with that in mind, I figured it was the Internet of Things to the rescue!

---

## The solution

I wanted to build a secure IoT device with [Azure Sphere](https://azure.microsoft.com/en-gb/services/azure-sphere/?WT.mc_id=julyot-co2-dglover) using the [Seeed Studio Grove CO2 & Temperature & Humidity Sensor](https://www.seeedstudio.com/Grove-CO2-Temperature-Humidity-Sensor-SCD30-p-2911.html) I had in my box of bits. The folks at Sensirion made it super easy to port their [SCD30 driver](https://github.com/Sensirion/embedded-scd) to Azure Sphere. It was just a matter of implementing the I2C init/read/write functions, a microsecond sleep function, plus setting up CMake build. It all just worked. The ported driver is included in this project.

---

## Azure IoT Central

[Azure IoT Central](https://azure.microsoft.com/en-in/services/iot-central/?WT.mc_id=julyot-co2-dglover) provides an easy way to connect, monitor, and manage your Internet of Things (IoT) assets at scale.

I created a free trial of [Azure IoT Central](https://azure.microsoft.com/en-gb/services/iot-central/?WT.mc_id=julyot-co2-dglover) and in no time I had CO2, temperature, and humidity telemetry displayed (yes, the data is real, so we have made some changes at home!). By the way, you can continue to connect two devices for free to IoT Central after the trial period expires.

![](https://raw.githubusercontent.com/gloveboxes/CO2-levels-making-you-happy-grumpy-sleepy-or-sad/master/resources/iot-central-dash.png)

### Extend and integrate Azure IoT Central applications with other cloud services

Azure IoT Central is also extensible using rules and workflows. For more information, review [Use workflows to integrate your Azure IoT Central application with other cloud services](https://docs.microsoft.com/en-us/azure/iot-central/core/howto-configure-rules-advanced?WT.mc_id=julyot-co2-dglover)

---

## Parts list

The solution supports two configurations.

### Seeed Studio Azure Sphere Mini Dev Board

1. [Seeed Studio Seeed Studio MT3620 Mini Dev Board](https://www.seeedstudio.com/mt3620-for-azure-sphere)
2. [MT3620 Grove Breakout](https://www.seeedstudio.com/MT3620-Grove-Breakout-p-4043.html)
3. [Seeed Studio Grove CO2 & Temperature & Humidity Sensor](https://www.seeedstudio.com/Grove-CO2-Temperature-Humidity-Sensor-SCD30-p-2911.html)
4. Optional, [3 x Grove LEDs](https://www.seeedstudio.com/Grove-Green-LED.html), or a [Grove Relay](https://www.seeedstudio.com/Grove-Relay.html) to drive a bigger warning light!

### Connect Grove peripherals

Attach the Azure Sphere to the Grove Breakout board and plug in the following peripherals.

1. Green LED plugged into Grove Socket D1
2. Red LED plugged into Grove Socket D0
3. Blue LED plugged into Grove Socket UART0
4. CO2, Temperature, & Humidity sensor into Grove Socket I2C

![](https://raw.githubusercontent.com/gloveboxes/CO2-levels-making-you-happy-grumpy-sleepy-or-sad/master/resources/seeed_studio-azure-sphere_mini-co2-monitor.jpg)

### AVNET Azure Sphere Starter Kit

1. [AVNET Azure Sphere Starter Kit](https://www.avnet.com/shop/us/products/avnet-engineering-services/aes-ms-mt3620-sk-g-3074457345636825680/)
2. [Seeed Studio Grove CO2 & Temperature & Humidity Sensor](https://www.seeedstudio.com/Grove-CO2-Temperature-Humidity-Sensor-SCD30-p-2911.html)
3. Optional, 1 x [Click Relay](https://www.mikroe.com/relay-click) to drive a bigger warning light.

### Connect the Grove peripheral

1. Plug the CO2, Temperature, & Humidity sensor into Grove Socket I2C on the AVNET Starter Kit.

![](https://raw.githubusercontent.com/gloveboxes/CO2-levels-making-you-happy-grumpy-sleepy-or-sad/master/resources/avnet_azure_sphere_starter_kit.jpg)

---

## Calibrating the SCD20 CO2 Sensor

**IMPORTANT**. Be sure to read up on calibrating the [Grove - CO2 & Temperature & Humidity Sensor (SCD30) Calibration](https://wiki.seeedstudio.com/Grove-CO2_Temperature_Humidity_Sensor-SCD30/#calibration) sensor. This solution will enable auto-calibration.

---

## How to build the solution

Clone the **Azure Sphere CO2 Monitor** solution to your desktop.

```bash
git clone https://github.com/gloveboxes/Track-CO2-levels-in-your-workspace-to-improve-health-wellbeing-and-productivity.git azure-sphere-co2-monitor
```

---

## Set up your developer tools and your Azure Sphere

Follow the Azure Sphere Developer Learning Path [Lab Set Up](https://github.com/gloveboxes/Azure-Sphere-Learning-Path/tree/master/zdocs_vs_code_iot_central/Lab_0_Introduction_and_Lab_Set_Up) to set up the developer tools, claim you Azure Sphere, and configure the device WiFi, and enable developer mode.

---

## Azure IoT Central

Your Azure Sphere device can securely connect and communicate with cloud services. Azure Sphere includes built-in library support for both Azure IoT Hub and Azure IoT Central. This lab focuses on Azure IoT Central. Take a moment to read [Your IoT journey: simplified and secure with Azure IoT Central and Azure Sphere](https://techcommunity.microsoft.com/t5/internet-of-things/your-iot-journey-simplified-and-secure-with-azure-iot-central/ba-p/1404247?WT.mc_id=julyot-co2-dglover).

[Azure IoT Central](https://azure.microsoft.com/en-in/services/iot-central/?WT.mc_id=julyot-co2-dglover) provides an easy way to connect, monitor, and manage your Internet of Things (IoT) assets at scale.

![Azure IoT Central](https://raw.githubusercontent.com/gloveboxes/Track-CO2-levels-in-your-workspace-to-improve-health-wellbeing-and-productivity/master/resources/azure-iot-central.jpg)

---

## Step 1: Create a new IoT Central Application

1. So the lab instructions are still visible, right mouse click, and open this link "[Azure IoT Central](https://azure.microsoft.com/en-au/services/iot-central/?WT.mc_id=julyot-co2-dglover)" in a new window.

2. Click **Build a solution**.

3. Next, you will need to sign with your Microsoft Personal, or Work, or School account. If you do not have a Microsoft account, then you can create one for free using the **Create one!** link.

    ![iot central](https://raw.githubusercontent.com/gloveboxes/Track-CO2-levels-in-your-workspace-to-improve-health-wellbeing-and-productivity/master/resources/iot-central-login.png)

4. Expand the sidebar menu by clicking on the **Burger menu** icon.

    ![](https://raw.githubusercontent.com/gloveboxes/Track-CO2-levels-in-your-workspace-to-improve-health-wellbeing-and-productivity/master/resources/iot-central-burger-menu.png)

5. Click **+ New application** to create a new Azure IoT Central application. 

6. Select **Custom app**

    ![](https://raw.githubusercontent.com/gloveboxes/Track-CO2-levels-in-your-workspace-to-improve-health-wellbeing-and-productivity/master/resources/iot-central-custom-app.png)

### Create a new application

1. Specify the **Application name**, the **URL**, select the **Free** pricing plan, and complete the registration form. 

    ![](https://raw.githubusercontent.com/gloveboxes/Track-CO2-levels-in-your-workspace-to-improve-health-wellbeing-and-productivity/master/resources/iot-central-new-application.png)

2. Then click **Create**.

### Create a new device template

A device template is a blueprint that defines the characteristics and behaviors of a type of device that connects to an Azure IoT Central application.

For more information on device templates, review the [Define a new IoT device type in your Azure IoT Central application](https://docs.microsoft.com/en-us/azure/iot-central/core/howto-set-up-template?WT.mc_id=julyot-co2-dglover) article. 

1. Click **Device templates**, then **+ New**.
    ![](https://raw.githubusercontent.com/gloveboxes/Track-CO2-levels-in-your-workspace-to-improve-health-wellbeing-and-productivity/master/resources/iot-central-template-new.png)

2. Click the **IoT device** template type.

    ![](https://raw.githubusercontent.com/gloveboxes/Track-CO2-levels-in-your-workspace-to-improve-health-wellbeing-and-productivity/master/resources/iot-central-new-iot-device-template.png)

3. Create an **IoT Device** Template.

    1. Select **IoT device**,
    2. Click **Next:Customise**,
    3. Name your template **Carbon Dioxide Monitor**,
    4. Click **Next: Review**,
    5. Click **Create**.

### Import a Capability Model

1. Click **Import capability model**
2. Navigate to the folder you cloned the CO2 Monitor solution into.
3. Navigate to the **iot_central** folder.
4. Select **CO2_Monitor_Capability_Model.json** and open

### Create a device visualization view

1. Click **Views**.
    ![](https://raw.githubusercontent.com/gloveboxes/Track-CO2-levels-in-your-workspace-to-improve-health-wellbeing-and-productivity/master/resources/iot-central-create-a-view.png)
2. Select **Visualizing the device**.
    ![](https://raw.githubusercontent.com/gloveboxes/Track-CO2-levels-in-your-workspace-to-improve-health-wellbeing-and-productivity/master/resources/iot-central-add-tile-status.png)
3. Expand the **Telemetry** section
4. Select **Carbon Dioxide (ppm)**
5. Click **Add Tile**
6. Select **Humidity** and **Temperature** telemetry items.
7. Click **Add Tile**.
8. Click **Save** to save the view.
    ![](https://raw.githubusercontent.com/gloveboxes/Track-CO2-levels-in-your-workspace-to-improve-health-wellbeing-and-productivity/master/resources/iot-central-view-save.png)

### Create a properties form

1. Click **Views**.
2. Click the **Editing device and cloud data** option.
    ![](https://raw.githubusercontent.com/gloveboxes/Track-CO2-levels-in-your-workspace-to-improve-health-wellbeing-and-productivity/master/resources/iot-central-view-properties-create.png)
3. Expand the **Properties** section.
4. Select **all properties**.
5. Click **Add Section**.
6. Click **Save** to save the form.
    ![](https://raw.githubusercontent.com/gloveboxes/Track-CO2-levels-in-your-workspace-to-improve-health-wellbeing-and-productivity/master/resources/iot-central-properties-save.png)

### Publish the device template

1. Click **Publish** to publish the template. Publishing the template makes it available for devices.
    ![](https://raw.githubusercontent.com/gloveboxes/Track-CO2-levels-in-your-workspace-to-improve-health-wellbeing-and-productivity/master/resources/iot-central-template-publish.png)
    <br/>

2. Next, confirm and click **Publish**

>See [Define a new IoT device type in your Azure IoT Central application](https://docs.microsoft.com/en-us/azure/iot-central/core/howto-set-up-template?WT.mc_id=julyot-co2-dglover) for information on creating your own device templates.

---

## Step 2: Link your Azure Sphere Tenant to IoT Central

You need to set up a trust relationship between your Azure Sphere tenant and your IoT Central application.

Devices claimed by your Azure Sphere tenant will be automatically enrolled when it first connects to your IoT Central application.

### Download the tenant authentication CA certificate

1. Open an **Azure Sphere Developer Command Prompt**
2. Be sure to make a note of the current directory, or change to the CO2 Monitor directory. You will need the name of this directory in the next step. 
3. Download the Certificate Authority (CA) certificate for your Azure Sphere tenant:

    ```bash
    azsphere tenant download-CA-certificate --output CAcertificate.cer
    ```

    The output file must have the .cer extension.

### Upload the tenant CA certificate to Azure IoT Central and generate a verification code

1. In Azure IoT Central, go to Administration > Device Connection > Manage primary certificate.

2. Click the folder icon next to the Primary box and navigate to the directory where you downloaded the certificate. If you don't see the .cer file in the list, make sure that the view filter is set to All files (*). Select the certificate and then click the gear icon next to the Primary box.

3. The Primary Certificate dialog box appears. The Subject and Thumbprint fields contain information about the current Azure Sphere tenant and primary root certificate.

4. Click the Refresh icon to the right of the Verification Code box to generate a verification code. Copy the verification code to the clipboard.

    ![](https://raw.githubusercontent.com/gloveboxes/Track-CO2-levels-in-your-workspace-to-improve-health-wellbeing-and-productivity/master/resources/iot-central-certificate-verify.png)

### Verify the tenant CA certificate

1. Return to the Azure Sphere Developer Command Prompt.

2. Download a validation certificate that proves that you own the tenant CA certificate. Replace code in the command with the verification code from the previous step.

    ```bash
    azsphere tenant download-validation-certificate --output ValidationCertification.cer --verificationcode <code>
    ```
3. The Azure Sphere Security Service signs the validation certificate with the verification code to prove that you own the CA.

### Use the validation certificate to verify the tenant identity

1. Return to Azure IoT Central and click Verify.
2. When prompted, navigate to the validation certificate that you downloaded in the previous step and select it. When the verification process is complete, the Primary Certificate dialog box displays the Verified message. Click Close to dismiss the box.

    ![](https://raw.githubusercontent.com/gloveboxes/Track-CO2-levels-in-your-workspace-to-improve-health-wellbeing-and-productivity/master/resources/iot-central-certificate-verified.png)

After you complete these steps, any device that is claimed into your Azure Sphere tenant will automatically be enrolled in your Azure IoT Central application when it first connects.

---

## Step 3: Explicitly allow connections to Azure IoT Central Endpoints

Remember, applications on Azure Sphere are locked down by default, including hardware and network endpoints. You must explicitly allow connections to the network endpoints of your Azure IoT Central application otherwise your Azure Sphere application will not be able to connect.

Follow these steps:

1. Open the **Azure Sphere Developer Command Prompt**.
2. Navigate to the Samples -> AzureIoT -> Tools directory. You cloned the Azure Sphere samples repository in the first lab.
    * On Windows, navigate to the Samples\AzureIoT\Tools\win-x64 directory.
    * On Linux, navigate to the Samples\AzureIoT\Tools\linux-x64 directory. On Linux, you may need to explicitly set execution permissions for the ShowIoTCentralConfig tool. From a terminal, run `chmod +x ShowIoTCentralConfig` to add execution permissions for the tool.

3. When you run the **ShowIoTCentralConfig** tool, you will be prompted for input data. The following table outlines what information you will be prompted for and where to obtain the required data.

    | Input data | From |
    |------------|----------|
    | **Are you using a legacy (2018) IoT Central application (Y/N)** | Respond **N** |
    | **IoT&nbsp;Central&nbsp;App&nbsp;URL** |  This can be found in your browser address bar. For Example https://myiotcentralapp.azureiotcentral.com/ |
    | **API token** | This can be generated from your IoT Central application. In the Azure IoT Central application select **Administration**, select **API Tokens**, select **Generate Token**, provide a name for the token (for example, "AzureSphereSample"), select **Administrator** as the role, and click **Generate**. Copy the token to the clipboard. The token starts with **SharedAccessSignature**. |
    | **ID Scope** | In the Azure IoT Central application, select **Administration** > **Device Connection** and then copy the **ID Scope** |
4. Run the **ShowIoTCentralConfig** tool.
    Now follow the prompts that the tool provides, and copy the information from the output into the app_manifest.json file in Visual Studio.

    > **Note**: Your organization might require consent for the ShowIoTCentralConfig tool to access your Azure IoT Central data in the same way that the Azure API requires such consent. In some organizations, [enterprise application permissions](https://docs.microsoft.com/azure-sphere/install/admin-consent?WT.mc_id=julyot-co2-dglover) must be granted by an IT administrator.
5. Review the output from the **ShowIoTCentralConfig** tool. It will look similar to the following text.

    </br>

    Find and modify the CmdArgs, AllowedConnections and DeviceAuthentication lines in your app_manifest.json so each includes the content from the below:

    ```json
    "CmdArgs": [ "0ne000BDC00" ],
    "Capabilities": {
        "AllowedConnections": [ "global.azure-devices-provisioning.net", "iotc-9999bc-3305-99ba-885e-6573fc4cf701.azure-devices.net", "iotc-789999fa-8306-4994-b70a-399c46501044.azure-devices.net", "iotc-7a099966-a8c1-4f33-b803-bf29998713787.azure-devices.net", "iotc-97299997-05ab-4988-8142-e299995acdb7.azure-devices.net", "iotc-d099995-7fec-460c-b717-e99999bf4551.azure-devices.net", "iotc-789999dd-3bf5-49d7-9e12-f6999991df8c.azure-devices.net", "iotc-29999917-7344-49e4-9344-5e0cc9999d9b.azure-devices.net", "iotc-99999e59-df2a-41d8-bacd-ebb9999143ab.azure-devices.net", "iotc-c0a9999b-d256-4aaf-aa06-e90e999902b3.azure-devices.net", "iotc-f9199991-ceb1-4f38-9f1c-13199992570e.azure-devices.net" ],
        "DeviceAuthentication": "--- YOUR AZURE SPHERE TENANT ID---",
    }
    ```

6. **Copy** the output from the ShowIoTCentralConfig tool to **_Notepad_** as you will need this information soon.

---

## Step 4: Get the Azure Sphere Tenant ID

We need the ID of the Azure Sphere Tenant that is now trusted by Azure IoT Central.

1. From the **Azure Sphere Developer Command Prompt**, run **```azsphere tenant show-selected```**.
    * The output of this command will look similar to the following.
        ```text
        Default Azure Sphere tenant ID is 'yourSphereTenant' (99999999-e021-43ce-9999-fa9999499994).
        ```
    * The **Tenant ID** is the numeric value inside the parentheses.
2. **Copy the Tenant ID to _Notepad_** as you will need it soon.

<!-- ---

## Azure IoT Central Device Templates

A device template is a blueprint that defines the characteristics and behaviors of a type of device that connects to an Azure IoT Central application.

For more information on device templates, review the [Define a new IoT device type in your Azure IoT Central application](https://docs.microsoft.com/en-us/azure/iot-central/core/howto-set-up-template?WT.mc_id=julyot-co2-dglover) article. 

1. From Azure IoT Central, navigate to **Device templates**, and select the **Azure Sphere** template.
2. Click on **Interfaces** to list the interface capabilities.
3. Explore the IoT Central device template interfaces, properties, and views.

![](https://raw.githubusercontent.com/gloveboxes/Track-CO2-levels-in-your-workspace-to-improve-health-wellbeing-and-productivity/master/resources/iot-central-device-template-display.png) -->

---

## Open the CO2 Monitor solution

### Step 1: Start Visual Studio Code

![](https://raw.githubusercontent.com/gloveboxes/Track-CO2-levels-in-your-workspace-to-improve-health-wellbeing-and-productivity/master/resources/vs-code-start.png)

### Step 2: Open the lab project

1. Click **Open folder**.
2. Open the **azure-sphere-co2-monitor** solution folder.
3. Open the **co2-monitor** folder.
4. Click **Select Folder** or the **OK** button to open the project.

### Step 3: Set your developer board configuration

This solution supports the AVNET Starter Kit and the Seeed Studio Mini Dev Board with the Grove Breakout. You need to set the configuration that matches your developer board.

The default developer board configuration is for the AVENT Azure Sphere Starter Kit. If you have this board, there is no additional configuration required.

1. Open CMakeList.txt
	![](https://raw.githubusercontent.com/gloveboxes/Track-CO2-levels-in-your-workspace-to-improve-health-wellbeing-and-productivity/master/resources/vs-code-open-cmake.png)
2. Add a # at the beginning of the set AVNET line to disable it.
3. Uncomment the **set** command that corresponds to your Azure Sphere developer board.

	```text
	set(AVNET TRUE "AVNET Azure Sphere Starter Kit")                
	# set(SEEED_STUDIO_RDB TRUE "Seeed Studio Azure Sphere MT3620 Development Kit (aka Reference Design Board or rdb)")
	# set(SEEED_STUDIO_MINI TRUE "Seeed Studio Azure Sphere MT3620 Mini Dev Board")
	```	

4. Save the file. This will auto-generate the CMake cache.

### Step 4: Configure the Azure Sphere Application

1. Open the **app_manifest.json** file

    ![](https://raw.githubusercontent.com/gloveboxes/Track-CO2-levels-in-your-workspace-to-improve-health-wellbeing-and-productivity/master/resources/vs-code-open-app-manifest.png)

2. Update the Azure IoT Central Application connection properties.
    * Update **CmdArgs** with your Azure IoT Central **ID Scope**. 
    * Update **DeviceAuthentication** with your **Azure Sphere Tenant ID**. Remember, this was the numeric value output from the ```azsphere tenant show-selected``` command that you copied to Notepad.

3. Update the network endpoints **AllowedConnections** with your Azure IoT Central Application endpoint URLs you copied to Notepad.

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

## Deploying the Application to Azure Sphere

### Step 1: Start the app build deploy process

1. Ensure main.c is open.
2. Select **CMake: [Debug]: Ready** from the Visual Studio Code Status Bar.

	![](https://raw.githubusercontent.com/gloveboxes/Track-CO2-levels-in-your-workspace-to-improve-health-wellbeing-and-productivity/master/resources/vs-code-start-application.png)

3. From Visual Studio Code, press <kbd>F5</kbd> to build, deploy, start, and attached the remote debugger to the application now running the Azure Sphere device.


### Step 2: View debugger output

1. Open the Visual Studio Code **Output** tab to view the output from **Log_Debug** statements in the code.

	> Pro Tip. You can open the output window by using the Visual Studio Code <kbd>Ctrl+K Ctrl+H</kbd> shortcut or click the **Output** tab.
2. You will see the device negotiating security, and then it will start sending telemetry to Azure IoT Central.

> You may see a couple of *ERROR: failure to create IoTHub Handle* messages displayed. These messages occur while the connection to IoT Central is being negotiated.

---

## Expected Device Behaviour

### Avnet Azure Sphere MT3620 Starter Kit

![](https://raw.githubusercontent.com/gloveboxes/CO2-levels-making-you-happy-grumpy-sleepy-or-sad/master/resources/avnet_azure_sphere_starter_kit.jpg)

1. The yellow WLAN LED will turn on when a connection is established with IoT Central.
2. The RBG will turn red or blue depending on HVAC Temperature you set in IoT Central.
3. The App Status LED will turn on if the CO2 level exceeds the CO2 alert level set in IoT Central.

### Seeed Studio MT3620 Mini Dev Board

![](https://raw.githubusercontent.com/gloveboxes/CO2-levels-making-you-happy-grumpy-sleepy-or-sad/master/resources/seeed_studio-azure-sphere_mini-co2-monitor.jpg)

1. The yellow User LED will turn on when a connection is established with IoT Central.
2. The red or blue LED will turn depending on HVAC Temperature you set in IoT Central.
3. The green LED will turn on if the CO2 level exceeds the CO2 alert level set in IoT Central.

---

## Step 3: View the device telemetry on the Azure IoT Central Dashboard

1. Switch back to Azure IoT Central in your web browser.
2. You need to wait a minute or two before your Azure Sphere device is automatically enrolled.
3. The newly enrolled device will have a numeric name that matches your Azure Sphere Device ID.
4. To display your Azure Sphere Device ID, start the **Azure Sphere Developer Command Prompt** and run the following command.

    ```bash
    azsphere device show-attached
    ```

---

## Step 4: Migrate your device to the Azure Sphere Template

You need to **Migrate** the newly enrolled device to the **Carbon Dioxide Monitor** template. The template maps the JSON formatted telemetry to the dashboard.

1. Select the newly enrolled device from the **All devices** template.
2. Click **Migrate**

    ![](https://raw.githubusercontent.com/gloveboxes/Track-CO2-levels-in-your-workspace-to-improve-health-wellbeing-and-productivity/master/resources/iot-central-migrate-device.png)

3. Select the **Carbon Dioxide Monitor** Template, and then click migrate.

    ![](https://raw.githubusercontent.com/gloveboxes/Track-CO2-levels-in-your-workspace-to-improve-health-wellbeing-and-productivity/master/resources/iot-central-migrate-select-template.png)

---

## Step 5: Display the Azure Sphere device telemetry

1. Click **Devices** on the sidebar.
2. Select the **Carbon Dioxide** template.
3. Click on the migrated device.
4. Select the **View** tab to view the device telemetry.
5. Rename your device. Click the **Rename** button and give your device a friendly name.

> Azure IoT Central does not update immediately. It may take a minute or two for the temperature, humidity, and pressure telemetry to be displayed.

![](https://raw.githubusercontent.com/gloveboxes/CO2-levels-making-you-happy-grumpy-sleepy-or-sad/master/resources/iot-central-dash.png)

### Properties view

You can view and set device properties from the **Form** tab.

![](https://raw.githubusercontent.com/gloveboxes/Track-CO2-levels-in-your-workspace-to-improve-health-wellbeing-and-productivity/master/resources/iot-central-set-desired-property-state.png)

### Setting the HVAC thermostat

The device simulates an HVAC. If the current temperature read by the device is greater than the thermostat setting then the blue LED will turn on to signify the room needs to be cooled. If the room temperature is less than the thermostat setting the red LED will be turned on to signify the heater needs to be turned on.

### Setting the CO2 alert level from the properties view

From the properties view you can set the CO2 alert level. Azure IoT Central uses properties and ultimately Azure IoT Hub device twins to set the alert threshold on the device. 

When the CO2 level reaches the alert level on the device you can set an action. The action that has been defined in code is to switch on an LED, but you can go bigger, and instead of setting an LED you could drive a relay to turn on a light, buzz an alarm, or turn on a fan.

### Learn more about IoT Central properties and Azure IoT Hub device twins

Learn more about Azure IoT Central, properties, and device twins from Azure Sphere Developer Learning Path lab 3 [Set the room virtual thermostat with Azure IoT Device Twins](https://github.com/gloveboxes/Azure-Sphere-Learning-Path/tree/master/zdocs_vs_code_iot_central/Lab_3_Control_Device_with_Device_Twins).

---

## Extend and integrate Azure IoT Central applications with other cloud services

Azure IoT Central is also extensible using rules and workflows. For more information, review [Use workflows to integrate your Azure IoT Central application with other cloud services](https://docs.microsoft.com/en-us/azure/iot-central/core/howto-configure-rules-advanced?WT.mc_id=julyot-co2-dglover)

---

Have fun and stay safe and be sure to follow us on [#JulyOT](https://twitter.com/hashtag/JulyOT?src=hash&ref_src=twsrc%5Etfw).
