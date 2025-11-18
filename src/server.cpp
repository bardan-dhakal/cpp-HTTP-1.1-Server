#include "server.h"

SocketServer createServerSocket(int Port) {

	WSADATA wsaData;
	SocketServer mySocket = {INVALID_SOCKET, 0 };
	
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (iResult != 0)
	{
		std::cout << "WSAStartup failed with error: " << iResult << std::endl;
		return mySocket;
	}

	std::cout << "Winsock intialized" << std::endl;

	SOCKET listening_socket;
	listening_socket = socket(AF_INET, SOCK_STREAM, 0);

	if (listening_socket == INVALID_SOCKET)
	{
		int lasterror = WSAGetLastError();
		std::cout << "Socket not initialized with error " << lasterror << std::endl;
		
		return mySocket;
	}

	mySocket.listening_socket = listening_socket;
	mySocket.port = Port;

	int opt_value = 1;
	if (setsockopt(listening_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt_value, sizeof(opt_value)) == -1)
	{
		int lasterror = WSAGetLastError();
		std::cout << "Socket option not initialized " << lasterror << std::endl;
	}

	std::cout << "SO_REUSEADDR set successfully" << std::endl;

	return mySocket;
}

void bindSocket(const SocketServer& mySocket)
{
	sockaddr_in addr_info;

	addr_info.sin_family = AF_INET;
	addr_info.sin_port = htons(mySocket.port);
	addr_info.sin_addr.s_addr = INADDR_ANY;

	if (bind(mySocket.listening_socket, (const sockaddr*)&addr_info, sizeof(addr_info)) == SOCKET_ERROR)
	{
		int lasterror = WSAGetLastError();
		std::cout << "Socket binding could not be complete " << lasterror << std::endl;
		return; 
	}

	return;
}
 