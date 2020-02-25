#ifndef device_twins_h
#define device_twins_h

#include "azure_iot.h"
#include "parson.h"
#include "peripheral.h"
#include <iothub_device_client_ll.h>

void EnableDeviceTwins(DeviceTwinPeripheral* deviceTwins[], size_t deviceTwinCount);
void TwinCallback(DEVICE_TWIN_UPDATE_STATE updateState, const unsigned char* payload, size_t payloadSize, void* userContextCallback);
void DeviceTwinsReportStatusCallback(int result, void* context);

#endif