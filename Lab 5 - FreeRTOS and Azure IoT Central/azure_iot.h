#ifndef iot_hub_h
#define iot_hub_h

#include "iothubtransportmqtt.h"
#include "parson.h"
#include "peripheral.h"
#include "utilities.h"
#include <applibs/log.h>
#include <applibs/networking.h>
#include <azure_sphere_provisioning.h>
#include <errno.h>
#include <iothub_client_options.h>
#include <iothub_device_client_ll.h>
#include <stdio.h>
#include <stdlib.h>

enum DirectMethodResponseCode
{
	METHOD_SUCCEEDED = 200,
	METHOD_FAILED = 500,
	METHOD_NOT_FOUND = 404
};

typedef enum DirectMethodResponseCode MethodResponseCode;

bool SendMsg(const char* msg);
void AzureCloudToDeviceHandler(EventData*);
void EnableCloudToDevice(void);
void DisableCloudToDevice(void);
void EnableDeviceTwins(DeviceTwinPeripheral* deviceTwins[], size_t deviceTwinCount);
void EnableDirectMethods(DirectMethodPeripheral* directMethods[], size_t directMethodCount);
void SetConnectionString(const char* connectionString); // Note, do not use Connection Strings for Production - this is here for lab workaround

#endif