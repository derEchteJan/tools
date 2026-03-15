#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

#include <string.h>
#include "search.h"
#include "filesys.h"
#include "stdx.h"

static auto sep = ", ";

static std::string getTagFilePath(const MarkdownFile &file)
{
    std::string indexPath = std::replace_suffix(file.getScriptPath(), ".md", ".index");
    return indexPath;
}



static void serializeTags(int outFd, const std::string &tagFileName)
{
    auto tagsFile = Filesys::open(tagFileName, Filesys::R);
    if(tagsFile == -1) return;
    
    Filesys::readLines(tagsFile, [&](const std::string &line){
        Filesys::write(outFd, sep);
        Filesys::write(outFd, line);
    });
        
    Filesys::close(tagsFile);
}

void Search::createRootIndex(const std::string &rootPath)
{
    // TODO: need truncate / ftruncate option to work
    std::string outFilePath = rootPath + "/search.index"; // TODO: move to global constants

    int outFd = Filesys::open(outFilePath, Filesys::RW | Filesys::CREATE);

    if(outFd == -1) { std::cout << "error opening search index file" << std::endl; return; }

    static std::vector<const char*> s_dummyTags = {
        "",
        ""
    };

    Filesys::handlers_t handler;

    handler.onFile = [&](const std::string &fileName, const std::string &filePath, int depth)
    {
        if(!std::ends_with(fileName, ".html")) return;

        std::string url = filePath.substr(rootPath.length());
        std::string displayName = fileName.substr(0, fileName.find("."));
        std::replace(displayName.begin(), displayName.end(), '_', ' ');
        std::string tagFileName = std::replace_suffix(filePath, ".html", ".index");

        std::string line = url + sep + displayName;
        Filesys::write(outFd, line);

        // serialize tags from tag file
        serializeTags(outFd, tagFileName);

        Filesys::write(outFd, "\n");
    };

    Filesys::iterateDir(rootPath.c_str(), handler);
}

void Search::createIndexFor(const MarkdownFile &file)
{
    auto tags = file.getTags();

    if(tags.size() == 0) return;

    std::string tagFilePath = getTagFilePath(file);

    auto tagFile = Filesys::open(tagFilePath, Filesys::RW | Filesys::CREATE);
    if(tagFile == -1)
    {
        std::cout << "Search: unable to open index tag file '" << tagFilePath << "'" << std::endl;
        return;
    }

    for(auto tag : tags)
    {
        Filesys::write(tagFile, tag);
        Filesys::write(tagFile, "\n");
    }

    Filesys::close(tagFile);

    std::cout << "Created index tag file: '" << tagFilePath << "'" << std::endl;
}