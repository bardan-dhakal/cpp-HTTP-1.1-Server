#include <iostream>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

struct SocketServer {
	SOCKET listening_socket;
	int port;
};

SocketServer createServerSocket( int Port); /*port is supposed to be obtained from the cmd line*/
void bindSocket(const SocketServer& mySocket); /* bind created socket to desired port number*/
void listenSocket(const SocketServer& mySocket); /*Listen on the created socket*/