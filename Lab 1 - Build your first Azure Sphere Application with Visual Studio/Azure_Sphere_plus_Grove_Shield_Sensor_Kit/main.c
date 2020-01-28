#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#include <applibs/log.h>
#include <applibs/gpio.h>

// Grove Temperature and Humidity Sensor
#include "../MT3620_Grove_Shield/MT3620_Grove_Shield_Library/Grove.h"
#include "../MT3620_Grove_Shield/MT3620_Grove_Shield_Library/Sensors/GroveTempHumiSHT31.h"

static volatile sig_atomic_t terminationRequested = false;

static void TerminationHandler(int signalNumber)
{
	// Don't use Log_Debug here, as it is not guaranteed to be async signal safe
	terminationRequested = true;
}

int main(int argc, char* argv[])
{
	Log_Debug("Application starting\n");

	// Register a SIGTERM handler for termination requests
	struct sigaction action;
	memset(&action, 0, sizeof(struct sigaction));
	action.sa_handler = TerminationHandler;
	sigaction(SIGTERM, &action, NULL);

	// Change this GPIO number and the number in app_manifest.json if required by your hardware.
	int fd = GPIO_OpenAsOutput(9, GPIO_OutputMode_PushPull, GPIO_Value_High);
	if (fd < 0) {
		Log_Debug(
			"Error opening GPIO: %s (%d). Check that app_manifest.json includes the GPIO used.\n",
			strerror(errno), errno);
		return -1;
	}

	// Initialize Grove Shield and Grove Temperature and Humidity Sensor
	int i2cFd;
	GroveShield_Initialize(&i2cFd, 115200);
	void* sht31 = GroveTempHumiSHT31_Open(i2cFd);

	const struct timespec sleepTime = { 1, 0 };
	while (!terminationRequested) {

		GroveTempHumiSHT31_Read(sht31);
		float temp = GroveTempHumiSHT31_GetTemperature(sht31);
		float humi = GroveTempHumiSHT31_GetHumidity(sht31);
		Log_Debug("Temperature: %.1fC\n", temp);
		Log_Debug("Humidity: %.1f\%c\n", humi, 0x25);

		GPIO_SetValue(fd, GPIO_Value_Low);
		nanosleep(&sleepTime, NULL);

		GPIO_SetValue(fd, GPIO_Value_High);
		nanosleep(&sleepTime, NULL);
	}
}
