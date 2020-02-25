#include "direct_methods.h"

DirectMethodPeripheral** _directMethods;
size_t _directMethodCount;

void EnableDirectMethods(DirectMethodPeripheral* directMethods[], size_t directMethodCount) {
	_directMethods = directMethods;
	_directMethodCount = directMethodCount;
}


/*
This implementation of Direct Methods expects a JSON Payload Object
*/
int AzureDirectMethodHandler(const char* method_name, const unsigned char* payload, size_t payloadSize,
	unsigned char** responsePayload, size_t* responsePayloadSize, void* userContextCallback) {

	const char* methodSucceededMsg = "Method Succeeded";
	const char* methodNotFoundMsg = "Method not found";
	const char* methodErrorMsg = "Method Error";
	const char* mallocFailedMsg = "Memory Allocation failed";
	const char* invalidJsonMsg = "Invalid JSON";

	DirectMethodPeripheral* directMethodPeripheral = NULL;

	const char* responseMessage = methodNotFoundMsg;
	int result = METHOD_NOT_FOUND;

	JSON_Value* root_value = NULL;
	JSON_Object* jsonObject = NULL;

	// Prepare the payload for the response. This is a heap allocated null terminated string.
	// The Azure IoT Hub SDK is responsible of freeing it.
	*responsePayload = NULL;  // Response payload content.
	*responsePayloadSize = 0; // Response payload content size.

	char* payLoadString = (char*)malloc(payloadSize + 1);
	if (payLoadString == NULL) {
		responseMessage = mallocFailedMsg;
		result = METHOD_FAILED;
		goto cleanup;
	}

	memcpy(payLoadString, payload, payloadSize);
	payLoadString[payloadSize] = 0; //null terminate string

	root_value = json_parse_string(payLoadString);
	if (root_value == NULL) {
		responseMessage = invalidJsonMsg;
		result = METHOD_FAILED;
		goto cleanup;
	}

	jsonObject = json_value_get_object(root_value);
	if (jsonObject == NULL) {
		responseMessage = invalidJsonMsg;
		result = METHOD_FAILED;
		goto cleanup;
	}

	// loop through array of DirectMethodPeripherals looking for a matching method name
	for (int i = 0; i < _directMethodCount; i++) {
		if (strcmp(method_name, _directMethods[i]->methodName) == 0) {
			directMethodPeripheral = _directMethods[i];
			break;
		}
	}

	if (directMethodPeripheral != NULL) {	// was a DirectMethodPeripheral found
		MethodResponseCode responseCode = directMethodPeripheral->handler(jsonObject, directMethodPeripheral);

		result = (int)responseCode;

		switch (responseCode)
		{
		case METHOD_SUCCEEDED:	// 200
			responseMessage = strlen(directMethodPeripheral->responseMessage) == 0 ? methodSucceededMsg : directMethodPeripheral->responseMessage;
			break;
		case METHOD_FAILED:		// 500
			responseMessage = strlen(directMethodPeripheral->responseMessage) == 0 ? methodErrorMsg : directMethodPeripheral->responseMessage;
			break;
		case METHOD_NOT_FOUND:
			break;
		}
	}

cleanup:

	// Prepare the payload for the response. This is a heap allocated null terminated string.
	// The Azure IoT Hub SDK is responsible of freeing it.

	*responsePayloadSize = strlen(responseMessage) + 2; // add two as going to wrap the message with quotes for JSON
	*responsePayload = (unsigned char*)malloc(*responsePayloadSize);

	// response message needs to be wrapped in quotes as part of a JSON object
	strcpy((char*)(*responsePayload), "\"");
	strncpy((char*)(*responsePayload + 1), responseMessage, *responsePayloadSize - 2);
	strcpy((char*)(*responsePayload + *responsePayloadSize - 1), "\"");

	if (root_value != NULL) {
		json_value_free(root_value);
	}

	free(payLoadString);
	payLoadString = NULL;

	if (directMethodPeripheral != NULL) {
		if (directMethodPeripheral->responseMessage != NULL) { // there was memory allocated for a response message so free it now
			free(directMethodPeripheral->responseMessage);
			directMethodPeripheral->responseMessage = NULL;
		}
	}

	return result;
}