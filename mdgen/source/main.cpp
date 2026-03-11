
#include <string>
#include <iostream> 

#include "markdownfile.h"
#include "search.h"


// string utils
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
}


int main(int argc, char **argv)
{
    (void)argc; (void)argv;

    // open input file (markdown)

    std::string inFilePath;
    for(int argi = 0; argi < argc; argi++)
    {
        std::string arg(argv[argi]);
        std::string mdFileArg = "file=";
        if(std::starts_with(arg, mdFileArg) && mdFileArg.length() < arg.length())
        {
            inFilePath = arg.substr(mdFileArg.length());
        };
    }
    if(inFilePath.length() <= 0)
    {
        std::cout << "no input file specified" << std::endl;
        exit(1);
    }
    if(!std::ends_with(inFilePath, ".md") || inFilePath.length() <= 3)
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

    OverviewFile overview("index.html");
    if(overview.parse())
    {
        overview.serialize();
    }

    Search search(mdFile.getRootPath());
    search.createIndex();

    return 0;
}