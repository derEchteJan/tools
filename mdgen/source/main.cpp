
#include <string>
#include <iostream> 

#include "markdownfile.h"
#include "search.h"
#include "settings.h"
#include "filesys.h"
#include "benchmark.h"
#include "stdx.h"


int main(int argc, char **argv)
{
    benchmark_start();

    Settings::init(argc, argv);
    Settings::print();

    // validate settings / arguments

#if 1
    bool fileSpecified = !Settings::fileParam.empty();
    bool dirSpecified = !Settings::dirParam.empty();
    if(!fileSpecified && !dirSpecified)
    {
        std::cout << "no input file/dir specified" << std::endl;
        exit(1);
    }
    if(fileSpecified && dirSpecified)
    {
        std::cout << "specify either file or dir" << std::endl;
        exit(1);
    }
    if(fileSpecified && !std::ends_with(Settings::fileParam, ".md"))
    {
        std::cout << "invalid file name: '" << Settings::fileParam << "', must end with .md" << std::endl;
        exit(1);
    }
    if(dirSpecified && Settings::dirParam[0] == '/')
    {
        std::cout << "invalid dir path, leading '/' not allowed" << std::endl;
        exit(1);
    }
#endif

    // parse file

    if(fileSpecified)
    {
        MarkdownFile mdFile(Settings::fileParam);
        if(mdFile.parse())
        {
            mdFile.serialize();
        }
    }
    else // dirSpecified
    {
        Filesys::handlers_t handlers;
        handlers.maxDepth = -1;
        handlers.onFile = [](const std::string &name, const std::string &abs, int depth)
        {
            //std::cout << "iterating: " << abs << std::endl;
            if(std::ends_with(name, ".md"))
            {
                MarkdownFile mdFile(abs);
                if(mdFile.parse())
                {
                    mdFile.serialize();
                }
            }
        };
        Filesys::iterateDir(Settings::dirParam.c_str(), handlers);
    }

    OverviewFile overview("index.html" /* <- might be put into settings */);
    if(overview.parse())
    {
        overview.serialize();
    }

    Search search(Settings::documentRoot);
    search.createIndex();

    benchmark_end();
    return 0;
}