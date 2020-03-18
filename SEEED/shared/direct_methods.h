#pragma once

#include "azure_iot.h"
#include "peripheral.h"

typedef enum 
{
	METHOD_SUCCEEDED = 200,
	METHOD_FAILED = 500,
	METHOD_NOT_FOUND = 404
} DirectMethodResponseCode;

struct _directMethodBinding {
	Peripheral peripheral;
	const char* methodName;
	DirectMethodResponseCode(*handler)(JSON_Object* json, struct _directMethodBinding* peripheral, char** responseMsg);
};

typedef struct _directMethodBinding DirectMethodBinding;

void OpenDirectMethodSet(DirectMethodBinding* directMethods[], size_t directMethodCount);
void CloseDirectMethodSet(void);
int AzureDirectMethodHandler(const char* method_name, const unsigned char* payload, size_t payloadSize,
	unsigned char** responsePayload, size_t* responsePayloadSize, void* userContextCallback);
