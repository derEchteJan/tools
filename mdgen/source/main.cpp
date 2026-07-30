
#include <string>
#include <iostream> 

#include "logging.h"
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

    bool fileSpecified = !Settings::fileParam.empty();
    bool dirSpecified = !Settings::dirParam.empty();
    if(!fileSpecified && !dirSpecified)
    {
        log_err( "no input file/dir specified" );
        exit(1);
    }
    if(fileSpecified && dirSpecified)
    {
        log_err( "specify either file or dir" );
        exit(1);
    }
    if(fileSpecified && !std::ends_with(Settings::fileParam, ".md"))
    {
        log_err( "invalid file name: '" << Settings::fileParam << "', must end with .md" );
        exit(1);
    }
    if(dirSpecified && Settings::dirParam[0] == '/')
    {
        log_err( "invalid dir path, leading '/' not allowed" );
        exit(1);
    }

    // parse file

    if(fileSpecified)
    {
        // single file specified

        MarkdownFile mdFile(Settings::fileParam);
        if(mdFile.parse())
        {
            mdFile.serialize();
            Search::createIndexFor(mdFile);
        }
    }
    else
    {
        // directory specified

        Filesys::handlers_t handlers;
        handlers.maxDepth = -1;
        handlers.onFile = [](const std::string &name, const std::string &abs, int depth)
        {
            if(std::ends_with(name, ".md"))
            {
                MarkdownFile mdFile(abs);
                if(mdFile.parse())
                {
                    mdFile.serialize();
                    Search::createIndexFor(mdFile);
                }
            }
        };
        std::string dir = Settings::documentRoot + '/' + Settings::dirParam;
        Filesys::iterateDir(dir.c_str(), handlers);
    }

    OverviewFile overview("index.html" /* <- TODO: might be put into settings */);
    if(overview.parse())
    {
        overview.serialize();
    }

    Search::createRootIndex(Settings::documentRoot);

    benchmark_end();
    return 0;
}