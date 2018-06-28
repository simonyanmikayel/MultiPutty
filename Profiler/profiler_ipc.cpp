#include "pch.h"
#include "profiler_ipc.h"

#ifdef _PROFILER

static SOCKET ConnectSocket = INVALID_SOCKET;

static void CloseSocket()
{
	if (ConnectSocket != INVALID_SOCKET)
	{
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
	}
}

bool profiler::WsaInit()
{
	WSADATA wsaData;

	// Initialize Winsock
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		Log("WSAStartup failed with error: %d\n", iResult);
		return false;
	}

	return true;
}

bool profiler::ConnectToServer(char* addr, char* port)
{
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	int iResult = getaddrinfo(addr, port, &hints, &result);
	if (iResult != 0) {
		Log("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return false;
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			Log("socket failed with error: %ld\n", WSAGetLastError());
			break;
		}

		// Connect to server.
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			CloseSocket();
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		Log("Unable to connect to server!\n");
		WSACleanup();
		return false;
	}

	return true;
}

bool profiler::Send(void* sendbuf, int cb)
{
	if (ConnectSocket == INVALID_SOCKET)
		return false;

	int iResult = ::send(ConnectSocket, (char*)sendbuf, cb, 0);
	if (iResult == SOCKET_ERROR) {
		CloseSocket();
		Log("send failed with error: %d\n", WSAGetLastError());
		WSACleanup();
		return false;
	}

	return true;
}

bool profiler::Recv(void* recvbuf, int cb)
{
	int iResult = 0;
	// Receive until the peer closes the connection
	while ( cb > 0 ) {

		iResult = recv(ConnectSocket, (char*)recvbuf, cb, 0);
		if (iResult > 0) {
			cb -= iResult;
			recvbuf = ((char*)recvbuf) + iResult;
		}
		else if (iResult == 0) {
			Log("Connection closed\n");
			break;
		}
		else {
			Log("recv failed with error: %d\n", WSAGetLastError());
			break;
		}
	}

	if (cb != 0) {
		CloseSocket();
		WSACleanup();
		return false;
	}
	else {
		return true;
	}
}

#endif //_PROFILER