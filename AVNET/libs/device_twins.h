#ifndef device_twins_h
#define device_twins_h

#include "azure_iot.h"
#include "parson.h"
#include "peripheral.h"
#include <iothub_device_client_ll.h>

#define DEVICE_TWIN_REPORT_LEN 50

typedef enum {
	TYPE_UNKNOWN = 0,
	TYPE_BOOL = 1,
	TYPE_FLOAT = 2,
	TYPE_INT = 3,
	TYPE_STRING = 4
} valueType;

struct _deviceTwinBinding {
	const char* twinProperty;
	void* twinState;
	valueType twinType;
	void (*handler)(struct _deviceTwinBinding* deviceTwinBinding);
};

typedef struct _deviceTwinBinding DeviceTwinBinding;

void TwinCallback(DEVICE_TWIN_UPDATE_STATE updateState, const unsigned char* payload, size_t payloadSize, void* userContextCallback);
void DeviceTwinsReportStatusCallback(int result, void* context);

void OpenDeviceTwinSet(DeviceTwinBinding* deviceTwins[], size_t deviceTwinCount);
void CloseDeviceTwinSet(void);

void OpenDeviceTwin(DeviceTwinBinding* deviceTwinBinding);
void CloseDeviceTwin(DeviceTwinBinding* deviceTwinBinding);
bool DeviceTwinReportState(DeviceTwinBinding* deviceTwinBinding, void* state);

#endif