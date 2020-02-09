#ifndef iot_hub_h
#define iot_hub_h

#include "parson.h"
#include "utilities.h"
#include <applibs/log.h>
#include <applibs/networking.h>
#include <azure_sphere_provisioning.h>
#include <errno.h>
#include <iothub_client_options.h>
#include <iothub_device_client_ll.h>
#include <stdio.h>
#include <stdlib.h>

bool SendMsg(const char* msg);
void AzureCloudToDeviceHandler(EventData*);
void EnableCloudToDevice(void);
void DisableCloudToDevice(void);
void EnableDeviceTwins(DeviceTwinPeripheral* deviceTwins[], size_t deviceTwinCount);
void EnableDirectMethods(DirectMethodPeripheral* directMethods[], size_t directMethodCount);

#endif