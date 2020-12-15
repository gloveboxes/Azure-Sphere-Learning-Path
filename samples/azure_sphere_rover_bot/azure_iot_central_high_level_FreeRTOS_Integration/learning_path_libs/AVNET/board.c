#include "board.h"


/// <summary>
///     Reads telemetry and returns the length of JSON data.
/// </summary>
bool lp_readTelemetry(LP_ENVIRONMENT * environment) {
	AvnetSkSensorUpdate(); // Important - this must be called immediately before reading the telemetry

	//AngularRateDegreesPerSecond ardps = GetAngularRate();
	//AccelerationMilligForce amgf = GetAcceleration();
	
	//Log_Debug("\nLSM6DSO: Angular rate [degrees per second] : %4.2f, %4.2f, %4.2f", ardps.x, ardps.y, ardps.z);
	//Log_Debug("\nLSM6DSO: Acceleration [millig force]  : %.4lf, %.4lf, %.4lf\n", amgf.x, amgf.y, amgf.z);

	environment->pressure = GetPressure();
	environment->temperature = GetTemperature();
	//light = GetLightLevel();
	environment->light = 0;

	int rnd = (rand() % 10) - 5;
	environment->humidity = (float)(50.0 + rnd);

	return true;
}

bool lp_initializeDevKit(void) {

	srand((unsigned int)time(NULL)); // seed the random number generator for fake telemetry

	if (initI2c() == -1) {
		return -1;
	}

	//OpenADC();

	return true;
}

bool lp_closeDevKit(void) {
	closeI2c();
	return true;
}
