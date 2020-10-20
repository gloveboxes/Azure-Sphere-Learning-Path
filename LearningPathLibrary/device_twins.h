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
	int twinVersion;
	bool twinStateUpdated;
	LP_DEVICE_TWIN_TYPE twinType;
	void (*handler)(struct _deviceTwinBinding* deviceTwinBinding);
};

typedef enum
{
	LP_DEVICE_TWIN_COMPLETED = 200,
	LP_DEVICE_TWIN_ERROR = 500,
	LP_DEVICE_TWIN_INVALID = 404
} LP_DEVICE_TWIN_RESPONSE_CODE;

typedef struct _deviceTwinBinding LP_DEVICE_TWIN_BINDING;

bool lp_deviceTwinAckDesiredState(LP_DEVICE_TWIN_BINDING* deviceTwinBinding, void* state, LP_DEVICE_TWIN_RESPONSE_CODE statusCode);
bool lp_deviceTwinReportState(LP_DEVICE_TWIN_BINDING* deviceTwinBinding, void* state);
void lp_deviceTwinClose(LP_DEVICE_TWIN_BINDING* deviceTwinBinding);
void lp_deviceTwinOpen(LP_DEVICE_TWIN_BINDING* deviceTwinBinding);
void lp_deviceTwinSetClose(void);
void lp_deviceTwinSetOpen(LP_DEVICE_TWIN_BINDING* deviceTwins[], size_t deviceTwinCount);
void lp_deviceTwinsReportStatusCallback(int result, void* context);
void lp_twinCallback(DEVICE_TWIN_UPDATE_STATE updateState, const unsigned char* payload, size_t payloadSize, void* userContextCallback);