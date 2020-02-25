#include "globals.h"

void RegisterTerminationHandler(void);
void TerminationHandler(int signalNumber);
void Terminate(void);
bool IsTerminationRequired(void);
