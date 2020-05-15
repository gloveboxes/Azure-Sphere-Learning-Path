#include "azure_iot.h"
#include <time.h>

const char* getAzureSphereProvisioningResultString(AZURE_SPHERE_PROV_RETURN_VALUE provisioningResult);
const char* GetReasonString(IOTHUB_CLIENT_CONNECTION_STATUS_REASON reason);
void SendMessageCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT, void*);
bool SetupAzureClient(void);
void HubConnectionStatusCallback(IOTHUB_CLIENT_CONNECTION_STATUS, IOTHUB_CLIENT_CONNECTION_STATUS_REASON, void*);
void AzureCloudToDeviceHandler(EventLoopTimer*);

IOTHUB_DEVICE_CLIENT_LL_HANDLE iothubClientHandle = NULL;
bool iothubAuthenticated = false;
const int keepalivePeriodSeconds = 20;
const char* _connectionString = NULL;

const int maxPeriodSeconds = 5; // defines the max back off period for DoWork with lost network

static LP_TIMER cloudToDeviceTimer = {
	.period = { 0, 0 },			// one-shot timer
	.name = "DoWork",
	.handler = &AzureCloudToDeviceHandler
};

void lp_startCloudToDevice(void) {
	if (cloudToDeviceTimer.eventLoopTimer == NULL) {
		lp_startTimer(&cloudToDeviceTimer);
		lp_setOneShotTimer(&cloudToDeviceTimer, &(struct timespec){1, 0});
	}
}

void lp_stopCloudToDevice(void) {
	if (cloudToDeviceTimer.eventLoopTimer != NULL) {
		lp_stopTimer(&cloudToDeviceTimer);
	}
}

void lp_setConnectionString(const char* connectionString) {
	_connectionString = connectionString;
}

/// <summary>
///     Callback confirming message delivered to IoT Hub.
/// </summary>
/// <param name="result">Message delivery status</param>
/// <param name="context">User specified context</param>
void SendMessageCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* context) {
	Log_Debug("INFO: Message received by IoT Hub. Result is: %d\n", result);
}

/// <summary>
///     Azure IoT Hub DoWork Handler with back off up to 5 seconds for network disconnect
/// </summary>
void AzureCloudToDeviceHandler(EventLoopTimer* eventLoopTimer) {
	static int period = 1; //  initialize period to 1 second

	if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0) {
		lp_terminate(ExitCode_AzureCloudToDeviceHandler);
		return;
	}

	if (iothubAuthenticated && iothubClientHandle != NULL) {
		IoTHubDeviceClient_LL_DoWork(iothubClientHandle);
		period = 1;
	}
	else {
		if (lp_connectToAzureIot()) {
			period = 1;
		}
		else {
			if (period < maxPeriodSeconds) { period++; }
		}
	}
	lp_setOneShotTimer(&cloudToDeviceTimer, &(struct timespec){period, 0});
}

bool lp_sendMsg(const char* msg) {
	if (strlen(msg) < 1) {
		return true;
	}

	if (!lp_connectToAzureIot()) {
		return false;
	}

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

	IoTHubDeviceClient_LL_DoWork(iothubClientHandle);

	return true;
}

bool lp_isNetworkReady(void) {
	bool isNetworkReady = false;
	if (Networking_IsNetworkingReady(&isNetworkReady) != -1) {
		if (isNetworkReady) {
			return true;
		}
		else {
			Log_Debug("\nNetwork not ready.\nFrom azure sphere command prompt, run azsphere device wifi show-status\n\n");
			return false;
		}
	}
	else {
		Log_Debug("Failed to get Network state\n");
		return false;
	}
}


IOTHUB_DEVICE_CLIENT_LL_HANDLE lp_getAzureIotClientHandle(void) {
	return iothubClientHandle;
}


/// <summary>
///     Check if network connected or already connected, else sets up connection to Azure IoT
/// </summary>
bool lp_connectToAzureIot(void) {
	if (!lp_isNetworkReady()) {
		return false;
	}
	else {
		if (iothubAuthenticated) {
			return true;
		}
		else {
			return SetupAzureClient();
		}
	}
}


/// <summary>
///     Sets up the Azure IoT Hub connection (creates the iothubClientHandle)
///     When the SAS Token for a device expires the connection needs to be recreated
///     which is why this is not simply a one time call.
/// </summary>
bool SetupAzureClient() {
	if (iothubClientHandle != NULL) {
		IoTHubDeviceClient_LL_Destroy(iothubClientHandle);
	}

	// For lab purposes only where the device tenant and associated x500 certificate may not be available
	// DO NOT use connection strings in production
	if (_connectionString != NULL && strlen(_connectionString) != 0) {
		IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol = MQTT_Protocol;
		iothubClientHandle = IoTHubDeviceClient_LL_CreateFromConnectionString(_connectionString, protocol);
		if (iothubClientHandle == NULL) {
			Log_Debug("Failure to create IoT Hub Client from connection string");
			return false;
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
	   
	if (IoTHubDeviceClient_LL_SetOption(iothubClientHandle, OPTION_KEEP_ALIVE, &keepalivePeriodSeconds) != IOTHUB_CLIENT_OK) {
		Log_Debug("ERROR: failure setting option \"%s\"\n", OPTION_KEEP_ALIVE);
		return false;
	}

	iothubAuthenticated = true;

	IoTHubDeviceClient_LL_SetDeviceTwinCallback(iothubClientHandle, lp_twinCallback, NULL);
	IoTHubDeviceClient_LL_SetDeviceMethodCallback(iothubClientHandle, lp_azureDirectMethodHandler, NULL);
	IoTHubDeviceClient_LL_SetConnectionStatusCallback(iothubClientHandle, HubConnectionStatusCallback, NULL);

	IoTHubDeviceClient_LL_DoWork(iothubClientHandle);

	return true;
}

/// <summary>
///     Sets the IoT Hub authentication state for the app
///     The SAS Token expires which will set the authentication state
/// </summary>
void HubConnectionStatusCallback(IOTHUB_CLIENT_CONNECTION_STATUS result, IOTHUB_CLIENT_CONNECTION_STATUS_REASON reason, void* userContextCallback) {
	iothubAuthenticated = (result == IOTHUB_CLIENT_CONNECTION_AUTHENTICATED);
	Log_Debug("IoT Hub Connection Status: %s\n", GetReasonString(reason));
}

/// <summary>
///     Converts AZURE_SPHERE_PROV_RETURN_VALUE to a string.
/// </summary>
const char* getAzureSphereProvisioningResultString(
	AZURE_SPHERE_PROV_RETURN_VALUE provisioningResult) {
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
const char* GetReasonString(IOTHUB_CLIENT_CONNECTION_STATUS_REASON reason) {
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
