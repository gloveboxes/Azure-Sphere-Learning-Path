# About Azure Sphere MT3620 Grove Shield Library

This is library for Azure Sphere MT3620 Grove Shield, the shield enhences Azure Sphere by adding I2C interface and Analog input. 

## Requirements

- Operation Systems - Windows 10 or Ubuntu 18.04
- Developer Tools: Visual Studio [Visual Studio Community/Professional/Enterprise](https://visualstudio.microsoft.com/downloads/) or [Visual Studio Code](https://code.visualstudio.com/)
- Hardware - [Azure Sphere](https://www.seeedstudio.com/Azure-Sphere-MT3620-Development-Kit-US-Version-p-3052.html), [MT3620 Grove Shield](https://www.seeedstudio.com/MT3620-Grove-Shield-p-3145.html) 

## Create a MT3620 application

Review the [Azure Sphere documentation]https://docs.microsoft.com/en-au/azure-sphere/ for the guide to setting up the developer tools and Azure Sphere SDK on Windows 10 or Ubuntu 18.04.


## MT3620 Grove Shield App Manifest

Select __app_manifest.json__ in the application project, add the below attributions, so that we can use the peripherals that MT3620 Grove Shield would use.

```JSON
"Capabilities": {
	"Gpio": [ 8, 9, 10, 15, 16, 17, 18, 19, 20, 12, 13, 0, 1, 4, 5, 57, 58, 11, 14, 48 ],
	"Uart": [ "ISU0", "ISU3" ],
	"AllowedApplicationConnections": []
}
```

## Some available header files

- Grove.h
- Sensors/Grove4DigitDisplay.h
- Sensors/GroveRelay.h
- Sensors/GroveTempHumiBaroBME280.h
- Sensors/GroveTempHumiSHT31.h
- Sensors/GroveAD7992.h
- Sensors/GroveOledDisplay96x96.h
- Sensors/GroveRelay.h
- Sensors/GroveRotaryAngleSensor.h
- Sensors/GroveLEDButton.h
- Sensors/GroveLightSensor.h


## Usage of the library, see Example - Temp and Huminidy SHT31

1. Add headers

```C
#include "Grove.h"
#include "Sensors/GroveTempHumiSHT31.h"
```

2. Initialize the shield in main() function

```C
int i2cFd;
GroveShield_Initialize(&i2cFd, 115200); // baudrate - 9600,14400,19200,115200,230400 
```

1. Initialize and instantiation

```C
void* sht31 = GroveTempHumiSHT31_Open(i2cFd);
```


4. Read temp and humidiy from the sensor
   
```C
GroveTempHumiSHT31_Read(sht31);
float temp = GroveTempHumiSHT31_GetTemperature(sht31);
float humi = GroveTempHumiSHT31_GetHumidity(sht31);
```
