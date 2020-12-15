#pragma once

#include "azure_iot.h"
#include "peripheral_gpio.h"

typedef enum 
{
	LP_METHOD_SUCCEEDED = 200,
	LP_METHOD_FAILED = 500,
	LP_METHOD_NOT_FOUND = 404
} LP_DIRECT_METHOD_RESPONSE_CODE;

struct _directMethodBinding {
	const char* methodName;
	LP_DIRECT_METHOD_RESPONSE_CODE(*handler)(JSON_Object* json, struct _directMethodBinding* peripheral, char** responseMsg);
};

typedef struct _directMethodBinding LP_DIRECT_METHOD_BINDING;

void lp_openDirectMethodSet(LP_DIRECT_METHOD_BINDING* directMethods[], size_t directMethodCount);
void lp_closeDirectMethodSet(void);
int lp_azureDirectMethodHandler(const char* method_name, const unsigned char* payload, size_t payloadSize,
	unsigned char** responsePayload, size_t* responsePayloadSize, void* userContextCallback);
