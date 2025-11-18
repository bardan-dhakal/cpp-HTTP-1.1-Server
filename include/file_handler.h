#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include <string>
#include "response_builder.h"

class FileHandler {
public:
	FileHandler(const std::string& webroot);

	// Handle GET request
	ResponseData handleGetRequest(const std::string& requested_path);

	// Get file content
	std::string readFile(const std::string& file_path);

	// Check if file exists and is accessible
	bool fileExists(const std::string& file_path);

	// Map request path to actual filesystem path
	std::string mapPathToFile(const std::string& request_path);

	// Validate path (prevent directory traversal)
	bool validateSecurityPath(const std::string& file_path);

private:
	std::string webroot;  // Root directory for serving files
};

#endif
