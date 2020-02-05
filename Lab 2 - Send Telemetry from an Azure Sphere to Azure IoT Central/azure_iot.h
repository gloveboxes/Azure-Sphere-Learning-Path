#ifndef iot_hub_h
#define iot_hub_h

#include "globals.h"
#include "parson.h"
#include <applibs/log.h>
#include <applibs/networking.h>
#include <azure_sphere_provisioning.h>
#include <errno.h>
#include <iothub_client_options.h>
#include <iothub_device_client_ll.h>
#include <stdio.h>
#include <stdlib.h>

bool SendMsg(const char* msg);
void AzureDoWorkTimerEventHandler(EventData*);
void InitAzureIoT(int epollFd);
void CloseAzureIoT(void);
void InitDeviceTwins(DeviceTwinPeripheral* deviceTwins[], size_t deviceTwinCount);
void InitDirectMethods(DirectMethodPeripheral* directMethods[], size_t directMethodCount);

#endif