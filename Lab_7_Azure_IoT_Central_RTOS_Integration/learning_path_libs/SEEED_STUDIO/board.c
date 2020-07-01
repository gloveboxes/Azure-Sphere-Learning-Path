#include "board.h"

/// <summary>
///     Reads telemetry and returns the length of JSON data.
/// </summary>
bool lp_readTelemetry(LP_ENVIRONMENT* environment)
{
	int rnd = (rand() % 10) - 5;
	environment->temperature = (float)(25.0 + rnd);
	environment->humidity = (float)(50.0 + rnd);

	rnd = (rand() % 50) - 25;
	environment->pressure = (float)(1000.0 + rnd);
	environment->light = 0;

	return true;
}

bool lp_initializeDevKit(void) {

	srand((unsigned int)time(NULL)); // seed the random number generator for fake telemetry

	return true;
}

bool lp_closeDevKit(void) {

	return true;
}