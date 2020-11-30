#pragma once

#include "azure_prov_client/prov_client_const.h"
#include "azure_prov_client/prov_device_ll_client.h"
#include "azure_prov_client/prov_security_factory.h"
#include "azure_prov_client/prov_transport.h"
#include "azure_prov_client/prov_transport_mqtt_client.h"
#include "device_twins.h"
#include "direct_methods.h"
#include "iothubtransportmqtt.h"
#include "terminate.h"
#include "timer.h"
#include "utilities.h"
#include <applibs/log.h>
#include <azure_sphere_provisioning.h>
#include <errno.h>
#include <iothub_client_options.h>
#include <iothub_device_client_ll.h>
#include <iothubtransportmqtt.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct LP_MESSAGE_PROPERTY
{
	const char* key;
	const char* value;
} LP_MESSAGE_PROPERTY;

bool lp_azureMsgSend(const char* msg);
bool lp_azureMsgSendWithProperties(const char* msg, LP_MESSAGE_PROPERTY** messageProperties, size_t messagePropertyCount);
void lp_azureToDeviceStart(void);
void lp_azureToDeviceStop(void);
void lp_azureConnectionStringSet(const char* connectionString); // Note, do not use Connection Strings for Production - this is here for lab workaround
void lp_azureInitialize(const char* idScope, const char* deviceTwinModelId);
bool lp_azureConnect(void);
IOTHUB_DEVICE_CLIENT_LL_HANDLE lp_azureClientHandleGet(void);
