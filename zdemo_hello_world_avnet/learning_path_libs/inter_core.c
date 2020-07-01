#include "inter_core.h"

void SocketEventHandler(EventLoop* el, int fd, EventLoop_IoEvents events, void* context);
bool ProcessMsg(void);
void (*_interCoreCallback)(struct LP_INTER_CORE_BLOCK*);
int sockFd = -1;
static EventRegistration* socketEventReg = NULL;


bool lp_sendInterCoreMessage(LP_INTER_CORE_BLOCK* control_block, size_t len)
{

	if (sockFd == -1)
	{
		Log_Debug("Socket not initialized");
		return false;
	}

	int bytesSent = send(sockFd, (void*)control_block, len, 0);
	if (bytesSent == -1)
	{
		Log_Debug("ERROR: Unable to send message: %d (%s)\n", errno, strerror(errno));
		return false;
	}

	return true;
}


int lp_enableInterCoreCommunications(const char* rtAppComponentId, void (*interCoreCallback)(LP_INTER_CORE_BLOCK*))
{
	_interCoreCallback = interCoreCallback;
	// Open connection to real-time capable application.
	sockFd = Application_Connect(rtAppComponentId);
	if (sockFd == -1)
	{
		Log_Debug("ERROR: Unable to create socket: %d (%s)\n", errno, strerror(errno));
		return -1;
	}

	// Set timeout, to handle case where real-time capable application does not respond.
	static const struct timeval recvTimeout = { .tv_sec = 5, .tv_usec = 0 };
	int result = setsockopt(sockFd, SOL_SOCKET, SO_RCVTIMEO, &recvTimeout, sizeof(recvTimeout));
	if (result == -1)
	{
		Log_Debug("ERROR: Unable to set socket timeout: %d (%s)\n", errno, strerror(errno));
		return -1;
	}

	// Register handler for incoming messages from real-time capable application.
	socketEventReg = EventLoop_RegisterIo(lp_getTimerEventLoop(), sockFd, EventLoop_Input, SocketEventHandler,
		/* context */ NULL);
	if (socketEventReg == NULL)
	{
		Log_Debug("ERROR: Unable to register socket event: %d (%s)\n", errno, strerror(errno));
		return -1;
	}

	return 0;
}


/// <summary>
///     Handle socket event by reading incoming data from real-time capable application.
/// </summary>
void SocketEventHandler(EventLoop* el, int fd, EventLoop_IoEvents events, void* context)
{
	if (!ProcessMsg())
	{
		lp_terminate(ExitCode_InterCoreHandler);
	}
}


/// <summary>
///     Handle socket event by reading incoming data from real-time capable application.
/// </summary>
bool ProcessMsg()
{
	LP_INTER_CORE_BLOCK ic_control_block;

	int bytesReceived = recv(sockFd, (void*)&ic_control_block, sizeof(ic_control_block), 0);

	if (bytesReceived == -1)
	{
		lp_terminate(ExitCode_InterCoreReceiveFailed);
		return false;
	}

	_interCoreCallback(&ic_control_block);

	return true;
}