#include "util.h"
#include <algorithm>
#include <cctype>

// Remove leading and trailing whitespace from a string
std::string trim(const std::string& str)
{
	if (str.empty())
		return str;

	//first non-whitespace character
	size_t start = 0;
	while (start < str.length() && std::isspace(str[start]))
		start++;

	// If entire string is whitespace
	if (start == str.length())
		return "";

	//last non-whitespace character
	size_t end = str.length() - 1;
	while (end > start && std::isspace(str[end]))
		end--;

	// Return substring from start to end (inclusive)
	return str.substr(start, end - start + 1);
}

// Split a string by a delimiter character into a vector of tokens
std::vector<std::string> split(const std::string& str, char delimiter)
{
	std::vector<std::string> tokens;
	std::string token = "";

	for (size_t i = 0; i < str.length(); i++)
	{
		if (str[i] == delimiter)
		{
			if (!token.empty())
			{
				tokens.push_back(token);
				token = "";
			}
		}
		else
		{
			token += str[i];
		}
	}

	// Add the last token if exists
	if (!token.empty())
		tokens.push_back(token);

	return tokens;
}

// Convert a string to lowercase
std::string to_lowercase(const std::string& str)
{
	std::string result = str;

	for (size_t i = 0; i < result.length(); i++)
	{
		result[i] = std::tolower(result[i]);
	}

	return result;
}

// Convert a string to uppercase
std::string to_uppercase(const std::string& str)
{
	std::string result = str;

	for (size_t i = 0; i < result.length(); i++)
	{
		result[i] = std::toupper(result[i]);
	}

	return result;
}

// Check if a string contains a substring
bool contains(const std::string& str, const std::string& substring)
{
	if (substring.empty())
	{
		return true;  // Empty substring is always contained
	}
		

	return str.find(substring) != std::string::npos;
}

// Find a multi-byte sequence in a string and return its position
size_t find_sequence(const std::string& str, const std::string& sequence)
{
	if (sequence.empty())
	{
		return 0;  // Empty sequence is at position 0
	}
		
	return str.find(sequence);
}
