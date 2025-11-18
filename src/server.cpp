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
	
	return mySocket;
}
 