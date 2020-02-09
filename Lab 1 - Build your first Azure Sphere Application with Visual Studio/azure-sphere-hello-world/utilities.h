#include "globals.h"
#include <string.h>
#include <errno.h>
#include <applibs/log.h>


int GetEpollFd(void);
void RegisterTerminationHandler(void);
void TerminationHandler(int signalNumber);
int StartTimer(Timer* timer);
int OpenPeripheral(Peripheral* peripheral);
void ProcessCmdArgs(int argc, char* argv[]);
