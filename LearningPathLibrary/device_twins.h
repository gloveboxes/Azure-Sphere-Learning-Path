#pragma once

#include "azure_iot.h"
#include "parson.h"
#include "peripheral_gpio.h"
#include <iothub_device_client_ll.h>

typedef enum {
	LP_TYPE_UNKNOWN = 0,
	LP_TYPE_BOOL = 1,
	LP_TYPE_FLOAT = 2,
	LP_TYPE_INT = 3,
	LP_TYPE_STRING = 4
} LP_DEVICE_TWIN_TYPE;

struct _deviceTwinBinding {
	const char* twinProperty;
	void* twinState;
	bool twinStateUpdated;
	LP_DEVICE_TWIN_TYPE twinType;
	void (*handler)(struct _deviceTwinBinding* deviceTwinBinding);
};

typedef struct _deviceTwinBinding LP_DEVICE_TWIN_BINDING;

void lp_twinCallback(DEVICE_TWIN_UPDATE_STATE updateState, const unsigned char* payload, size_t payloadSize, void* userContextCallback);
void lp_deviceTwinsReportStatusCallback(int result, void* context);

void lp_openDeviceTwinSet(LP_DEVICE_TWIN_BINDING* deviceTwins[], size_t deviceTwinCount);
void lp_closeDeviceTwinSet(void);

void lp_openDeviceTwin(LP_DEVICE_TWIN_BINDING* deviceTwinBinding);
void lp_closeDeviceTwin(LP_DEVICE_TWIN_BINDING* deviceTwinBinding);
bool lp_deviceTwinReportState(LP_DEVICE_TWIN_BINDING* deviceTwinBinding, void* state);
