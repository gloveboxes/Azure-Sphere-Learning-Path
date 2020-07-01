#include "board.h"

/// <summary>
///     Reads telemetry and returns the length of JSON data.
/// </summary>
int lp_readTelemetry(char * msgBuffer, size_t bufferLen) {
	static int msgId = 0;
	float temperature;
	float humidity;	

	int rnd = (rand() % 10) - 5;
	temperature = (float)(25.0 + rnd);
	humidity = (float)(50.0 + rnd);

	static const char* MsgTemplate = "{ \"Temperature\": \"%3.2f\", \"Humidity\": \"%3.1f\", \"MsgId\":%d }";
	return snprintf(msgBuffer, bufferLen, MsgTemplate, temperature, humidity, msgId++);
}

bool lp_initializeDevKit(void) {

	srand((unsigned int)time(NULL)); // seed the random number generator for fake telemetry

	return true;
}

bool lp_closeDevKit(void) {

	return true;
}