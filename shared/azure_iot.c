#include "azure_iot.h"

const char* getAzureSphereProvisioningResultString(AZURE_SPHERE_PROV_RETURN_VALUE provisioningResult);
const char* GetReasonString(IOTHUB_CLIENT_CONNECTION_STATUS_REASON reason);
void SendMessageCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT, void*);
bool SetupAzureClient(void);
void HubConnectionStatusCallback(IOTHUB_CLIENT_CONNECTION_STATUS, IOTHUB_CLIENT_CONNECTION_STATUS_REASON, void*);


IOTHUB_DEVICE_CLIENT_LL_HANDLE iothubClientHandle = NULL;
bool iothubAuthenticated = false;
const int keepalivePeriodSeconds = 20;
const char* _connectionString = NULL;

static Timer cloudToDeviceTimer = {
	.eventData = {.eventHandler = &AzureCloudToDeviceHandler },
	.period = { 1, 0 },			// 500 milliseconds
	.name = "DoWork",
	.fd = -1
};


void EnableCloudToDevice(void) {
	if (cloudToDeviceTimer.fd == -1) {
		StartTimer(&cloudToDeviceTimer);
	}
}


void DisableCloudToDevice(void) {
	if (cloudToDeviceTimer.fd != -1) {
		CloseFdAndPrintError(cloudToDeviceTimer.fd, cloudToDeviceTimer.name);
	}
}

void SetConnectionString(const char* connectionString) {
	_connectionString = connectionString;
}

/// <summary>
///     Callback confirming message delivered to IoT Hub.
/// </summary>
/// <param name="result">Message delivery status</param>
/// <param name="context">User specified context</param>
void SendMessageCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* context)
{
	Log_Debug("INFO: Message received by IoT Hub. Result is: %d\n", result);
}

void AzureCloudToDeviceHandler(EventData* eventData) {
	if (ConsumeTimerFdEvent(cloudToDeviceTimer.fd) != 0) {
		Terminate();
		return;
	}

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

	// For lab purposes only where the device tenant and associated x500 certificate may not be available
	// DO NOT use connection strings in production
	if (_connectionString != NULL && strlen(_connectionString) != 0) {
		IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol = MQTT_Protocol;
		iothubClientHandle = IoTHubDeviceClient_LL_CreateFromConnectionString(_connectionString, protocol);
		if (iothubClientHandle == NULL)
		{
			Log_Debug("Failure to create IoT Hub Client from connection string");
		}
	}
	else {
		AZURE_SPHERE_PROV_RETURN_VALUE provResult = IoTHubDeviceClient_LL_CreateWithAzureSphereDeviceAuthProvisioning(scopeId, 10000, &iothubClientHandle);
		Log_Debug("IoTHubDeviceClient_LL_CreateWithAzureSphereDeviceAuthProvisioning returned '%s'.\n", getAzureSphereProvisioningResultString(provResult));
		if (provResult.result != AZURE_SPHERE_PROV_RESULT_OK) {
			Log_Debug("ERROR: failure to create IoTHub Handle.\n");
			return false;
		}
	}

	iothubAuthenticated = true;

	if (IoTHubDeviceClient_LL_SetOption(iothubClientHandle, OPTION_KEEP_ALIVE, &keepalivePeriodSeconds) != IOTHUB_CLIENT_OK) {
		Log_Debug("ERROR: failure setting option \"%s\"\n", OPTION_KEEP_ALIVE);
		return false;
	}

	IoTHubDeviceClient_LL_DoWork(iothubClientHandle);

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

//void DeviceTwinUpdateReportedState(char* reportedPropertiesString) {
//	if (IoTHubDeviceClient_LL_SendReportedState(
//		iothubClientHandle, (unsigned char*)reportedPropertiesString,
//		strlen(reportedPropertiesString), DeviceTwinsReportStatusCallback, 0) != IOTHUB_CLIENT_OK) {
//		Log_Debug("ERROR: failed to set reported state for '%s'.\n", reportedPropertiesString);
//	}
//	else {
//		Log_Debug("INFO: Reported state updated '%s'.\n", reportedPropertiesString);
//	}
//}

