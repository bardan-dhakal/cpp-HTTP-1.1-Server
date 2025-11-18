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

void listenSocket(const SocketServer& mySocket)
{
	if (listen(mySocket.listening_socket, 5) == SOCKET_ERROR)
	{
		int lasterror = WSAGetLastError();
		std::cout << "Can not listen on socket " << lasterror << std::endl;
		return;
	}

	std::cout << "Listening on socket ..." << std::endl;

	return;

}

SOCKET acceptConnection(const SocketServer& mySocket)
{
	sockaddr_in client_addr;
	int addr_size = sizeof(sockaddr_in);

	SOCKET client_socket = accept(mySocket.listening_socket, (sockaddr*)&client_addr, &addr_size);

	if (client_socket == INVALID_SOCKET)
	{
		int lasterror = WSAGetLastError();
		std::cout << "Accept failed with error: " << lasterror << std::endl;
		return INVALID_SOCKET;
	}

	std::cout << "Client connected, socket: " << client_socket << std::endl;
	return client_socket;
}

int sendData(SOCKET client_socket, const std::string& data)
{
	int total_bytes_sent = 0;
	int remaining_bytes;
	const char* pointer_to_data = nullptr;
	int result;

	while (total_bytes_sent < data.length())
	{
		remaining_bytes = data.length() - total_bytes_sent;
		pointer_to_data = data.c_str() + total_bytes_sent;

		result = send(client_socket, pointer_to_data, remaining_bytes, 0);

		if (result == SOCKET_ERROR)
		{
			int lasterror = WSAGetLastError();
			std::cout << "Could not send data: " << lasterror << std::endl;
			return -1;
		}

		total_bytes_sent += result;

	}

	return total_bytes_sent;
}

std::string receiveData(SOCKET client_socket)
{
	std::string accumulated_data = ""; //stores all received data
	char buffer[4096]; //buffer to store on every read
	int bytes_received = 0; //bytes received from recv() func

	while (true)
	{
		bytes_received = recv(client_socket, buffer, 4096, 0);

		if (bytes_received == SOCKET_ERROR)
		{
			int lasterror = WSAGetLastError();
			std::cout << "Receive failed with error: " << lasterror << std::endl;
			return accumulated_data;  // Return what we got so far
		}

		if (bytes_received == 0)
		{
			std::cout << "Client disconnected" << std::endl;
			return accumulated_data;
		}

		accumulated_data.append(buffer, bytes_received);

		if (accumulated_data.find("\r\n\r\n") != std::string::npos)
		{
			std::cout << "Complete request received" << std::endl;
			return accumulated_data;
		}

		if (accumulated_data.length() > 100000) //100KB limit
		{
			std::cout << "Request is too large" << std::endl;
			return accumulated_data;
		}

	}

}