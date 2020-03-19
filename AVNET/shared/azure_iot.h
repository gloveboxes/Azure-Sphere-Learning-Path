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

//extern IOTHUB_DEVICE_CLIENT_LL_HANDLE iothubClientHandle;

bool SendMsg(const char* msg);
void AzureCloudToDeviceHandler(EventLoopTimer*);
void StartCloudToDevice(void);
void StopCloudToDevice(void);
void SetConnectionString(const char* connectionString); // Note, do not use Connection Strings for Production - this is here for lab workaround
IOTHUB_DEVICE_CLIENT_LL_HANDLE GetAzureIotClientHandle(void);
bool ConnectToAzureIot(void);
bool IsNetworkReady(void);

#endif