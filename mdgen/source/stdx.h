#pragma once

#include <string>

// stl if it was good

namespace std
{
/// std::string extensions
bool starts_with(const std::string &string, const char* prefix);
bool starts_with(const std::string &string, const std::string &prefix);
bool ends_with(const std::string &string, const std::string &suffix);
}