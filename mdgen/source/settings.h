#pragma once

#include <string>

class Settings
{
public:
    static const char* const ENABLED; // "true"
    static const char* const DISABLED; // "false"

    enum LogLevel
    {
      E_DEFAULT = 0, E_DETAILED = 1, E_VERBOSE = 2
    };

    // Settings variables:
    static std::string documentRoot;    /** arg 'document_root=...' */
    static std::string pagesDir;
    static std::string attachmentsDir;
    static std::string fileParam;       /** arg 'file=...' */
    static std::string dirParam;        /** arg 'dir=...' */
    static std::string siteName;
    static std::string theme;
    static int         verbose;         /** arg 'verbose=...' */

    // Methods
    static void init(int argc, char **argv);
    static void print();

private:
    static void readValue(const char *key, std::string &targetVariable);
    static void readValueInt(const char *key, int &targetVariable);
};