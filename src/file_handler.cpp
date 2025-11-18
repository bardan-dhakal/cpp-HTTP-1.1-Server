#include "file_handler.h"
#include <iostream>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

// Constructor: Set the webroot directory
FileHandler::FileHandler(const std::string& webroot) : webroot(webroot)
{
	std::cout << "[FILE_HANDLER] Initialized with webroot: " << webroot << std::endl;
}

// Handle GET request: Maps path to file, reads it, returns response
ResponseData FileHandler::handleGetRequest(const std::string& requested_path)
{
	// Map request path to actual file path
	std::string file_path = mapPathToFile(requested_path);

	// Validate path (prevent directory traversal attacks)
	if (!validateSecurityPath(file_path))
	{
		std::cout << "[FILE_HANDLER] Security violation: " << file_path << std::endl;
		return generateErrorResponse(403, "Forbidden: Access denied");
	}

	// Check if file exists
	if (!fileExists(file_path))
	{
		std::cout << "[FILE_HANDLER] File not found: " << file_path << std::endl;
		return generateErrorResponse(404, "Not Found");
	}

	// Try to read file
	try
	{
		std::string file_content = readFile(file_path);
		std::cout << "[FILE_HANDLER] Served file: " << file_path << " (" << file_content.length() << " bytes)" << std::endl;

		// Build success response
		ResponseData response;
		response.status_code = 200;
		response.reason_phrase = "OK";
		response.body = file_content;

		// Determine MIME type from filename
		std::string mime_type = "application/octet-stream";
		size_t dot_pos = file_path.find_last_of('.');
		if (dot_pos != std::string::npos)
		{
			std::string ext = file_path.substr(dot_pos);
			// Simple MIME type mapping
			if (ext == ".html") mime_type = "text/html";
			else if (ext == ".txt") mime_type = "text/plain";
			else if (ext == ".css") mime_type = "text/css";
			else if (ext == ".js") mime_type = "application/javascript";
			else if (ext == ".json") mime_type = "application/json";
			else if (ext == ".png") mime_type = "image/png";
			else if (ext == ".jpg" || ext == ".jpeg") mime_type = "image/jpeg";
		}

		response.headers.push_back({"Content-Type", mime_type});
		response.headers.push_back({"Content-Length", std::to_string(file_content.length())});
		response.headers.push_back({"Connection", "keep-alive"});
		response.headers.push_back({"Server", "SimpleHTTPServer/1.0"});

		return response;
	}
	catch (const std::exception& e)
	{
		std::cout << "[FILE_HANDLER] Error reading file: " << e.what() << std::endl;
		return generateErrorResponse(500, "Internal Server Error");
	}
}

// Read entire file content into string
std::string FileHandler::readFile(const std::string& file_path)
{
	std::ifstream file(file_path, std::ios::binary);

	if (!file.is_open())
	{
		throw std::runtime_error("Cannot open file: " + file_path);
	}

	// Get file size
	file.seekg(0, std::ios::end);
	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);

	// Read entire file into string
	std::string content(size, '\0');
	file.read(&content[0], size);

	return content;
}

// Check if file exists and is a regular file (not directory)
bool FileHandler::fileExists(const std::string& file_path)
{
	try
	{
		fs::path p(file_path);
		return fs::exists(p) && fs::is_regular_file(p);
	}
	catch (const std::exception& e)
	{
		std::cout << "[FILE_HANDLER] Error checking file: " << e.what() << std::endl;
		return false;
	}
}

// Map request path like "/index.html" to actual filesystem path
std::string FileHandler::mapPathToFile(const std::string& request_path)
{
	// Combine webroot with request path
	// Example: webroot="webroot", request_path="/index.html" -> "webroot/index.html"

	std::string file_path = webroot;

	// Ensure webroot ends with separator
	if (file_path.back() != '\\' && file_path.back() != '/')
	{
		file_path += '\\';
	}

	// Remove leading slash from request path
	std::string clean_path = request_path;
	if (!clean_path.empty() && clean_path[0] == '/')
	{
		clean_path = clean_path.substr(1);
	}

	// Handle root request (/)
	if (clean_path.empty())
	{
		clean_path = "index.html";
	}

	file_path += clean_path;

	return file_path;
}

// Validate that path is within webroot (prevent directory traversal attacks)
bool FileHandler::validateSecurityPath(const std::string& file_path)
{
	try
	{
		// Get absolute (canonical) paths
		fs::path webroot_abs = fs::absolute(webroot);
		fs::path file_abs = fs::absolute(file_path);

		// Check if file path starts with webroot path
		// This prevents ../ attacks that try to escape the webroot
		std::string file_str = file_abs.string();
		std::string webroot_str = webroot_abs.string();

		// Ensure webroot ends with separator for proper prefix check
		if (webroot_str.back() != '\\')
			webroot_str += '\\';

		// Check if file path starts with webroot path
		return file_str.find(webroot_str) == 0;
	}
	catch (const std::exception& e)
	{
		std::cout << "[FILE_HANDLER] Error validating path: " << e.what() << std::endl;
		return false;
	}
}
