#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <vector>

std::string trim(const std::string& str);
std::vector<std::string> split(const std::string& str, char delimiter);
std::string to_lowercase(const std::string& str);
std::string to_uppercase(const std::string& str);
bool contains(const std::string& str, const std::string& substring);
size_t find_sequence(const std::string& str, const std::string& sequence);

#endif