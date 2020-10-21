#include "utilities.h"

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

char* lp_getCurrentUtc(char* buffer, size_t bufferSize) {
	time_t now = time(NULL);
	struct tm* t = gmtime(&now);
	strftime(buffer, bufferSize - 1, "%Y-%m-%dT%H:%M:%SZ", t);
	return buffer;
}