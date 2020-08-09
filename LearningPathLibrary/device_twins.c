#include "device_twins.h"

void SetDesiredState(JSON_Object* desiredProperties, LP_DEVICE_TWIN_BINDING* deviceTwinBinding);
void lp_deviceTwinsReportStatusCallback(int result, void* context);
bool DeviceTwinUpdateReportedState(char* reportedPropertiesString);


LP_DEVICE_TWIN_BINDING** _deviceTwins = NULL;
size_t _deviceTwinCount = 0;


void lp_openDeviceTwinSet(LP_DEVICE_TWIN_BINDING* deviceTwins[], size_t deviceTwinCount) {
	_deviceTwins = deviceTwins;
	_deviceTwinCount = deviceTwinCount;

	for (int i = 0; i < _deviceTwinCount; i++) {
		lp_openDeviceTwin(_deviceTwins[i]);
	}
}

void lp_closeDeviceTwinSet(void) {
	for (int i = 0; i < _deviceTwinCount; i++) { lp_closeDeviceTwin(_deviceTwins[i]); }
}

void lp_openDeviceTwin(LP_DEVICE_TWIN_BINDING* deviceTwinBinding) {
	if (deviceTwinBinding->twinType == LP_TYPE_UNKNOWN) {
		Log_Debug("\n\nDevice Twin '%s' missing type information.\nInclude .twinType option in LP_DEVICE_TWIN_BINDING definition.\nExample .twinType=LP_TYPE_BOOL. Valid types include LP_TYPE_BOOL, LP_TYPE_INT, LP_TYPE_FLOAT, LP_TYPE_STRING.\n\n", deviceTwinBinding->twinProperty);
		lp_terminate(ExitCode_OpenDeviceTwin);
	}

	// types JSON and String allocated dynamically when called in azure_iot.c
	switch (deviceTwinBinding->twinType) {
	case LP_TYPE_INT:
		deviceTwinBinding->twinState = malloc(sizeof(int));
		*(int*)deviceTwinBinding->twinState = 0;
		break;
	case LP_TYPE_FLOAT:
		deviceTwinBinding->twinState = malloc(sizeof(float));
		*(float*)deviceTwinBinding->twinState = 0.0f;
		break;
	case LP_TYPE_BOOL:
		deviceTwinBinding->twinState = malloc(sizeof(bool));
		*(bool*)deviceTwinBinding->twinState = false;
		break;
	default:
		break;
	}
}

void lp_closeDeviceTwin(LP_DEVICE_TWIN_BINDING* deviceTwinBinding) {
	if (deviceTwinBinding->twinState != NULL) {
		free(deviceTwinBinding->twinState);
		deviceTwinBinding->twinState = NULL;
	}
}

/// <summary>
///     Callback invoked when a Device Twin update is received from IoT Hub.
/// </summary>
/// <param name="payload">contains the Device Twin JSON document (desired and reported)</param>
/// <param name="payloadSize">size of the Device Twin JSON document</param>
void lp_twinCallback(DEVICE_TWIN_UPDATE_STATE updateState, const unsigned char* payload,
	size_t payloadSize, void* userContextCallback) {
	JSON_Value* root_value = NULL;
	JSON_Object* root_object = NULL;

	char* payLoadString = (char*)malloc(payloadSize + 1);
	if (payLoadString == NULL) {
		goto cleanup;
	}

	memcpy(payLoadString, payload, payloadSize);
	payLoadString[payloadSize] = 0; //null terminate string

	root_value = json_parse_string(payLoadString);
	if (root_value == NULL) {
		goto cleanup;
	}

	root_object = json_value_get_object(root_value);
	if (root_object == NULL) {
		goto cleanup;
	}

	JSON_Object* desiredProperties = json_object_dotget_object(root_object, "desired");
	if (desiredProperties == NULL) {
		desiredProperties = root_object;
	}

	for (int i = 0; i < _deviceTwinCount; i++) {
		JSON_Object* currentJSONProperties = json_object_dotget_object(desiredProperties, _deviceTwins[i]->twinProperty);
		if (currentJSONProperties != NULL) {
			SetDesiredState(currentJSONProperties, _deviceTwins[i]);
		}
	}

cleanup:
	// Release the allocated memory.
	if (root_value != NULL) {
		json_value_free(root_value);
	}

	if (payLoadString != NULL) {
		free(payLoadString);
		payLoadString = NULL;
	}
}

/// <summary>
///     Checks to see if the device twin twinProperty(name) is found in the json object. If yes, then act upon the request
/// </summary>
void SetDesiredState(JSON_Object* jsonObject, LP_DEVICE_TWIN_BINDING* deviceTwinBinding) {

	switch (deviceTwinBinding->twinType) {
	case LP_TYPE_INT:
		if (json_object_has_value_of_type(jsonObject, "value", JSONNumber)) {
			*(int*)deviceTwinBinding->twinState = (int)json_object_get_number(jsonObject, "value");

			deviceTwinBinding->twinStateUpdated = true;

			if (deviceTwinBinding->handler != NULL) {
				deviceTwinBinding->handler(deviceTwinBinding);
			}
		}
		break;
	case LP_TYPE_FLOAT:
		if (json_object_has_value_of_type(jsonObject, "value", JSONNumber)) {
			*(float*)deviceTwinBinding->twinState = (float)json_object_get_number(jsonObject, "value");

			deviceTwinBinding->twinStateUpdated = true;

			if (deviceTwinBinding->handler != NULL) {
				deviceTwinBinding->handler(deviceTwinBinding);
			}
		}
		break;
	case LP_TYPE_BOOL:
		if (json_object_has_value_of_type(jsonObject, "value", JSONBoolean)) {
			*(bool*)deviceTwinBinding->twinState = (bool)json_object_get_boolean(jsonObject, "value");

			deviceTwinBinding->twinStateUpdated = true;

			if (deviceTwinBinding->handler != NULL) {
				deviceTwinBinding->handler(deviceTwinBinding);
			}
		}
		break;
	case LP_TYPE_STRING:
		if (json_object_has_value_of_type(jsonObject, "value", JSONString)) {
			deviceTwinBinding->twinState = (char*)json_object_get_string(jsonObject, "value");

			if (deviceTwinBinding->handler != NULL) {
				deviceTwinBinding->handler(deviceTwinBinding);
			}
			deviceTwinBinding->twinState = NULL;
		}
		break;
	default:
		break;
	}
}

bool lp_deviceTwinReportState(LP_DEVICE_TWIN_BINDING* deviceTwinBinding, void* state) {
	int len = 0;
	size_t reportLen = 10; // initialize to 10 chars to allow for JSON and NULL termination. This is generous by a couple of bytes
	bool result = false;

	if (deviceTwinBinding == NULL) {
		return false;
	}

	if (!lp_connectToAzureIot()) {
		return false;
	}

	reportLen += strlen(deviceTwinBinding->twinProperty); // allow for twin property name in JSON response

	if (deviceTwinBinding->twinType == LP_TYPE_STRING) {
		reportLen += strlen((char*)state);
	}
	else {
		reportLen += 20; // allow 20 chars for Int, float, and boolean serialization
	}

	char* reportedPropertiesString = (char*)malloc(reportLen);
	if (reportedPropertiesString == NULL) {
		return false;
	}

	memset(reportedPropertiesString, 0, reportLen);

	switch (deviceTwinBinding->twinType) {
	case LP_TYPE_INT:
		*(int*)deviceTwinBinding->twinState = *(int*)state;
		len = snprintf(reportedPropertiesString, reportLen, "{\"%s\":%d}", deviceTwinBinding->twinProperty,
			(*(int*)deviceTwinBinding->twinState));
		break;
	case LP_TYPE_FLOAT:
		*(float*)deviceTwinBinding->twinState = *(float*)state;
		len = snprintf(reportedPropertiesString, reportLen, "{\"%s\":%f}", deviceTwinBinding->twinProperty,
			(*(float*)deviceTwinBinding->twinState));
		break;
	case LP_TYPE_BOOL:
		*(bool*)deviceTwinBinding->twinState = *(bool*)state;
		len = snprintf(reportedPropertiesString, reportLen, "{\"%s\":%s}", deviceTwinBinding->twinProperty,
			(*(bool*)deviceTwinBinding->twinState ? "true" : "false"));
		break;
	case LP_TYPE_STRING:
		deviceTwinBinding->twinState = NULL;
		len = snprintf(reportedPropertiesString, reportLen, "{\"%s\":\"%s\"}", deviceTwinBinding->twinProperty, (char*)state);
		break;
	case LP_TYPE_UNKNOWN:
		Log_Debug("Device Twin Type Unknown");
		break;
	default:
		break;
	}

	if (len > 0) {
		result = DeviceTwinUpdateReportedState(reportedPropertiesString);
	}

	if (reportedPropertiesString != NULL) {
		free(reportedPropertiesString);
		reportedPropertiesString = NULL;
	}

	return result;
}


bool DeviceTwinUpdateReportedState(char* reportedPropertiesString) {
	if (IoTHubDeviceClient_LL_SendReportedState(
		lp_getAzureIotClientHandle(), (unsigned char*)reportedPropertiesString,
		strlen(reportedPropertiesString), lp_deviceTwinsReportStatusCallback, 0) != IOTHUB_CLIENT_OK) 
	{
		Log_Debug("ERROR: failed to set reported state for '%s'.\n", reportedPropertiesString);
		return false;
	}
	else {
		Log_Debug("INFO: Reported state twinStateUpdated '%s'.\n", reportedPropertiesString);
		return true;
	}

	IoTHubDeviceClient_LL_DoWork(lp_getAzureIotClientHandle());
}


/// <summary>
///     Callback invoked when the Device Twin reported properties are accepted by IoT Hub.
/// </summary>
void lp_deviceTwinsReportStatusCallback(int result, void* context) {
	Log_Debug("INFO: Device Twin reported properties update result: HTTP status code %d\n", result);
}
