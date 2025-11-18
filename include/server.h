#ifndef SERVER_H
#define SERVER_H


#include <iostream>
#include <string>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

struct SocketServer {
	SOCKET listening_socket;
	int port;
};

SocketServer createServerSocket( int Port); /*port is supposed to be obtained from the cmd line*/
void bindSocket(const SocketServer& mySocket); /* bind created socket to desired port number*/
void listenSocket(const SocketServer& mySocket); /*Listen on the created socket*/
SOCKET acceptConnection(const SocketServer& mySocket); /* accepts incoming connections and provides new socket for communication*/
int sendData(SOCKET client_socket, const std::string& data); /* send data to socket*/
std::string receiveData(SOCKET client_socket); /* receiving data from client*/
void closeSocket(SOCKET socket_fd);

#endif