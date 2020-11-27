#include "azure_iot.h"

bool sendMsg(const char* msg, LP_MESSAGE_PROPERTY** messageProperties, size_t messagePropertyCount);
static bool ProvisionWithDpsPnP(void);
static bool SetupAzureClient(void);
static const char* GetReasonString(IOTHUB_CLIENT_CONNECTION_STATUS_REASON reason);
static const char* getAzureSphereProvisioningResultString(AZURE_SPHERE_PROV_RETURN_VALUE provisioningResult);
static void AzureCloudToDeviceHandler(EventLoopTimer*);
static void HubConnectionStatusCallback(IOTHUB_CLIENT_CONNECTION_STATUS, IOTHUB_CLIENT_CONNECTION_STATUS_REASON, void*);
static void SendMessageCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT, void*);

static IOTHUB_DEVICE_CLIENT_LL_HANDLE iothubClientHandle = NULL;
static bool iothubAuthenticated = false;
// static const int keepalivePeriodSeconds = 20;

static const char* _idScope = NULL;
static const char* _connectionString = NULL;
static const char* _deviceTwinModelId = NULL;

#define dpsUrl "global.azure-devices-provisioning.net"

static bool dpsRegisterCompleted;
static PROV_DEVICE_RESULT dpsRegisterStatus = PROV_DEVICE_REG_HUB_NOT_SPECIFIED;

// static LP_MESSAGE_PROPERTY** _messageProperties = NULL;
// static size_t _messagePropertyCount = 0;

static const int maxPeriodSeconds = 5; // defines the max back off period for DoWork with lost network
// static const int deviceIdForDaaCertUsage = 1; // A constant used to direct the IoT SDK to use
											  // the DAA cert under the hood.

static LP_TIMER cloudToDeviceTimer = {
	.period = {0, 0}, // one-shot timer
	.name = "DoWork",
	.handler = &AzureCloudToDeviceHandler };

void lp_azureToDeviceStart(void)
{
	if (cloudToDeviceTimer.eventLoopTimer == NULL)
	{
		lp_timerStart(&cloudToDeviceTimer);
		lp_timerOneShotSet(&cloudToDeviceTimer, &(struct timespec){1, 0});
	}
}

void lp_azureToDeviceStop(void)
{
	if (cloudToDeviceTimer.eventLoopTimer != NULL)
	{
		lp_timerStop(&cloudToDeviceTimer);
	}
}

void lp_azureConnectionStringSet(const char* connectionString)
{
	_connectionString = connectionString;
}

void lp_azureInitialize(const char* idScope, const char* deviceTwinModelId)
{
	_idScope = idScope;
	_deviceTwinModelId = deviceTwinModelId;
}

/// <summary>
///     Callback confirming message delivered to IoT Hub.
/// </summary>
/// <param name="result">Message delivery status</param>
/// <param name="context">User specified context</param>
static void SendMessageCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* context)
{
#if LP_LOGGING_ENABLED
	Log_Debug("INFO: Message received by IoT Hub. Result is: %d\n", result);
#endif
}

/// <summary>
///     Azure IoT Hub DoWork Handler with back off up to 5 seconds for network disconnect
/// </summary>
static void AzureCloudToDeviceHandler(EventLoopTimer* eventLoopTimer)
{
	static int period = 1; //  initialize period to 1 second

	if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0)
	{
		lp_terminate(ExitCode_AzureCloudToDeviceHandler);
		return;
	}

	if (iothubAuthenticated && iothubClientHandle != NULL)
	{
		IoTHubDeviceClient_LL_DoWork(iothubClientHandle);
		period = 1;
	}
	else
	{
		if (lp_azureConnect())
		{
			period = 1;
		}
		else
		{
			if (period < maxPeriodSeconds)
			{
				period++;
			}
		}
	}
	lp_timerOneShotSet(&cloudToDeviceTimer, &(struct timespec){period, 0});
}

bool lp_azureMsgSendWithProperties(const char* msg, LP_MESSAGE_PROPERTY** messageProperties, size_t messagePropertyCount)
{
	return sendMsg(msg, messageProperties, messagePropertyCount);
}

bool lp_azureMsgSend(const char* msg)
{
	return sendMsg(msg, NULL, 0);
}

bool sendMsg(const char* msg, LP_MESSAGE_PROPERTY** messageProperties, size_t messagePropertyCount)
{

	if (strlen(msg) < 1)
	{
		return true;
	}

	if (!lp_azureConnect())
	{
		return false;
	}

	IOTHUB_MESSAGE_HANDLE messageHandle = IoTHubMessage_CreateFromString(msg);

	if (messageHandle == 0)
	{
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
		/*&callback_param*/ 0) != IOTHUB_CLIENT_OK)
	{
		Log_Debug("WARNING: failed to hand over the message to IoTHubClient\n");
		return false;
	}
	else
	{
#if LP_LOGGING_ENABLED
		Log_Debug("INFO: IoTHubClient accepted the message for delivery\n");
#endif
	}

	IoTHubMessage_Destroy(messageHandle);

	IoTHubDeviceClient_LL_DoWork(iothubClientHandle);

	return true;
}

IOTHUB_DEVICE_CLIENT_LL_HANDLE lp_azureClientHandleGet(void)
{
	return iothubClientHandle;
}

/// <summary>
///     Check if network connected or already connected, else sets up connection to Azure IoT
/// </summary>
bool lp_azureConnect(void)
{
	if (!lp_isNetworkReady())
	{
		return false;
	}
	else
	{
		if (iothubAuthenticated)
		{
			return true;
		}
		else
		{
			return SetupAzureClient();
		}
	}
}

/// <summary>
///     Sets up the Azure IoT Hub connection (creates the iothubClientHandle)
///     When the SAS Token for a device expires the connection needs to be recreated
///     which is why this is not simply a one time call.
/// </summary>
static bool SetupAzureClient()
{
	if (iothubClientHandle != NULL)
	{
		IoTHubDeviceClient_LL_Destroy(iothubClientHandle);
	}

	// For lab purposes only where the device tenant and associated x500 certificate may not be available
	// DO NOT use connection strings in production
	if (_connectionString != NULL && strlen(_connectionString) != 0)
	{
		IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol = MQTT_Protocol;
		iothubClientHandle = IoTHubDeviceClient_LL_CreateFromConnectionString(_connectionString, protocol);
		if (iothubClientHandle == NULL)
		{
			Log_Debug("Failure to create IoT Hub Client from connection string");
			return false;
		}
	}
	else
	{
		if (!ProvisionWithDpsPnP()) {
			return false;
		}
		else
		{
			AZURE_SPHERE_PROV_RETURN_VALUE provResult = IoTHubDeviceClient_LL_CreateWithAzureSphereDeviceAuthProvisioning(_idScope, 10000, &iothubClientHandle);
			Log_Debug("IoTHubDeviceClient_LL_CreateWithAzureSphereDeviceAuthProvisioning returned '%s'.\n", getAzureSphereProvisioningResultString(provResult));
			if (provResult.result != AZURE_SPHERE_PROV_RESULT_OK)
			{
				Log_Debug("ERROR: failure to create IoTHub Handle.\n");
				return false;
			}
		}
	}

	if (_deviceTwinModelId != NULL && strlen(_deviceTwinModelId) > 0)
	{
		if (IoTHubDeviceClient_LL_SetOption(iothubClientHandle, OPTION_MODEL_ID, _deviceTwinModelId) != IOTHUB_CLIENT_OK)
		{
			Log_Debug("ERROR: failure setting option \"%s\"\n", OPTION_MODEL_ID);
			return false;
		}
	}

	iothubAuthenticated = true;

	IoTHubDeviceClient_LL_SetDeviceTwinCallback(iothubClientHandle, lp_twinCallback, NULL);
	IoTHubDeviceClient_LL_SetDeviceMethodCallback(iothubClientHandle, lp_directMethodHandler, NULL);
	IoTHubDeviceClient_LL_SetConnectionStatusCallback(iothubClientHandle, HubConnectionStatusCallback, NULL);

	IoTHubDeviceClient_LL_DoWork(iothubClientHandle);

	return true;
}

/// <summary>
///     DPS provisioning callback with status
/// </summary>
static void RegisterDeviceCallback(PROV_DEVICE_RESULT registerResult, const char* iothubUri, const char* deviceId, void* userContext)
{
	dpsRegisterCompleted = registerResult == PROV_DEVICE_RESULT_OK;
	dpsRegisterStatus = registerResult;
}

/// <summary>
///     Provision with DPS and assign IoT Plug and Play Model ID
/// </summary>
static bool ProvisionWithDpsPnP(void)
{
	PROV_DEVICE_LL_HANDLE prov_handle;
	PROV_DEVICE_RESULT prov_result;
	bool result = false;
	char* dtdlBuffer = NULL;
	int deviceIdForDaaCertUsage = 0;  // set DaaCertUsage to false

	dpsRegisterCompleted = false;
	dpsRegisterStatus = PROV_DEVICE_REG_HUB_NOT_SPECIFIED;

	if (!lp_isNetworkReady() || !lp_isDeviceAuthReady()) {
		return false;
	}

	if (_deviceTwinModelId != NULL && strlen(_deviceTwinModelId) > 0)
	{
		size_t modelIdLen = 20; // allow for JSON format "{\"modelId\":\"%s\"}", 14 char, plus null and a couple of extra :)
		modelIdLen += strlen(_deviceTwinModelId); // allow for twin property name in JSON response

		dtdlBuffer = (char*)malloc(modelIdLen);
		if (dtdlBuffer == NULL) {
			Log_Debug("ERROR: PnP Model ID malloc failed.\n");
			goto cleanup;
		}

		int len = snprintf(dtdlBuffer, modelIdLen, "{\"modelId\":\"%s\"}", _deviceTwinModelId);
		if (len < 0 || len >= modelIdLen) {
			Log_Debug("ERROR: Cannot write Model ID to buffer.\n");
			goto cleanup;
		}
	}

	// Initiate security with X509 Certificate
	if (prov_dev_security_init(SECURE_DEVICE_TYPE_X509) != 0) {
		Log_Debug("ERROR: Failed to initiate X509 Certificate security\n");
		goto cleanup;
	}

	// Create Provisioning Client for communication with DPS using MQTT protocol
	if ((prov_handle = Prov_Device_LL_Create(dpsUrl, _idScope, Prov_Device_MQTT_Protocol)) == NULL) {
		Log_Debug("ERROR: Failed to create Provisioning Client\n");
		goto cleanup;
	}

	// Sets Device ID on Provisioning Client
	if ((prov_result = Prov_Device_LL_SetOption(prov_handle, "SetDeviceId", &deviceIdForDaaCertUsage)) != PROV_DEVICE_RESULT_OK) {
		Log_Debug("ERROR: Failed to set Device ID in Provisioning Client, error=%d\n", prov_result);
		goto cleanup;
	}

	// Sets Model ID provisioning data
	if (dtdlBuffer != NULL) {
		if ((prov_result = Prov_Device_LL_Set_Provisioning_Payload(prov_handle, dtdlBuffer)) != PROV_DEVICE_RESULT_OK) {
			Log_Debug("Error: Failed to set Model ID in Provisioning Client, error=%d\n", prov_result);
			goto cleanup;
		}
	}

	// Sets the callback function for device registration
	if ((prov_result = Prov_Device_LL_Register_Device(prov_handle, RegisterDeviceCallback, NULL, NULL, NULL)) != PROV_DEVICE_RESULT_OK) {
		Log_Debug("ERROR: Failed to set callback function for device registration, error=%d\n", prov_result);
		goto cleanup;
	}

	// Begin provisioning device with DPS
	// Initiates timer to prevent timing out
	static const long timeoutMs = 60000; // aloow up to 60 seconds before timeout
	static const long workDelayMs = 25;
	const struct timespec sleepTime = { .tv_sec = 0, .tv_nsec = workDelayMs * 1000 * 1000 };
	long time_elapsed = 0;

	while (!dpsRegisterCompleted && time_elapsed < timeoutMs) {
		Prov_Device_LL_DoWork(prov_handle);
		nanosleep(&sleepTime, NULL);
		time_elapsed += workDelayMs;
	}

	if (!dpsRegisterCompleted || dpsRegisterStatus != PROV_DEVICE_RESULT_OK) {
		Log_Debug("ERROR: Failed to register device with provisioning service\n");
		goto cleanup;
	}

	result = true;

cleanup:
	if (dtdlBuffer != NULL) {
		free(dtdlBuffer);
		dtdlBuffer = NULL;
	}

	if (prov_handle != NULL) {
		Prov_Device_LL_Destroy(prov_handle);
	}

	prov_dev_security_deinit();
	return result;
}

/// <summary>
///     Sets the IoT Hub authentication state for the app
///     The SAS Token expires which will set the authentication state
/// </summary>
static void HubConnectionStatusCallback(IOTHUB_CLIENT_CONNECTION_STATUS result, IOTHUB_CLIENT_CONNECTION_STATUS_REASON reason, void* userContextCallback)
{
	iothubAuthenticated = (result == IOTHUB_CLIENT_CONNECTION_AUTHENTICATED);
	Log_Debug("IoT Hub Connection Status: %s\n", GetReasonString(reason));
}

/// <summary>
///     Converts AZURE_SPHERE_PROV_RETURN_VALUE to a string.
/// </summary>
static const char* getAzureSphereProvisioningResultString(
	AZURE_SPHERE_PROV_RETURN_VALUE provisioningResult)
{
	switch (provisioningResult.result)
	{
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
static const char* GetReasonString(IOTHUB_CLIENT_CONNECTION_STATUS_REASON reason)
{
	static char* reasonString = "unknown reason";
	switch (reason)
	{
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
