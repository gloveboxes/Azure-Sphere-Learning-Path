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

#pragma region Azure IoT Hub/IoT Central

bool SendMsg(const char* msg);
const char* getAzureSphereProvisioningResultString(AZURE_SPHERE_PROV_RETURN_VALUE provisioningResult);
const char* GetReasonString(IOTHUB_CLIENT_CONNECTION_STATUS_REASON reason);
void SendMessageCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT, void* );
bool SetupAzureClient(void);
void HubConnectionStatusCallback(IOTHUB_CLIENT_CONNECTION_STATUS, IOTHUB_CLIENT_CONNECTION_STATUS_REASON, void*);
void AzureDoWorkTimerEventHandler(EventData*);

#pragma endregion

#pragma region Device Twins

void TwinCallback(DEVICE_TWIN_UPDATE_STATE updateState, const unsigned char* payload, size_t payloadSize, void* userContextCallback);
void SetDesiredState(JSON_Object* desiredProperties, DeviceTwinPeripheral* deviceTwinPeripheral);
void TwinReportState(const char* propertyName, bool propertyValue);
void ReportStatusCallback(int result, void* context);
void InitDeviceTwins(DeviceTwinPeripheral* deviceTwins[], size_t deviceTwinCount);

#pragma endregion

#pragma region Direct Methods

int AzureDirectMethodHandler(const char* method_name, const unsigned char* payload, size_t payloadSize,
	unsigned char** responsePayload, size_t* responsePayloadSize, void* userContextCallback);

#pragma endregion

#endif