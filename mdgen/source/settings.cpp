#include "settings.h"

#include <cstring>
#include <iostream>

#include "logging.h"

static const char s_argSep = '=';

const char * const Settings::ENABLED = "true";
const char * const Settings::DISABLED = "false";

// SETTING VARIABLES
std::string Settings::documentRoot = "/usr/local/apache2/htdocs";
std::string Settings::pagesDir = "pages";
std::string Settings::attachmentsDir = ".attachments";
std::string Settings::fileParam;
std::string Settings::dirParam;
std::string Settings::siteName = "Rezepte";
int         Settings::verbose = 0; // 0 / false = quiet, 1 = simple, 2 = detailed


static std::string getEnvVar(const char *key)
{
    char *val = getenv(key);
    return val == NULL ? std::string() : std::string(val);
}

static int strToInt(const std::string &string)
{
    if(string == "true")
        return 1;
    if(string == "false")
        return 0;
    else
    {
        int result = atoi(string.c_str());
        return result;
    }
}

static int s_argc;
static char **s_argv;

void Settings::init(int argc, char ** argv)
{
    s_argc = argc;
    s_argv = argv;
    Settings::readValue("document_root", Settings::documentRoot);
    Settings::readValue("file", Settings::fileParam);
    Settings::readValue("dir", Settings::dirParam);
    Settings::readValue("site_name", Settings::siteName);
    Settings::readValueInt("verbose", Settings::verbose);
}

void Settings::print()
{
#define LOG_FUNC logd
#define LOG_VAR(member) LOG_FUNC( "  " << #member << ": '" << Settings::member << "'" );

    LOG_FUNC( "Settings:" );
    LOG_FUNC( "{" );
    LOG_VAR(documentRoot);
    LOG_VAR(fileParam);
    LOG_VAR(attachmentsDir);
    LOG_VAR(dirParam);
    LOG_VAR(siteName);
    LOG_VAR(verbose);
    LOG_FUNC( "}" );

#undef LOG_FUNC
#undef LOG_VAR
}

void Settings::readValue(const char *key, std::string &targetVariable)
{
    if(!key) return;

    auto keyLen = strlen(key);
    
    // read value from args if found
    for(int argi = 0; argi < s_argc; argi++)
    {
        auto arg = s_argv[argi];
        if(arg && strncmp(arg, key, keyLen) == 0)
        {
            auto argLen = strlen(arg);
            if(argLen > keyLen && arg[keyLen] == s_argSep)
            {
                targetVariable = std::string(&arg[keyLen + 1]);
            }
            else
            {
                targetVariable = Settings::ENABLED;
            }
            return;
        }
    }
    
    // alternatively read value from env if found
    char *envValue = getenv(key);
    if(envValue)
    {
        targetVariable = strlen(envValue) > 0 ? envValue : Settings::ENABLED;
        return;
    }
}

void Settings::readValueInt(const char *key, int &targetVariable)
{
    if(!key) return;

    auto keyLen = strlen(key);
    std::string rawValue;
    
    // read value from args if found
    for(int argi = 0; argi < s_argc; argi++)
    {
        auto arg = s_argv[argi];
        if(arg && strncmp(arg, key, keyLen) == 0)
        {
            auto argLen = strlen(arg);
            if(argLen > keyLen && arg[keyLen] == s_argSep)
            {
                rawValue = std::string(&arg[keyLen + 1]);
                targetVariable = strToInt(rawValue);
            }
            else
            {
                targetVariable = 1;
            }
            return;
        }
    }
    
    // alternatively read value from env if found
    char *envValue = getenv(key);
    if(envValue)
    {
        if(strlen(envValue) > 0)
        {
            rawValue = envValue;
            targetVariable = strToInt(rawValue);
        }
        else
        {
            targetVariable = 1;
        }
        return;
    }
}