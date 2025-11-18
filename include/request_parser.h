#ifndef REQUEST_PARSER_H
#define REQUEST_PARSER_H

#include <string>
#include <vector>
#include <winsock2.h>
#include "util.h"

struct RequestData {
	std::string method;
	std::string path;
	std::string http_version;
	std::vector<std::pair<std::string, std::string>> headers;
	std::string body;
	bool is_valid;
	std::string error_message;
};

RequestData parseRequest(const std::string& raw_request);
std::string readRequestFromSocket(SOCKET client_socket);

#endif