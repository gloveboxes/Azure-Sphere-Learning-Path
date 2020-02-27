#ifndef device_twins_h
#define device_twins_h

#include "azure_iot.h"
#include "parson.h"
#include "peripheral.h"
#include <iothub_device_client_ll.h>

typedef enum {
	TYPE_UNKNOWN = 0,
	TYPE_BOOL = 1,
	TYPE_FLOAT = 2,
	TYPE_INT = 3,
	TYPE_STRING = 4
} valueType;

struct _deviceTwinPeripheral {
	Peripheral peripheral;
	const char* twinProperty;
	void* twinState;
	valueType twinType;
	void (*handler)(struct _deviceTwinPeripheral* deviceTwinPeripheral);
};

typedef struct _deviceTwinPeripheral DeviceTwinPeripheral;

void TwinCallback(DEVICE_TWIN_UPDATE_STATE updateState, const unsigned char* payload, size_t payloadSize, void* userContextCallback);
void DeviceTwinsReportStatusCallback(int result, void* context);

void OpenDeviceTwinSet(DeviceTwinPeripheral* deviceTwins[], size_t deviceTwinCount);
void CloseDeviceTwinSet(void);

void OpenDeviceTwin(DeviceTwinPeripheral* deviceTwinPeripheral);
void CloseDeviceTwin(DeviceTwinPeripheral* deviceTwinPeripheral);

#endif