#include "board.h"


/// <summary>
///     Reads telemetry and returns the length of JSON data.
/// </summary>
bool lp_readTelemetry(LP_ENVIRONMENT* environment)
{
	//ENSURE lp_calibrate_angular_rate(); call from lp_initializeDevKit before calling lp_get_angular_rate()

	//AngularRateDegreesPerSecond ardps = lp_get_angular_rate();
	//AccelerationMilligForce amf = lp_get_acceleration();

	//Log_Debug("\nLSM6DSO: Angular rate [degrees per second] : %4.2f, %4.2f, %4.2f\n", ardps.x, ardps.y, ardps.z);
	//Log_Debug("\nLSM6DSO: Acceleration [millig force]  : %.4lf, %.4lf, %.4lf\n", amgf.x, amgf.y, amgf.z);

	environment->temperature = lp_get_temperature();
	environment->pressure = lp_get_pressure();

	// environment->light = lp_GetLightLevel();
	environment->light = 0;

	int rnd = (rand() % 10) - 5;
	environment->humidity = (float)(50.0 + rnd);

	return true;
}

bool lp_initializeDevKit(void)
{
	srand((unsigned int)time(NULL)); // seed the random number generator for fake telemetry

	lp_imu_initialize();

	//lp_calibrate_angular_rate(); // call if using gyro

	//lp_OpenADC();

	return true;
}

bool lp_closeDevKit(void)
{
	//closeI2c();
	return true;
}
