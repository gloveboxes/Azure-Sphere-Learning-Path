#include "board.h"


/// <summary>
///     Reads telemetry and returns the length of JSON data.
/// </summary>
bool lp_readTelemetry(LP_ENVIRONMENT* environment)
{
	//ENSURE calibrate_angular_rate(); call from lp_initializeDevKit before calling get_angular_rate()

	//AngularRateDegreesPerSecond ardps = get_angular_rate();
	//AccelerationMilligForce amf = get_acceleration();

	//Log_Debug("\nLSM6DSO: Angular rate [degrees per second] : %4.2f, %4.2f, %4.2f\n", ardps.x, ardps.y, ardps.z);
	//Log_Debug("\nLSM6DSO: Acceleration [millig force]  : %.4lf, %.4lf, %.4lf\n", amgf.x, amgf.y, amgf.z);

	environment->temperature = get_temperature();
	environment->pressure = get_pressure();

	//light = GetLightLevel();
	environment->light = 0;

	int rnd = (rand() % 10) - 5;
	environment->humidity = (float)(50.0 + rnd);

	return true;
}

bool lp_initializeDevKit(void)
{
	srand((unsigned int)time(NULL)); // seed the random number generator for fake telemetry

	imu_initialize();

	//calibrate_angular_rate(); // call if using gyro

	//OpenADC();

	return true;
}

bool lp_closeDevKit(void)
{
	//closeI2c();
	return true;
}
