#include "request_parser.h"
#include "server.h"
#include <iostream>

// Forward declarations for helper functions
void parseRequestLine(const std::string& line, RequestData& request);
void parseHeaders(const std::string& headers_section, RequestData& request);

// Main parsing function: Takes raw HTTP request and returns structured RequestData
RequestData parseRequest(const std::string& raw_request)
{
	RequestData request;
	request.is_valid = true;
	request.error_message = "";

	// Check if request is empty
	if (raw_request.empty())
	{
		request.is_valid = false;
		request.error_message = "Request is empty";
		return request;
	}

	// Find the blank line that separates headers from body (\r\n\r\n)
	size_t blank_line_pos = find_sequence(raw_request, "\r\n\r\n");

	if (blank_line_pos == std::string::npos)
	{
		request.is_valid = false;
		request.error_message = "Request incomplete: no blank line found";
		return request;
	}

	// Split request into headers section and body section
	std::string headers_section = raw_request.substr(0, blank_line_pos);
	std::string body_section = raw_request.substr(blank_line_pos + 4); // +4 to skip "\r\n\r\n"

	// Find the first line (request line)
	size_t first_line_end = find_sequence(headers_section, "\r\n");
	if (first_line_end == std::string::npos)
	{
		request.is_valid = false;
		request.error_message = "Request line not found";
		return request;
	}

	std::string request_line = headers_section.substr(0, first_line_end);
	std::string remaining_headers = headers_section.substr(first_line_end + 2); // +2 to skip "\r\n"

	// Parse request line (GET /index.html HTTP/1.1)
	parseRequestLine(request_line, request);

	if (!request.is_valid)
		return request;

	// Parse headers
	parseHeaders(remaining_headers, request);

	// Parse body
	request.body = body_section;

	// Validate request
	if (request.method.empty() || request.path.empty())
	{
		request.is_valid = false;
		request.error_message = "Invalid request: missing method or path";
		return request;
	}

	std::cout << "[PARSER] Request parsed successfully: " << request.method << " " << request.path << std::endl;
	return request;
}

// Parse the request line: "METHOD PATH VERSION"
void parseRequestLine(const std::string& line, RequestData& request)
{
	std::vector<std::string> tokens = split(line, ' ');

	if (tokens.size() != 3)
	{
		request.is_valid = false;
		request.error_message = "Request line must have 3 tokens (METHOD PATH VERSION)";
		return;
	}

	request.method = to_uppercase(tokens[0]);
	request.path = tokens[1];
	request.http_version = tokens[2];

	// Validate HTTP version
	if (request.http_version != "HTTP/1.0" && request.http_version != "HTTP/1.1")
	{
		request.is_valid = false;
		request.error_message = "Unsupported HTTP version: " + request.http_version;
		return;
	}

	// Validate method
	if (request.method != "GET" && request.method != "POST" && request.method != "PUT" &&
		request.method != "DELETE" && request.method != "HEAD" && request.method != "OPTIONS")
	{
		request.is_valid = false;
		request.error_message = "Unsupported HTTP method: " + request.method;
		return;
	}

	// Validate path starts with /
	if (request.path.empty() || request.path[0] != '/')
	{
		request.is_valid = false;
		request.error_message = "Request path must start with /";
		return;
	}
}

// Parse headers section: "Header-Name: Header-Value\r\nHeader2: Value2\r\n..."
void parseHeaders(const std::string& headers_section, RequestData& request)
{
	if (headers_section.empty())
		return;

	// Split headers by \r\n
	std::vector<std::string> header_lines;
	std::string current_line = "";

	for (size_t i = 0; i < headers_section.length(); i++)
	{
		if (i + 1 < headers_section.length() && headers_section[i] == '\r' && headers_section[i + 1] == '\n')
		{
			if (!current_line.empty())
				header_lines.push_back(current_line);
			current_line = "";
			i++; // Skip the \n
		}
		else
		{
			current_line += headers_section[i];
		}
	}

	// Add last line if exists
	if (!current_line.empty())
		header_lines.push_back(current_line);

	// Parse each header line
	for (const auto& header_line : header_lines)
	{
		// Find colon separator
		size_t colon_pos = header_line.find(':');

		if (colon_pos == std::string::npos)
		{
			// Invalid header, skip it
			continue;
		}

		// Extract name and value
		std::string header_name = header_line.substr(0, colon_pos);
		std::string header_value = header_line.substr(colon_pos + 1);

		// Trim whitespace
		header_name = trim(header_name);
		header_value = trim(header_value);

		// Normalize header name to lowercase for case-insensitive comparison
		header_name = to_lowercase(header_name);

		// Store header pair
		request.headers.push_back({header_name, header_value});
	}
}

// Read complete request from socket (combines with receiveData)
std::string readRequestFromSocket(SOCKET client_socket)
{
	// Use the receiveData function from server.cpp
	// It already handles accumulating data until \r\n\r\n is found
	return receiveData(client_socket);
}
