#ifndef iot_hub_h
#define iot_hub_h

#include "device_twins.h"
#include "direct_methods.h"
#include "globals.h"
#include "iothubtransportmqtt.h"
#include "terminate.h"
#include "timer.h"
#include <applibs/log.h>
#include <applibs/networking.h>
#include <azure_sphere_provisioning.h>
#include <errno.h>
#include <iothub_client_options.h>
#include <iothub_device_client_ll.h>
#include <stdio.h>
#include <stdlib.h>

#define DEVICE_TWIN_REPORT_LEN 50

extern IOTHUB_DEVICE_CLIENT_LL_HANDLE iothubClientHandle;

bool SendMsg(const char* msg);
void AzureCloudToDeviceHandler(EventLoopTimer*);
void EnableCloudToDevice(void);
void DisableCloudToDevice(void);
void SetConnectionString(const char* connectionString); // Note, do not use Connection Strings for Production - this is here for lab workaround

#endif