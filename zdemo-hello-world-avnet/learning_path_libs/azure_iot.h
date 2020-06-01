#pragma once

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

bool lp_sendMsg(const char* msg);
void lp_startCloudToDevice(void);
void lp_stopCloudToDevice(void);
void lp_setConnectionString(const char* connectionString); // Note, do not use Connection Strings for Production - this is here for lab workaround
IOTHUB_DEVICE_CLIENT_LL_HANDLE lp_getAzureIotClientHandle(void);
bool lp_connectToAzureIot(void);
bool lp_isNetworkReady(void);
