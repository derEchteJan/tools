#include <stdlib.h>
#include <fstream>
#include <string>
#include <vector>
#include <iostream> 

#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

//static const char *s_rootDir = "/var/www/html";

#if 0
void iterateDir(const char *name, int indent = 0)
{
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(name)))
        return;

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR)
        {
            // directory
            char path[1024];
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
            printf("%*s[%s]\n", indent, "", entry->d_name);
            iterateDir(path, indent + 2);
        } else {
            // file
            printf("%*s- %s\n", indent, "", entry->d_name);
            
        }
    }
    closedir(dir);
}
#endif

// string utils
bool startsWith(const std::string &str, const char* prefix)
{
    return prefix != nullptr && str.rfind(prefix, 0) == 0;
}

bool endsWith(std::string const &str, std::string const &suffix)
{
    if (suffix.size() > str.size()) return false;
    return std::equal(suffix.rbegin(), suffix.rend(), str.rbegin());
}

class MarkdownInlineElement
{
public:
    int m_startIdx;
    int m_endIdx;

    virtual void serialize() = 0;
};

#include "markdownfile.h"

int main(int argc, char **argv)
{
    (void)argc; (void)argv;

    // open input file (markdown)

    std::string inFilePath;
    for(int argi = 0; argi < argc; argi++)
    {
        std::string arg(argv[argi]);
        auto mdFileArg = "file=";
        if(startsWith(arg, mdFileArg) && strlen(mdFileArg) < arg.length())
        {
            inFilePath = arg.substr(strlen(mdFileArg));
        };
    }
    if(inFilePath.length() <= 0)
    {
        std::cout << "no input file specified" << std::endl;
        exit(1);
    }
    if(!endsWith(inFilePath, ".md") || inFilePath.length() <= 3)
    {
        std::cout << "invalid file name: '" << inFilePath << "'" << std::endl;
        exit(1);
    }
    
    // parse file

    MarkdownFile mdFile(inFilePath);
    if(mdFile.parse())
    {
        mdFile.serialize();
    }

    //iterateDir("/home/rico");

    return 0;
}