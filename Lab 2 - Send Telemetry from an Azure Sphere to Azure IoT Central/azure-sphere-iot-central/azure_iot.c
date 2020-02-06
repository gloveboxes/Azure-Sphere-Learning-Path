#include "azure_iot.h"

#pragma region Azure IoT Hub/IoT Central

const char* getAzureSphereProvisioningResultString(AZURE_SPHERE_PROV_RETURN_VALUE provisioningResult);
const char* GetReasonString(IOTHUB_CLIENT_CONNECTION_STATUS_REASON reason);
void SendMessageCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT, void*);
bool SetupAzureClient(void);
void HubConnectionStatusCallback(IOTHUB_CLIENT_CONNECTION_STATUS, IOTHUB_CLIENT_CONNECTION_STATUS_REASON, void*);

#pragma endregion

#pragma region Device Twins

void TwinCallback(DEVICE_TWIN_UPDATE_STATE updateState, const unsigned char* payload, size_t payloadSize, void* userContextCallback);
void SetDesiredState(JSON_Object* desiredProperties, DeviceTwinPeripheral* deviceTwinPeripheral);
void TwinReportState(const char* propertyName, bool propertyValue);
void ReportStatusCallback(int result, void* context);

#pragma endregion

#pragma region Direct Methods

int AzureDirectMethodHandler(const char* method_name, const unsigned char* payload, size_t payloadSize,
	unsigned char** responsePayload, size_t* responsePayloadSize, void* userContextCallback);

#pragma endregion


IOTHUB_DEVICE_CLIENT_LL_HANDLE iothubClientHandle = NULL;
DeviceTwinPeripheral** _deviceTwins = NULL;
size_t _deviceTwinCount = 0;
DirectMethodPeripheral** _directMethods;
size_t _directMethodCount;
bool iothubAuthenticated = false;
const int keepalivePeriodSeconds = 20;


/// <summary>
///     Callback confirming message delivered to IoT Hub.
/// </summary>
/// <param name="result">Message delivery status</param>
/// <param name="context">User specified context</param>
void SendMessageCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* context)
{
	Log_Debug("INFO: Message received by IoT Hub. Result is: %d\n", result);
}

void AzureDoWorkTimerEventHandler(EventData* eventData) {
	if (iothubClientHandle != NULL) {
		IoTHubDeviceClient_LL_DoWork(iothubClientHandle);
	}
}

bool SendMsg(const char* msg) {
	if (strlen(msg) < 1) {
		return true;
	}

	bool isNetworkReady = false;
	if (Networking_IsNetworkingReady(&isNetworkReady) != -1) {
		if (isNetworkReady && !iothubAuthenticated) {
			if (!SetupAzureClient()) {
				return false;
			}
		}
	}
	else {
		Log_Debug("Failed to get Network state\n");
		return false;
	}

	if (iothubAuthenticated) {

		IOTHUB_MESSAGE_HANDLE messageHandle = IoTHubMessage_CreateFromString(msg);

		if (messageHandle == 0) {
			Log_Debug("WARNING: unable to create a new IoTHubMessage\n");
			return false;
		}

		if (IoTHubDeviceClient_LL_SendEventAsync(iothubClientHandle, messageHandle, SendMessageCallback,
			/*&callback_param*/ 0) != IOTHUB_CLIENT_OK) {
			Log_Debug("WARNING: failed to hand over the message to IoTHubClient\n");
			return false;
		}
		else {
			Log_Debug("INFO: IoTHubClient accepted the message for delivery\n");
		}

		IoTHubMessage_Destroy(messageHandle);

		return true;
	}
	else {
		return false;
	}
}


/// <summary>
///     Sets up the Azure IoT Hub connection (creates the iothubClientHandle)
///     When the SAS Token for a device expires the connection needs to be recreated
///     which is why this is not simply a one time call.
/// </summary>
bool SetupAzureClient()
{
	if (iothubClientHandle != NULL) {
		IoTHubDeviceClient_LL_Destroy(iothubClientHandle);
	}

	AZURE_SPHERE_PROV_RETURN_VALUE provResult = IoTHubDeviceClient_LL_CreateWithAzureSphereDeviceAuthProvisioning(scopeId, 10000, &iothubClientHandle);
	Log_Debug("IoTHubDeviceClient_LL_CreateWithAzureSphereDeviceAuthProvisioning returned '%s'.\n", getAzureSphereProvisioningResultString(provResult));

	if (provResult.result != AZURE_SPHERE_PROV_RESULT_OK) {
		Log_Debug("ERROR: failure to create IoTHub Handle.");
		return false;
	}

	iothubAuthenticated = true;

	if (IoTHubDeviceClient_LL_SetOption(iothubClientHandle, OPTION_KEEP_ALIVE, &keepalivePeriodSeconds) != IOTHUB_CLIENT_OK) {
		Log_Debug("ERROR: failure setting option \"%s\"\n", OPTION_KEEP_ALIVE);
		return false;
	}

	IoTHubDeviceClient_LL_SetDeviceTwinCallback(iothubClientHandle, TwinCallback, NULL);
	IoTHubDeviceClient_LL_SetDeviceMethodCallback(iothubClientHandle, AzureDirectMethodHandler, NULL);
	IoTHubDeviceClient_LL_SetConnectionStatusCallback(iothubClientHandle, HubConnectionStatusCallback, NULL);

	return true;
}

/// <summary>
///     Sets the IoT Hub authentication state for the app
///     The SAS Token expires which will set the authentication state
/// </summary>
void HubConnectionStatusCallback(IOTHUB_CLIENT_CONNECTION_STATUS result, IOTHUB_CLIENT_CONNECTION_STATUS_REASON reason, void* userContextCallback)
{
	iothubAuthenticated = (result == IOTHUB_CLIENT_CONNECTION_AUTHENTICATED);
	Log_Debug("IoT Hub Authenticated: %s\n", GetReasonString(reason));
}

/// <summary>
///     Converts AZURE_SPHERE_PROV_RETURN_VALUE to a string.
/// </summary>
const char* getAzureSphereProvisioningResultString(
	AZURE_SPHERE_PROV_RETURN_VALUE provisioningResult)
{
	switch (provisioningResult.result) {
	case AZURE_SPHERE_PROV_RESULT_OK:
		return "AZURE_SPHERE_PROV_RESULT_OK";
	case AZURE_SPHERE_PROV_RESULT_INVALID_PARAM:
		return "AZURE_SPHERE_PROV_RESULT_INVALID_PARAM";
	case AZURE_SPHERE_PROV_RESULT_NETWORK_NOT_READY:
		return "AZURE_SPHERE_PROV_RESULT_NETWORK_NOT_READY";
	case AZURE_SPHERE_PROV_RESULT_DEVICEAUTH_NOT_READY:
		return "AZURE_SPHERE_PROV_RESULT_DEVICEAUTH_NOT_READY";
	case AZURE_SPHERE_PROV_RESULT_PROV_DEVICE_ERROR:
		return "AZURE_SPHERE_PROV_RESULT_PROV_DEVICE_ERROR";
	case AZURE_SPHERE_PROV_RESULT_GENERIC_ERROR:
		return "AZURE_SPHERE_PROV_RESULT_GENERIC_ERROR";
	default:
		return "UNKNOWN_RETURN_VALUE";
	}
}

/// <summary>
///     Converts the IoT Hub connection status reason to a string.
/// </summary>
const char* GetReasonString(IOTHUB_CLIENT_CONNECTION_STATUS_REASON reason)
{
	static char* reasonString = "unknown reason";
	switch (reason) {
	case IOTHUB_CLIENT_CONNECTION_EXPIRED_SAS_TOKEN:
		reasonString = "IOTHUB_CLIENT_CONNECTION_EXPIRED_SAS_TOKEN";
		break;
	case IOTHUB_CLIENT_CONNECTION_DEVICE_DISABLED:
		reasonString = "IOTHUB_CLIENT_CONNECTION_DEVICE_DISABLED";
		break;
	case IOTHUB_CLIENT_CONNECTION_BAD_CREDENTIAL:
		reasonString = "IOTHUB_CLIENT_CONNECTION_BAD_CREDENTIAL";
		break;
	case IOTHUB_CLIENT_CONNECTION_RETRY_EXPIRED:
		reasonString = "IOTHUB_CLIENT_CONNECTION_RETRY_EXPIRED";
		break;
	case IOTHUB_CLIENT_CONNECTION_NO_NETWORK:
		reasonString = "IOTHUB_CLIENT_CONNECTION_NO_NETWORK";
		break;
	case IOTHUB_CLIENT_CONNECTION_COMMUNICATION_ERROR:
		reasonString = "IOTHUB_CLIENT_CONNECTION_COMMUNICATION_ERROR";
		break;
	case IOTHUB_CLIENT_CONNECTION_OK:
		reasonString = "IOTHUB_CLIENT_CONNECTION_OK";
		break;
	}
	return reasonString;
}


#pragma region Device Twins

void InitDeviceTwins(DeviceTwinPeripheral* deviceTwins[], size_t deviceTwinCount) {
	_deviceTwins = deviceTwins;
	_deviceTwinCount = deviceTwinCount;
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
		SetDesiredState(desiredProperties, _deviceTwins[i]);
	}

cleanup:
	// Release the allocated memory.
	if (root_value != NULL) {
		json_value_free(root_value);
	}
	free(payLoadString);
}

/// <summary>
///     Checks to see if the device twin twinProperty(name) is found in the json object. If yes, then act upon the request
/// </summary>
void SetDesiredState(JSON_Object* desiredProperties, DeviceTwinPeripheral* deviceTwinPeripheral) {
	JSON_Object* jsonObject = json_object_dotget_object(desiredProperties, deviceTwinPeripheral->twinProperty);
	if (jsonObject != NULL) {
		deviceTwinPeripheral->handler(jsonObject, deviceTwinPeripheral);
		TwinReportState(deviceTwinPeripheral->twinProperty, deviceTwinPeripheral->twinState);
	}
}

void TwinReportState(const char* propertyName, bool propertyValue)
{
	if (iothubClientHandle == NULL) {
		Log_Debug("ERROR: client not initialized\n");
	}
	else {
		static char reportedPropertiesString[30] = { 0 };
		int len = snprintf(reportedPropertiesString, 30, "{\"%s\":%s}", propertyName,
			(propertyValue == true ? "true" : "false"));
		if (len < 0)
			return;

		if (IoTHubDeviceClient_LL_SendReportedState(
			iothubClientHandle, (unsigned char*)reportedPropertiesString,
			strlen(reportedPropertiesString), ReportStatusCallback, 0) != IOTHUB_CLIENT_OK) {
			Log_Debug("ERROR: failed to set reported state for '%s'.\n", propertyName);
		}
		else {
			Log_Debug("INFO: Reported state for '%s' to value '%s'.\n", propertyName,
				(propertyValue == true ? "true" : "false"));
		}
	}
}


/// <summary>
///     Callback invoked when the Device Twin reported properties are accepted by IoT Hub.
/// </summary>
void ReportStatusCallback(int result, void* context)
{
	Log_Debug("INFO: Device Twin reported properties update result: HTTP status code %d\n", result);
}

#pragma endregion


#pragma region Direct Methods

void InitDirectMethods(DirectMethodPeripheral* directMethods[], size_t directMethodCount) {
	_directMethods = directMethods;
	_directMethodCount = directMethodCount;
}

int AzureDirectMethodHandler(const char* method_name, const unsigned char* payload, size_t payloadSize,
	unsigned char** responsePayload, size_t* responsePayloadSize, void* userContextCallback) {

	const char* onSuccess = "\"Successfully invoke device method\"";
	const char* notFound = "\"No method found\"";
	const char* methodFailed = "\"Method failed\"";

	const char* responseMessage = onSuccess;
	int result = 200;
	JSON_Value* root_value = NULL;
	JSON_Object* jsonObject = NULL;
	bool methodFound = false;

	// Prepare the payload for the response. This is a heap allocated null terminated string.
	// The Azure IoT Hub SDK is responsible of freeing it.
	*responsePayload = NULL;  // Response payload content.
	*responsePayloadSize = 0; // Response payload content size.

	char* payLoadString = (char*)malloc(payloadSize + 1);
	if (payLoadString == NULL) {
		responseMessage = "payload memory failed";
		result = 500;
		goto cleanup;
	}

	memcpy(payLoadString, payload, payloadSize);
	payLoadString[payloadSize] = 0; //null terminate string

	root_value = json_parse_string(payLoadString);
	if (root_value == NULL) {
		responseMessage = "Invalid JSON";
		result = 500;
		goto cleanup;
	}

	jsonObject = json_value_get_object(root_value);
	if (jsonObject == NULL) {
		responseMessage = "Invalid JSON";
		result = 500;
		goto cleanup;
	}

	for (int i = 0; i < _directMethodCount; i++) {
		if (strcmp(method_name, _directMethods[i]->methodName) == 0) {
			methodFound = true;
			if (jsonObject != NULL) {
				bool status = _directMethods[i]->handler(jsonObject, _directMethods[i]);
				if (!status) {
					responseMessage = methodFailed;
					result = 500;
				}
			}
			break;
		}
	}

	if (!methodFound)
	{
		responseMessage = notFound;
		result = 404;
	}

cleanup:

	// Prepare the payload for the response. This is a heap allocated null terminated string.
	// The Azure IoT Hub SDK is responsible of freeing it.
	*responsePayloadSize = strlen(responseMessage);
	*responsePayload = (unsigned char*)malloc(*responsePayloadSize);
	strncpy((char*)(*responsePayload), responseMessage, *responsePayloadSize);

	if (root_value != NULL) {
		json_value_free(root_value);
	}
	free(payLoadString);

	return result;
}

#pragma endregion
