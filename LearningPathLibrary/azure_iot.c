#include "azure_iot.h"


static const char* getAzureSphereProvisioningResultString(AZURE_SPHERE_PROV_RETURN_VALUE provisioningResult);
static const char* GetReasonString(IOTHUB_CLIENT_CONNECTION_STATUS_REASON reason);
static void SendMessageCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT, void*);
static bool SetupAzureClient(void);
static void HubConnectionStatusCallback(IOTHUB_CLIENT_CONNECTION_STATUS, IOTHUB_CLIENT_CONNECTION_STATUS_REASON, void*);
static void AzureCloudToDeviceHandler(EventLoopTimer*);
bool sendMsg(const char* msg, LP_MESSAGE_PROPERTY** messageProperties, size_t messagePropertyCount);

static IOTHUB_DEVICE_CLIENT_LL_HANDLE iothubClientHandle = NULL;
static bool iothubAuthenticated = false;
static const int keepalivePeriodSeconds = 20;

static const char*	_idScope = NULL;
static const char* _connectionString = NULL;

// static LP_MESSAGE_PROPERTY** _messageProperties = NULL;
// static size_t _messagePropertyCount = 0;

static const int maxPeriodSeconds = 5; // defines the max back off period for DoWork with lost network

static LP_TIMER cloudToDeviceTimer = {
	.period = { 0, 0 },			// one-shot timer
	.name = "DoWork",
	.handler = &AzureCloudToDeviceHandler
};

void lp_azureToDeviceStart(void) {
	if (cloudToDeviceTimer.eventLoopTimer == NULL) {
		lp_timerStart(&cloudToDeviceTimer);
		lp_timerSetOneShot(&cloudToDeviceTimer, &(struct timespec){1, 0});
	}
}

void lp_azureToDeviceStop(void) {
	if (cloudToDeviceTimer.eventLoopTimer != NULL) {
		lp_timeStop(&cloudToDeviceTimer);
	}
}

void lp_azureConnectionStringSet(const char* connectionString){
	_connectionString = connectionString;
}
void lp_azureIdScopeSet(const char * idScope){
	_idScope = idScope;
}

/// <summary>
///     Callback confirming message delivered to IoT Hub.
/// </summary>
/// <param name="result">Message delivery status</param>
/// <param name="context">User specified context</param>
static void SendMessageCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* context) {
	#if LP_LOGGING_ENABLED
	Log_Debug("INFO: Message received by IoT Hub. Result is: %d\n", result);
	#endif
}

/// <summary>
///     Azure IoT Hub DoWork Handler with back off up to 5 seconds for network disconnect
/// </summary>
static void AzureCloudToDeviceHandler(EventLoopTimer* eventLoopTimer) {
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
		if (lp_azureConnect()) {
			period = 1;
		}
		else {
			if (period < maxPeriodSeconds) { period++; }
		}
	}
	lp_timerSetOneShot(&cloudToDeviceTimer, &(struct timespec){period, 0});
}

bool lp_azureMsgSendWithProperties(const char* msg, LP_MESSAGE_PROPERTY** messageProperties, size_t messagePropertyCount){
	return sendMsg(msg, messageProperties, messagePropertyCount);
}

bool lp_azureMsgSend(const char* msg){
	return sendMsg(msg, NULL, 0);
}

bool sendMsg(const char* msg, LP_MESSAGE_PROPERTY** messageProperties, size_t messagePropertyCount){

	if (strlen(msg) < 1) {
		return true;
	}

	if (!lp_azureConnect()) {
		return false;
	}

	IOTHUB_MESSAGE_HANDLE messageHandle = IoTHubMessage_CreateFromString(msg);

	if (messageHandle == 0) {
		Log_Debug("WARNING: unable to create a new IoTHubMessage\n");
		return false;
	}

	if (messageProperties != NULL && messagePropertyCount > 0)
	{
		for (size_t i = 0; i < messagePropertyCount; i++)
		{
			if (messageProperties[i]->key != NULL && messageProperties[i]->value != NULL)
			{
				IoTHubMessage_SetProperty(messageHandle, messageProperties[i]->key, messageProperties[i]->value);
			}
		}
	}

	if (IoTHubDeviceClient_LL_SendEventAsync(iothubClientHandle, messageHandle, SendMessageCallback,
		/*&callback_param*/ 0) != IOTHUB_CLIENT_OK) {
		Log_Debug("WARNING: failed to hand over the message to IoTHubClient\n");
		return false;
	}
	else {
		#if LP_LOGGING_ENABLED
		Log_Debug("INFO: IoTHubClient accepted the message for delivery\n");
		#endif
	}

	IoTHubMessage_Destroy(messageHandle);

	IoTHubDeviceClient_LL_DoWork(iothubClientHandle);

	return true;
}




IOTHUB_DEVICE_CLIENT_LL_HANDLE lp_azureClientHandleGet(void) {
	return iothubClientHandle;
}


/// <summary>
///     Check if network connected or already connected, else sets up connection to Azure IoT
/// </summary>
bool lp_azureConnect(void) {
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
static bool SetupAzureClient() {
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
		AZURE_SPHERE_PROV_RETURN_VALUE provResult = IoTHubDeviceClient_LL_CreateWithAzureSphereDeviceAuthProvisioning(_idScope, 10000, &iothubClientHandle);
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
	IoTHubDeviceClient_LL_SetDeviceMethodCallback(iothubClientHandle, lp_directMethodHandler, NULL);
	IoTHubDeviceClient_LL_SetConnectionStatusCallback(iothubClientHandle, HubConnectionStatusCallback, NULL);

	IoTHubDeviceClient_LL_DoWork(iothubClientHandle);

	return true;
}

/// <summary>
///     Sets the IoT Hub authentication state for the app
///     The SAS Token expires which will set the authentication state
/// </summary>
static void HubConnectionStatusCallback(IOTHUB_CLIENT_CONNECTION_STATUS result, IOTHUB_CLIENT_CONNECTION_STATUS_REASON reason, void* userContextCallback) {
	iothubAuthenticated = (result == IOTHUB_CLIENT_CONNECTION_AUTHENTICATED);
	Log_Debug("IoT Hub Connection Status: %s\n", GetReasonString(reason));
}

/// <summary>
///     Converts AZURE_SPHERE_PROV_RETURN_VALUE to a string.
/// </summary>
static const char* getAzureSphereProvisioningResultString(
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
static const char* GetReasonString(IOTHUB_CLIENT_CONNECTION_STATUS_REASON reason) {
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
	case IOTHUB_CLIENT_CONNECTION_NO_PING_RESPONSE:
		reasonString = "IOTHUB_CLIENT_CONNECTION_NO_PING_RESPONSE";
		break;
	}
	return reasonString;
}
