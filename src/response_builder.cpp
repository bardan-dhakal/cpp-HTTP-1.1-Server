#include "response_builder.h"
#include <iostream>
#include <map>
#include <sstream>

// Map file extensions to MIME types
std::map<std::string, std::string> mime_type_map = {
	{".html", "text/html"},
	{".htm", "text/html"},
	{".txt", "text/plain"},
	{".css", "text/css"},
	{".js", "application/javascript"},
	{".json", "application/json"},
	{".jpeg", "image/jpeg"},
	{".jpg", "image/jpeg"},
	{".png", "image/png"},
	{".gif", "image/gif"},
	{".svg", "image/svg+xml"},
	{".ico", "image/x-icon"},
	{".pdf", "application/pdf"},
	{".zip", "application/zip"},
	{"", "application/octet-stream"} // default
};

// Map status codes to reason phrases
std::map<int, std::string> status_reason_map = {
	{200, "OK"},
	{201, "Created"},
	{204, "No Content"},
	{400, "Bad Request"},
	{403, "Forbidden"},
	{404, "Not Found"},
	{405, "Method Not Allowed"},
	{413, "Payload Too Large"},
	{500, "Internal Server Error"},
	{503, "Service Unavailable"}
};

// Get MIME type from filename extension
std::string getMimeType(const std::string& filename)
{
	// Find last dot in filename
	size_t dot_pos = filename.find_last_of('.');

	if (dot_pos == std::string::npos)
	{
		// No extension
		return mime_type_map[""];
	}

	std::string extension = filename.substr(dot_pos);

	// Look up extension in map
	if (mime_type_map.find(extension) != mime_type_map.end())
	{
		return mime_type_map[extension];
	}

	// Default for unknown types
	return mime_type_map[""];
}

// Generate error response for given status code
ResponseData generateErrorResponse(int status_code, const std::string& message)
{
	ResponseData response;

	response.status_code = status_code;
	response.reason_phrase = status_reason_map[status_code];

	// Create simple error body
	std::ostringstream body_stream;
	body_stream << status_code << " " << response.reason_phrase << "\r\n";
	response.body = body_stream.str();

	// Add headers
	response.headers.push_back({"Content-Type", "text/plain"});
	response.headers.push_back({"Content-Length", std::to_string(response.body.length())});
	response.headers.push_back({"Connection", "close"});

	return response;
}

// Serialize ResponseData into HTTP response string
std::string serializeResponse(const ResponseData& response)
{
	std::ostringstream response_stream;

	// Write status line: HTTP/1.1 200 OK\r\n
	response_stream << "HTTP/1.1 " << response.status_code << " " << response.reason_phrase << "\r\n";

	// Write headers: Header-Name: Header-Value\r\n
	for (const auto& header : response.headers)
	{
		response_stream << header.first << ": " << header.second << "\r\n";
	}

	// Write blank line to separate headers from body
	response_stream << "\r\n";

	// Write body
	response_stream << response.body;

	return response_stream.str();
}

// Helper function to create a successful response for file serving
ResponseData buildSuccessResponse(const std::string& file_content, const std::string& filename)
{
	ResponseData response;

	response.status_code = 200;
	response.reason_phrase = "OK";
	response.body = file_content;

	// Add headers
	response.headers.push_back({"Content-Type", getMimeType(filename)});
	response.headers.push_back({"Content-Length", std::to_string(file_content.length())});
	response.headers.push_back({"Connection", "keep-alive"});
	response.headers.push_back({"Server", "SimpleHTTPServer/1.0"});

	return response;
}
