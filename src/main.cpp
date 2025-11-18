#include <iostream>
#include <string>
#include <thread>
#include "server.h"
#include "request_parser.h"
#include "response_builder.h"
#include "file_handler.h"

// Global flag for graceful shutdown
volatile bool server_running = true;

// Forward declaration
void handleClient(SOCKET client_socket, FileHandler& file_handler);

// Simple signal handler for Ctrl+C
void signalHandler(int signal)
{
	std::cout << "\n[MAIN] Shutting down server..." << std::endl;
	server_running = false;
}

// Client handler function - runs in separate thread for each client
// This function processes one complete request-response cycle
void handleClient(SOCKET client_socket, FileHandler& file_handler)
{
	std::cout << "[HANDLER] Client thread started for socket: " << client_socket << std::endl;

	try
	{
		// STEP 1: Read request from socket
		std::cout << "[HANDLER] Reading request from client..." << std::endl;
		std::string raw_request = receiveData(client_socket);

		if (raw_request.empty())
		{
			std::cout << "[HANDLER] Empty request received" << std::endl;
			closeSocket(client_socket);
			std::cout << "[HANDLER] Client thread terminating" << std::endl;
			return;
		}

		// STEP 2: Parse the request
		std::cout << "[HANDLER] Parsing HTTP request..." << std::endl;
		RequestData request = parseRequest(raw_request);

		ResponseData response;

		// STEP 3: Validate request
		if (!request.is_valid)
		{
			std::cout << "[HANDLER] Invalid request: " << request.error_message << std::endl;
			response = generateErrorResponse(400, "Bad Request: " + request.error_message);
		}
		// STEP 4: Handle the request (currently only GET)
		else if (request.method == "GET")
		{
			std::cout << "[HANDLER] Handling GET request for: " << request.path << std::endl;
			response = file_handler.handleGetRequest(request.path);
		}
		else
		{
			std::cout << "[HANDLER] Unsupported method: " << request.method << std::endl;
			response = generateErrorResponse(405, "Method Not Allowed");
		}

		// STEP 5: Serialize response
		std::cout << "[HANDLER] Serializing response (status " << response.status_code << ")..." << std::endl;
		std::string response_str = serializeResponse(response);

		// STEP 6: Send response to client
		std::cout << "[HANDLER] Sending response to client..." << std::endl;
		int bytes_sent = sendData(client_socket, response_str);

		if (bytes_sent > 0)
		{
			std::cout << "[HANDLER] Sent " << bytes_sent << " bytes to client" << std::endl;
		}
		else
		{
			std::cout << "[HANDLER] Failed to send response" << std::endl;
		}

		// STEP 7: Close connection
		std::cout << "[HANDLER] Closing client connection..." << std::endl;
		closeSocket(client_socket);

		std::cout << "[HANDLER] Client thread terminating" << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cout << "[HANDLER] Exception in client handler: " << e.what() << std::endl;
		closeSocket(client_socket);
	}
	catch (...)
	{
		std::cout << "[HANDLER] Unknown exception in client handler" << std::endl;
		closeSocket(client_socket);
	}
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
	std::cout << "   HTTP/1.1 Server (Multi-threaded)" << std::endl;
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

	// STEP 4: Main server loop - Accept clients and create threads
	int client_count = 0;
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

		client_count++;
		std::cout << "[MAIN] Client #" << client_count << " connected" << std::endl;

		// STEP 4b: Create new thread for this client
		// The thread runs handleClient() independently
		// Main loop immediately returns to accept() waiting for next client
		try
		{
			std::thread client_thread(handleClient, client_socket, std::ref(file_handler));
			client_thread.detach();  // Let thread run independently, no need to join
			std::cout << "[MAIN] Created thread for client #" << client_count << std::endl;
		}
		catch (const std::exception& e)
		{
			std::cout << "[ERROR] Failed to create thread: " << e.what() << std::endl;
			closeSocket(client_socket);
		}
	}

	// STEP 5: Shutdown - Close listening socket
	std::cout << "\n[MAIN] Closing listening socket..." << std::endl;
	closeSocket(server.listening_socket);

	// STEP 6: Cleanup Winsock
	std::cout << "[MAIN] Cleaning up Winsock..." << std::endl;
	WSACleanup();

	std::cout << "[SUCCESS] Server shut down gracefully" << std::endl;
	std::cout << "[INFO] Handled " << client_count << " client connections" << std::endl;
	return 0;
}
