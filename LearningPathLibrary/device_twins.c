#include "device_twins.h"

static void setDesiredState(JSON_Object* desiredProperties, LP_DEVICE_TWIN_BINDING* deviceTwinBinding);
static bool deviceTwinUpdateReportedState(char* reportedPropertiesString);
static bool deviceTwinReportState(LP_DEVICE_TWIN_BINDING* deviceTwinBinding, void* state, bool deviceTwinAcknowledgment, LP_DEVICE_TWIN_RESPONSE_CODE statusCode);


static LP_DEVICE_TWIN_BINDING** _deviceTwins = NULL;
static size_t _deviceTwinCount = 0;


void lp_deviceTwinSetOpen(LP_DEVICE_TWIN_BINDING* deviceTwins[], size_t deviceTwinCount) {
	_deviceTwins = deviceTwins;
	_deviceTwinCount = deviceTwinCount;

	for (int i = 0; i < _deviceTwinCount; i++) {
		lp_deviceTwinOpen(_deviceTwins[i]);
	}
}

void lp_deviceTwinSetClose(void) {
	for (int i = 0; i < _deviceTwinCount; i++) { lp_deviceTwinClose(_deviceTwins[i]); }
}

void lp_deviceTwinOpen(LP_DEVICE_TWIN_BINDING* deviceTwinBinding) {
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
	case LP_TYPE_STRING:
		// Note no memory is allocated for string twin type as size is unknown
		break;
	default:
		break;
	}
}

void lp_deviceTwinClose(LP_DEVICE_TWIN_BINDING* deviceTwinBinding) {
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
		JSON_Value* jsonValue = json_object_get_value(desiredProperties, _deviceTwins[i]->twinProperty);
		if (jsonValue != NULL) {
			setDesiredState(desiredProperties, _deviceTwins[i]);
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
static void setDesiredState(JSON_Object* jsonObject, LP_DEVICE_TWIN_BINDING* deviceTwinBinding) {

	if (json_object_has_value_of_type(jsonObject, "$version", JSONNumber)) {
		deviceTwinBinding->twinVersion = (int)json_object_get_number(jsonObject, "$version");
	}

	switch (deviceTwinBinding->twinType) {
	case LP_TYPE_INT:
		if (json_object_has_value_of_type(jsonObject, deviceTwinBinding->twinProperty, JSONNumber)) {
			*(int*)deviceTwinBinding->twinState = (int)json_object_get_number(jsonObject, deviceTwinBinding->twinProperty);

			deviceTwinBinding->twinStateUpdated = true;

			if (deviceTwinBinding->handler != NULL) {
				deviceTwinBinding->handler(deviceTwinBinding);
			}
		}
		break;
	case LP_TYPE_FLOAT:
		if (json_object_has_value_of_type(jsonObject, deviceTwinBinding->twinProperty, JSONNumber)) {
			*(float*)deviceTwinBinding->twinState = (float)json_object_get_number(jsonObject, deviceTwinBinding->twinProperty);

			deviceTwinBinding->twinStateUpdated = true;

			if (deviceTwinBinding->handler != NULL) {
				deviceTwinBinding->handler(deviceTwinBinding);
			}
		}
		break;
	case LP_TYPE_BOOL:
		if (json_object_has_value_of_type(jsonObject, deviceTwinBinding->twinProperty, JSONBoolean)) {
			*(bool*)deviceTwinBinding->twinState = (bool)json_object_get_boolean(jsonObject, deviceTwinBinding->twinProperty);

			deviceTwinBinding->twinStateUpdated = true;

			if (deviceTwinBinding->handler != NULL) {
				deviceTwinBinding->handler(deviceTwinBinding);
			}
		}
		break;
	case LP_TYPE_STRING:
		if (json_object_has_value_of_type(jsonObject, deviceTwinBinding->twinProperty, JSONString)) {
			deviceTwinBinding->twinState = (char*)json_object_get_string(jsonObject, deviceTwinBinding->twinProperty);

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

/// <summary>
///     Sends device twin desire state acknowledgement
/// </summary>
bool lp_deviceTwinAckDesiredState(LP_DEVICE_TWIN_BINDING* deviceTwinBinding, void* state, LP_DEVICE_TWIN_RESPONSE_CODE statusCode) {
	return deviceTwinReportState(deviceTwinBinding, state, true, statusCode);
}

/// <summary>
///     device twin report state to Azure IoT Hub
/// </summary>
bool lp_deviceTwinReportState(LP_DEVICE_TWIN_BINDING* deviceTwinBinding, void* state) {
	return deviceTwinReportState(deviceTwinBinding, state, false, LP_DEVICE_TWIN_COMPLETED);
}

/// <summary>
///   Supports device twin report state and device twin ack desired state request
/// </summary>
static bool deviceTwinReportState(LP_DEVICE_TWIN_BINDING* deviceTwinBinding, void* state, bool deviceTwinAcknowledgment, LP_DEVICE_TWIN_RESPONSE_CODE statusCode) {
	int len = 0;
	size_t reportLen = 10; // initialize to 10 chars to allow for JSON and NULL termination. This is generous by a couple of bytes
	bool result = false;

	if (deviceTwinBinding == NULL) {
		return false;
	}

	if (!lp_azureConnect()) {
		return false;
	}

	reportLen += strlen(deviceTwinBinding->twinProperty); // allow for twin property name in JSON response

	if (deviceTwinBinding->twinType == LP_TYPE_STRING) {
		reportLen += strlen((char*)state);
	}
	else {
		reportLen += 20; // allow 20 chars for Int, float, and boolean serialization
	}

	// to allow for device twin acknowlegement data
	if (deviceTwinAcknowledgment) {
		reportLen += 40;
	}

	char* reportedPropertiesString = (char*)malloc(reportLen);
	if (reportedPropertiesString == NULL) {
		return false;
	}

	memset(reportedPropertiesString, 0, reportLen);

	switch (deviceTwinBinding->twinType) {
	case LP_TYPE_INT:
		*(int*)deviceTwinBinding->twinState = *(int*)state;

		if (deviceTwinAcknowledgment) {
			len = snprintf(reportedPropertiesString, reportLen, "{\"%s\":{\"value\":%d, \"ac\":%d, \"av\":%d}}",
				deviceTwinBinding->twinProperty, (*(int*)deviceTwinBinding->twinState),
				(int)statusCode, deviceTwinBinding->twinVersion);
		}
		else {
			len = snprintf(reportedPropertiesString, reportLen, "{\"%s\":%d}", 
				deviceTwinBinding->twinProperty, (*(int*)deviceTwinBinding->twinState));
		}
		break;
	case LP_TYPE_FLOAT:
		*(float*)deviceTwinBinding->twinState = *(float*)state;

		if (deviceTwinAcknowledgment) {
			len = snprintf(reportedPropertiesString, reportLen, "{\"%s\":{\"value\":%f, \"ac\":%d, \"av\":%d}}",
				deviceTwinBinding->twinProperty, (*(float*)deviceTwinBinding->twinState),
				(int)statusCode, deviceTwinBinding->twinVersion);
		}
		else {
			len = snprintf(reportedPropertiesString, reportLen, "{\"%s\":%f}", 
				deviceTwinBinding->twinProperty, (*(float*)deviceTwinBinding->twinState));
		}
		break;
	case LP_TYPE_BOOL:
		*(bool*)deviceTwinBinding->twinState = *(bool*)state;

		if (deviceTwinAcknowledgment) {
			len = snprintf(reportedPropertiesString, reportLen, "{\"%s\":{\"value\":%s, \"ac\":%d, \"av\":%d}}",
				deviceTwinBinding->twinProperty, (*(bool*)deviceTwinBinding->twinState ? "true" : "false"),
				(int)statusCode, deviceTwinBinding->twinVersion);
		}
		else {
			len = snprintf(reportedPropertiesString, reportLen, "{\"%s\":%s}",
				deviceTwinBinding->twinProperty, (*(bool*)deviceTwinBinding->twinState ? "true" : "false"));
		}
		break;
	case LP_TYPE_STRING:
		deviceTwinBinding->twinState = NULL;

		if (deviceTwinAcknowledgment) {
			len = snprintf(reportedPropertiesString, reportLen, "{\"%s\":{\"value\":\"%s\", \"ac\":%d, \"av\":%d}}",
				deviceTwinBinding->twinProperty, (char*)state,
				(int)statusCode, deviceTwinBinding->twinVersion);
		}
		else {
			len = snprintf(reportedPropertiesString, reportLen, "{\"%s\":\"%s\"}",
				deviceTwinBinding->twinProperty, (char*)state);
		}

		break;
	case LP_TYPE_UNKNOWN:
		Log_Debug("Device Twin Type Unknown");
		break;
	default:
		break;
	}

	if (len > 0) {
		result = deviceTwinUpdateReportedState(reportedPropertiesString);
	}

	if (reportedPropertiesString != NULL) {
		free(reportedPropertiesString);
		reportedPropertiesString = NULL;
	}

	return result;
}

static bool deviceTwinUpdateReportedState(char* reportedPropertiesString) {
	if (IoTHubDeviceClient_LL_SendReportedState(
		lp_azureClientHandleGet(), (unsigned char*)reportedPropertiesString,
		strlen(reportedPropertiesString), lp_deviceTwinsReportStatusCallback, 0) != IOTHUB_CLIENT_OK)
	{
#if LP_LOGGING_ENABLED
		Log_Debug("ERROR: failed to set reported state for '%s'.\n", reportedPropertiesString);
#endif

		return false;
	}
	else {
#if LP_LOGGING_ENABLED
		Log_Debug("INFO: Reported state twinStateUpdated '%s'.\n", reportedPropertiesString);
#endif

		return true;
	}

	IoTHubDeviceClient_LL_DoWork(lp_azureClientHandleGet());
}

/// <summary>
///     Callback invoked when the Device Twin reported properties are accepted by IoT Hub.
/// </summary>
void lp_deviceTwinsReportStatusCallback(int result, void* context) {
#if LP_LOGGING_ENABLED
	Log_Debug("INFO: Device Twin reported properties update result: HTTP status code %d\n", result);
#endif
}
