#include "globals.h"


char scopeId[SCOPEID_LENGTH];
char rtAppComponentId[RT_APP_COMPONENT_LENGTH];  //initialized from cmdline argument
//volatile sig_atomic_t terminationRequired = false;
bool realTelemetry = false;		// Generate fake telemetry or use Seeed Studio Grove SHT31 Sensor


void lp_processCmdArgs(int argc, char* argv[]) {
	do {
		argc--;
		switch (argc)
		{
		case 1:
			strncpy(scopeId, argv[1], SCOPEID_LENGTH);
			break;
		case 2:
			strncpy(rtAppComponentId, argv[2], RT_APP_COMPONENT_LENGTH);
			break;
		}
	} while (argc > 1);
}

char* lp_getCurrentUtc(char* buffer, size_t bufferSize) {
	time_t now = time(NULL);
	struct tm* t = gmtime(&now);
	strftime(buffer, bufferSize - 1, "%Y-%d-%m %H:%M:%S", t);
	return buffer;
}