#include "device_twins.h"

void SetDesiredState(JSON_Object* desiredProperties, DeviceTwinPeripheral* deviceTwinPeripheral);
void TwinReportState(DeviceTwinPeripheral* deviceTwinPeripheral);
void DeviceTwinsReportStatusCallback(int result, void* context);
void DeviceTwinUpdateReportedState(char* reportedPropertiesString);

DeviceTwinPeripheral** _deviceTwins = NULL;
size_t _deviceTwinCount = 0;


void OpenDeviceTwinSet(DeviceTwinPeripheral* deviceTwins[], size_t deviceTwinCount) {
	_deviceTwins = deviceTwins;
	_deviceTwinCount = deviceTwinCount;

	for (int i = 0; i < _deviceTwinCount; i++) { 
		OpenDeviceTwin(_deviceTwins[i]); 
	}
}

void CloseDeviceTwinSet(void) {
	for (int i = 0; i < _deviceTwinCount; i++) { CloseDeviceTwin(_deviceTwins[i]); }
}

void OpenDeviceTwin(DeviceTwinPeripheral* deviceTwinPeripheral) {

	// types JSON and String allocated dynamically when called in azure_iot.c
	switch (deviceTwinPeripheral->twinType)
	{
	case TYPE_INT:
		deviceTwinPeripheral->twinState = malloc(sizeof(int));
		break;
	case TYPE_FLOAT:
		deviceTwinPeripheral->twinState = malloc(sizeof(float));
		break;
	case TYPE_BOOL:
		deviceTwinPeripheral->twinState = malloc(sizeof(bool));
		break;
	default:
		break;
	}

	if (deviceTwinPeripheral->peripheral.initialise != NULL) {
		deviceTwinPeripheral->peripheral.initialise(&deviceTwinPeripheral->peripheral);
	}
}

void CloseDeviceTwin(DeviceTwinPeripheral* deviceTwinPeripheral) {
	if (deviceTwinPeripheral->twinState != NULL) {
		free(deviceTwinPeripheral->twinState);
		deviceTwinPeripheral->twinState = NULL;
	}

	CloseFdAndPrintError(deviceTwinPeripheral->peripheral.fd, deviceTwinPeripheral->peripheral.name);
}

/// <summary>
///     Callback invoked when a Device Twin update is received from IoT Hub.
///     Updates local state for 'showEvents' (bool).
/// </summary>
/// <param name="payload">contains the Device Twin JSON document (desired and reported)</param>
/// <param name="payloadSize">size of the Device Twin JSON document</param>
void TwinCallback(DEVICE_TWIN_UPDATE_STATE updateState, const unsigned char* payload,
	size_t payloadSize, void* userContextCallback)
{
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
	free(payLoadString);
	payLoadString = NULL;
}

/// <summary>
///     Checks to see if the device twin twinProperty(name) is found in the json object. If yes, then act upon the request
/// </summary>
void SetDesiredState(JSON_Object* jsonObject, DeviceTwinPeripheral* deviceTwinPeripheral) {
	//JSON_Object* jsonObject = NULL;

	switch (deviceTwinPeripheral->twinType)
	{
	case TYPE_INT:
		*(int*)deviceTwinPeripheral->twinState = (int)json_object_get_number(jsonObject, "value");
		if (deviceTwinPeripheral->handler != NULL) {
			deviceTwinPeripheral->handler(deviceTwinPeripheral);
		}
		TwinReportState(deviceTwinPeripheral);
		break;
	case TYPE_FLOAT:
		*(float*)deviceTwinPeripheral->twinState = (float)json_object_get_number(jsonObject, "value");
		if (deviceTwinPeripheral->handler != NULL) {
			deviceTwinPeripheral->handler(deviceTwinPeripheral);
		}
		TwinReportState(deviceTwinPeripheral);
		break;
	case TYPE_BOOL:
		*(bool*)deviceTwinPeripheral->twinState = (bool)json_object_get_boolean(jsonObject, "value");
		if (deviceTwinPeripheral->handler != NULL) {
			deviceTwinPeripheral->handler(deviceTwinPeripheral);
		}
		TwinReportState(deviceTwinPeripheral);
		break;
	case TYPE_STRING:
		deviceTwinPeripheral->twinState = (char*)json_object_get_string(jsonObject, "value");

		if (deviceTwinPeripheral->handler != NULL) {
			deviceTwinPeripheral->handler(deviceTwinPeripheral);
		}

		TwinReportState(deviceTwinPeripheral);
		deviceTwinPeripheral->twinState = NULL;
		break;
	default:
		break;
	}
}


void TwinReportState(DeviceTwinPeripheral* deviceTwinPeripheral)
{
	int len = 0;

	if (iothubClientHandle == NULL) {
		Log_Debug("ERROR: client not initialized\n");
	}
	else {
		static char reportedPropertiesString[DEVICE_TWIN_REPORT_LEN] = { 0 };

		switch (deviceTwinPeripheral->twinType)
		{
		case TYPE_INT:
			len = snprintf(reportedPropertiesString, DEVICE_TWIN_REPORT_LEN, "{\"%s\":%d}", deviceTwinPeripheral->twinProperty,
				(*(int*)deviceTwinPeripheral->twinState));
			break;
		case TYPE_FLOAT:
			len = snprintf(reportedPropertiesString, DEVICE_TWIN_REPORT_LEN, "{\"%s\":%f}", deviceTwinPeripheral->twinProperty,
				(*(float*)deviceTwinPeripheral->twinState));
			break;
		case TYPE_BOOL:
			len = snprintf(reportedPropertiesString, DEVICE_TWIN_REPORT_LEN, "{\"%s\":%s}", deviceTwinPeripheral->twinProperty,
				(*(bool*)deviceTwinPeripheral->twinState ? "true" : "false"));
			break;
		case TYPE_STRING:
			len = snprintf(reportedPropertiesString, DEVICE_TWIN_REPORT_LEN, "{\"%s\":\"%s\"}", deviceTwinPeripheral->twinProperty,
				(deviceTwinPeripheral->twinState));
			break;
		default:
			break;
		}

		if (len == 0) { return; }

		DeviceTwinUpdateReportedState(reportedPropertiesString);
	}
}

void DeviceTwinUpdateReportedState(char* reportedPropertiesString) {
	if (IoTHubDeviceClient_LL_SendReportedState(
		iothubClientHandle, (unsigned char*)reportedPropertiesString,
		strlen(reportedPropertiesString), DeviceTwinsReportStatusCallback, 0) != IOTHUB_CLIENT_OK) {
		Log_Debug("ERROR: failed to set reported state for '%s'.\n", reportedPropertiesString);
	}
	else {
		Log_Debug("INFO: Reported state updated '%s'.\n", reportedPropertiesString);
	}
}


/// <summary>
///     Callback invoked when the Device Twin reported properties are accepted by IoT Hub.
/// </summary>
void DeviceTwinsReportStatusCallback(int result, void* context)
{
	Log_Debug("INFO: Device Twin reported properties update result: HTTP status code %d\n", result);
}
