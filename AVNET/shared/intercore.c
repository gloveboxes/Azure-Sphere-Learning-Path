#include "intercore.h"

void SocketEventHandler(EventLoop* el, int fd, EventLoop_IoEvents events, void* context);
bool ProcessMsg(void);
void (*_interCoreCallback)(char*);
int sockFd = -1;

static EventRegistration* socketEventReg = NULL;


bool SendInterCoreMessage(const char* msg) {

	if (sockFd == -1) {
		Log_Debug("Socket not initialized");
		return false;
	}

	int bytesSent = send(sockFd, msg, strlen(msg), 0);
	if (bytesSent == -1) {
		Log_Debug("ERROR: Unable to send message: %d (%s)\n", errno, strerror(errno));
		return false;
	}

	return true;
}

int EnableInterCoreCommunications(const char* rtAppComponentId, void (*interCoreCallback)(char*)) {
	_interCoreCallback = interCoreCallback;
	// Open connection to real-time capable application.
	sockFd = Application_Socket(rtAppComponentId);
	if (sockFd == -1) {
		Log_Debug("ERROR: Unable to create socket: %d (%s)\n", errno, strerror(errno));
		return -1;
	}

	// Set timeout, to handle case where real-time capable application does not respond.
	static const struct timeval recvTimeout = { .tv_sec = 5, .tv_usec = 0 };
	int result = setsockopt(sockFd, SOL_SOCKET, SO_RCVTIMEO, &recvTimeout, sizeof(recvTimeout));
	if (result == -1) {
		Log_Debug("ERROR: Unable to set socket timeout: %d (%s)\n", errno, strerror(errno));
		return -1;
	}

	// Register handler for incoming messages from real-time capable application.
	socketEventReg = EventLoop_RegisterIo(GetTimerEventLoop(), sockFd, EventLoop_Input, SocketEventHandler,
		/* context */ NULL);
	if (socketEventReg == NULL) {
		Log_Debug("ERROR: Unable to register socket event: %d (%s)\n", errno, strerror(errno));
		return -1;
	}

	return 0;
}


/// <summary>
///     Handle socket event by reading incoming data from real-time capable application.
/// </summary>
void SocketEventHandler(EventLoop* el, int fd, EventLoop_IoEvents events, void* context) {
	if (!ProcessMsg()) {
		Terminate();
	}
}

/// <summary>
///     Handle socket event by reading incoming data from real-time capable application.
/// </summary>
bool ProcessMsg() {
	char rxBuf[32];
	char msg[32];
	memset(msg, 0, sizeof msg);

	int bytesReceived = recv(sockFd, rxBuf, sizeof(rxBuf), 0);

	if (bytesReceived == -1) {
		//Log_Debug("ERROR: Unable to receive message: %d (%s)\n", errno, strerror(errno));
		return false;
	}

	//Log_Debug("Received %d bytes: ", bytesReceived);
	for (int i = 0; i < bytesReceived; ++i) {
		msg[i] = isprint(rxBuf[i]) ? rxBuf[i] : '.';
	}
	Log_Debug("\n%s\n\n",msg);

	_interCoreCallback(msg);

	return true;
}
