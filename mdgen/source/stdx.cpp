#include "stdx.h"

#include <cstring>

namespace std
{
bool starts_with(const std::string &string, const char* prefix)
{
    return prefix != nullptr && string.rfind(prefix, 0) == 0;
}

bool starts_with(const std::string &string, const std::string &prefix)
{
    return string.length() >= prefix.length() && string.rfind(prefix, 0) == 0;
}

bool ends_with(const std::string &string, const std::string &suffix)
{
    if (suffix.size() > string.size()) return false;
    return std::equal(suffix.rbegin(), suffix.rend(), string.rbegin());
}

std::string replace_suffix(const std::string &str, const char *repl, const char *with)
{
    if(ends_with(str, repl))
    {
        return str.substr(0, str.length() - strlen(repl)).append(with);
    }
    else
    {
        return str;
    }
}

}