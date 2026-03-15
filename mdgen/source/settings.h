#pragma once

#include <string>

class Settings
{
public:
    static const char* const ENABLED; // "true"
    static const char* const DISABLED; // "false"

    // Settings variables:
    static std::string documentRoot;    /** arg 'document_root=...' */
    static std::string fileParam;       /** arg 'file=...' */
    static std::string dirParam;        /** arg 'dir=...' */
    static std::string siteName;

    // Methods
    static void init(int argc, char **argv);
    static void print();

private:
    static void readValue(const char *key, std::string &targetVariable);
};