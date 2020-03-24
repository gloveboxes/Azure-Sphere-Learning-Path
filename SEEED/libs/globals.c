#include "globals.h"


char scopeId[SCOPEID_LENGTH];
char rtAppComponentId[RT_APP_COMPONENT_LENGTH];  //initialized from cmdline argument
//volatile sig_atomic_t terminationRequired = false;
bool realTelemetry = false;		// Generate fake telemetry or use Seeed Studio Grove SHT31 Sensor


void ProcessCmdArgs(int argc, char* argv[]) {
	do {
		argc--;
		switch (argc)
		{
		case 1:
			strncpy(scopeId, argv[2], SCOPEID_LENGTH);
			break;
		case 2:
			strncpy(rtAppComponentId, argv[3], RT_APP_COMPONENT_LENGTH);
			break;
		}
	} while (argc > 1);
}