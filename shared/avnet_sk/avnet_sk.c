#include "avnet_sk.h"

/// <summary>
///     Reads telemetry and returns the length of JSON data.
/// </summary>
int ReadTelemetry(char * msgBuffer, size_t bufferLen) {
	static int msgId = 0;
	float temperature;
	float humidity;
	float pressure;
	int light;

	AvnetSkSensorUpdate(); // Important - this must be called immediately before reading the telemtry

	//AngularRateDegreesPerSecond ardps = GetAngularRate();
	//AccelerationMilligForce amgf = GetAcceleration();
	
	//Log_Debug("LSM6DSO: Angular rate [degrees per second] : %4.2f, %4.2f, %4.2f\n", ardps.x, ardps.y, ardps.z);
	//Log_Debug("\nLSM6DSO: Acceleration [millig force]  : %.4lf, %.4lf, %.4lf\n", amgf.x, amgf.y, amgf.z);

	pressure = GetPressure();
	temperature = GetTemperature();
	light = GetLightLevel();

	int rnd = (rand() % 10) - 5;
	humidity = (float)(50.0 + rnd);

	static const char* MsgTemplate = "{ \"Temperature\": \"%3.2f\", \"Humidity\": \"%3.1f\", \"Pressure\":\"%3.1f\", \"Light\":%d, \"MsgId\":%d }";
	return snprintf(msgBuffer, bufferLen, MsgTemplate, temperature, humidity, pressure, light, msgId++);
}

bool RegisterBoard(void) {

	if (initI2c() == -1) {
		return -1;
	}

	OpenADC();

	return true;
}

bool CloseBoard(void) {
	closeI2c();
	return true;
}