#ifndef RESPONSE_BUILDER_H
#define RESPONSE_BUILDER_H

#include <string>
#include <vector>

struct ResponseData {
	int status_code;                                          // 200, 404, 500, etc.
	std::string reason_phrase;                                // "OK", "Not Found", "Internal Server Error"
	std::vector<std::pair<std::string, std::string>> headers; // Header name-value pairs
	std::string body;                                         // Response body content
};

// Function declarations
ResponseData generateErrorResponse(int status_code, const std::string& message);
std::string serializeResponse(const ResponseData& response);
std::string getMimeType(const std::string& filename);

#endif
