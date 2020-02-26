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

struct _directMethodPeripheral {
	Peripheral peripheral;
	const char* methodName;
	enum DirectMethodResponseCode(*handler)(JSON_Object* json, struct _directMethodPeripheral* peripheral);
	char* responseMessage;
};

typedef struct _directMethodPeripheral DirectMethodPeripheral;
typedef enum DirectMethodResponseCode MethodResponseCode;

void RegisterDirectMethodSet(DirectMethodPeripheral* directMethods[], size_t directMethodCount);
void CloseDirectMethodSet(void);
int AzureDirectMethodHandler(const char* method_name, const unsigned char* payload, size_t payloadSize,
	unsigned char** responsePayload, size_t* responsePayloadSize, void* userContextCallback);

#endif