#include "settings.h"

#include <cstring>
#include <iostream>

static const char s_argSep = '=';

const char * const Settings::ENABLED = "true";
const char * const Settings::DISABLED = "false";

// SETTING VARIABLES
std::string Settings::documentRoot = "/usr/local/apache2/htdocs";
std::string Settings::fileParam;
std::string Settings::dirParam;
std::string Settings::siteName = "Rezepte";


static std::string getEnvVar(const char *key)
{
    char *val = getenv(key);
    return val == NULL ? std::string() : std::string(val);
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
}

void Settings::print()
{
    std::cout << "Settings:" << std::endl << "{" << std::endl;
#define JAN_LOG(member) std::cout << "  " << #member << ": '" << Settings::member << "'" << std::endl
    JAN_LOG(documentRoot);
    JAN_LOG(fileParam);
    JAN_LOG(dirParam);
    JAN_LOG(siteName);
#undef JAN_LOG
    std::cout << "}" << std::endl;
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