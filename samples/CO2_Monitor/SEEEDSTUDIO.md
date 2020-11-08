# Improve health, wellbeing, and productivity by tracking your home workspace CO2 levels

How to build a healthier working environment by monitoring CO2, temperature, and humidity levels with a [Seeed Studio Azure Sphere device](https://azure.microsoft.com/en-gb/services/azure-sphere/?WT.mc_id=julyot-co2-dglover), an [SDC30 sensor](https://www.seeedstudio.com/Grove-CO2-Temperature-Humidity-Sensor-SCD30-p-2911.html), and [Azure IoT Central](https://azure.microsoft.com/en-gb/services/iot-central/?WT.mc_id=julyot-co2-dglover).

![](https://raw.githubusercontent.com/gloveboxes/CO2-levels-making-you-happy-grumpy-sleepy-or-sad/master/resources/banner-seeed.png)

---

## #JulyOT

This is part of the #JulyOT [IoT Tech Community](http://aka.ms/julyot) series, a collection of blog posts, hands-on-labs, and videos designed to demonstrate and teach developers how to build projects with Azure Internet of Things (IoT) services. Please also follow [#JulyOT](https://twitter.com/hashtag/JulyOT) on Twitter.

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

---

## Parts list

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

---

## Get started

Follow the complete guide to "[Track CO2 levels in your workspace to improve health, wellbeing, and productivity](https://gloveboxes.github.io/Track-CO2-levels-in-your-workspace-to-improve-health-wellbeing-and-productivity/)"

### Learning resources

Learning resources: [Azure Sphere Developer Learning Path](https://github.com/gloveboxes/Azure-Sphere-Learning-Path).
