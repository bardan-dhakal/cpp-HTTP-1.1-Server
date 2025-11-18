#include <iostream>
#include <string>
#include "server.h"
#include "request_parser.h"
#include "response_builder.h"
#include "file_handler.h"

// Global flag for graceful shutdown
volatile bool server_running = true;

// Simple signal handler for Ctrl+C
void signalHandler(int signal)
{
	std::cout << "\n[MAIN] Shutting down server..." << std::endl;
	server_running = false;
}

int main(int argc, char* argv[])
{
	// Parse command-line arguments for port
	int port = 8080;  // Default port
	std::string webroot = "webroot";  // Default webroot

	if (argc > 1)
		port = std::stoi(argv[1]);
	if (argc > 2)
		webroot = argv[2];

	std::cout << "=====================================" << std::endl;
	std::cout << "   HTTP/1.1 Server" << std::endl;
	std::cout << "=====================================" << std::endl;
	std::cout << "[MAIN] Port: " << port << std::endl;
	std::cout << "[MAIN] Webroot: " << webroot << std::endl;

	// Initialize file handler
	FileHandler file_handler(webroot);

	// STEP 1: Create server socket
	std::cout << "\n[MAIN] Creating server socket..." << std::endl;
	SocketServer server = createServerSocket(port);

	if (server.listening_socket == INVALID_SOCKET)
	{
		std::cout << "[ERROR] Failed to create socket" << std::endl;
		return 1;
	}

	// STEP 2: Bind socket to port
	std::cout << "[MAIN] Binding socket to port " << port << "..." << std::endl;
	bindSocket(server);

	// STEP 3: Listen for connections
	std::cout << "[MAIN] Listening for connections..." << std::endl;
	listenSocket(server);

	std::cout << "\n[SUCCESS] Server started successfully!" << std::endl;
	std::cout << "Connect to: http://localhost:" << port << "/" << std::endl;
	std::cout << "[INFO] Press Ctrl+C to shut down" << std::endl;
	std::cout << "=====================================" << std::endl << std::endl;

	// STEP 4: Main server loop - Accept clients and handle requests
	while (server_running)
	{
		// STEP 4a: Accept incoming client connection
		std::cout << "[MAIN] Waiting for client connection..." << std::endl;
		SOCKET client_socket = acceptConnection(server);

		if (client_socket == INVALID_SOCKET)
		{
			std::cout << "[ERROR] Failed to accept connection" << std::endl;
			continue;
		}

		// STEP 4b: Read request from socket
		std::cout << "[MAIN] Reading request from client..." << std::endl;
		std::string raw_request = receiveData(client_socket);

		if (raw_request.empty())
		{
			std::cout << "[WARN] Empty request received" << std::endl;
			closeSocket(client_socket);
			continue;
		}

		// STEP 4c: Parse the request
		std::cout << "[MAIN] Parsing HTTP request..." << std::endl;
		RequestData request = parseRequest(raw_request);

		ResponseData response;

		// STEP 4d: Validate request
		if (!request.is_valid)
		{
			std::cout << "[ERROR] Invalid request: " << request.error_message << std::endl;
			response = generateErrorResponse(400, "Bad Request: " + request.error_message);
		}
		// STEP 4e: Handle the request (currently only GET)
		else if (request.method == "GET")
		{
			std::cout << "[MAIN] Handling GET request for: " << request.path << std::endl;
			response = file_handler.handleGetRequest(request.path);
		}
		else
		{
			std::cout << "[WARN] Unsupported method: " << request.method << std::endl;
			response = generateErrorResponse(405, "Method Not Allowed");
		}

		// STEP 4f: Serialize response
		std::cout << "[MAIN] Serializing response (status " << response.status_code << ")..." << std::endl;
		std::string response_str = serializeResponse(response);

		// STEP 4g: Send response to client
		std::cout << "[MAIN] Sending response to client..." << std::endl;
		int bytes_sent = sendData(client_socket, response_str);

		if (bytes_sent > 0)
		{
			std::cout << "[SUCCESS] Sent " << bytes_sent << " bytes to client" << std::endl;
		}
		else
		{
			std::cout << "[ERROR] Failed to send response" << std::endl;
		}

		// STEP 4h: Close connection
		std::cout << "[MAIN] Closing client connection..." << std::endl;
		closeSocket(client_socket);

		std::cout << "[MAIN] Request handling complete" << std::endl;
		std::cout << std::endl;  // Blank line for readability
	}

	// STEP 5: Shutdown - Close listening socket
	std::cout << "[MAIN] Closing listening socket..." << std::endl;
	closeSocket(server.listening_socket);

	// STEP 6: Cleanup Winsock
	std::cout << "[MAIN] Cleaning up Winsock..." << std::endl;
	WSACleanup();

	std::cout << "[SUCCESS] Server shut down gracefully" << std::endl;
	return 0;
}
