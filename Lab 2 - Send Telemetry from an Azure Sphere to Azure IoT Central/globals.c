#include "globals.h"

char scopeId[SCOPEID_LENGTH];

volatile sig_atomic_t terminationRequired = false;
bool realTelemetry = false;		// Generate fake telemetry or use Seeed Studio Grove SHT31 Sensor
