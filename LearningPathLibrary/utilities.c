#include "utilities.h"

bool lp_isNetworkReady(void) {
	bool isNetworkReady = false;
	if (Networking_IsNetworkingReady(&isNetworkReady) != 0) {
		Log_Debug("ERROR: Networking_IsNetworkingReady: %d (%s)\n", errno, strerror(errno));
	}
	else {
		if (!isNetworkReady) {
			Log_Debug("\nNetwork not ready.\nFrom azure sphere command prompt, run azsphere device wifi show-status\n\n");
		}
	}

	return isNetworkReady;
}

bool lp_isDeviceAuthReady(void) {
	// Verifies authentication is ready on device
	bool currentAppDeviceAuthReady = false;
	if (Application_IsDeviceAuthReady(&currentAppDeviceAuthReady) != 0) {
		Log_Debug("ERROR: Application_IsDeviceAuthReady: %d (%s)\n", errno, strerror(errno));
	}
	else {
		if (!currentAppDeviceAuthReady) {
			Log_Debug("ERROR: Current Application not Device Auth Ready\n");
		}
	}

	return currentAppDeviceAuthReady;
}

char* lp_getCurrentUtc(char* buffer, size_t bufferSize) {
	time_t now = time(NULL);
	struct tm* t = gmtime(&now);
	strftime(buffer, bufferSize - 1, "%Y-%m-%dT%H:%M:%SZ", t);
	return buffer;
}