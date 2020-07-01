#pragma once

#include "globals.h"
#include "exit_codes.h"

void lp_registerTerminationHandler(void);
void lp_terminationHandler(int signalNumber);
void lp_terminate(int exitCode);
bool lp_isTerminationRequired(void);
int lp_getTerminationExitCode(void);