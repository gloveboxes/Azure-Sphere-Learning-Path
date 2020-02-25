#ifndef direct_methods_h
#define direct_methods_h

#include "azure_iot.h"
#include "peripheral.h"

enum DirectMethodResponseCode
{
	METHOD_SUCCEEDED = 200,
	METHOD_FAILED = 500,
	METHOD_NOT_FOUND = 404
};

typedef enum DirectMethodResponseCode MethodResponseCode;

int AzureDirectMethodHandler(const char* method_name, const unsigned char* payload, size_t payloadSize,
	unsigned char** responsePayload, size_t* responsePayloadSize, void* userContextCallback);
void EnableDirectMethods(DirectMethodPeripheral* directMethods[], size_t directMethodCount);

#endif