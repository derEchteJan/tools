#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

#include "search.h"
#include "filesys.h"
#include "stdx.h"

Search::Search(const std::string &rootPath)
    : m_rootPath(rootPath)
{
    m_outFilePath = m_rootPath + "/search.index";
}

static const char* getDummyTag1()
{
    static int s_dummyTagIdx1 = 0;
    static std::vector<const char*> s_dummyTags1 = {
        "vorspeisen",
        "nachspeisen",
        "hautpspeisen",
        "beilagen",
        "soßen"
    };
    auto result = s_dummyTags1.at(s_dummyTagIdx1++);
    s_dummyTagIdx1 %= s_dummyTags1.size();
    return result;
}

static const char* getDummyTag2()
{
    static int s_dummyTagIdx2 = 0;
    static std::vector<const char*> s_dummyTags2 = {
        "deutsch",
        "italienisch",
        "mexikanisch",
        "thailändisch",
        "indisch",
        "griechisch"
    };
    auto result = s_dummyTags2.at(s_dummyTagIdx2++);
    s_dummyTagIdx2 %= s_dummyTags2.size();
    return result;
}

void Search::createIndex()
{
    // TODO: need truncate / ftruncate option to work
    int outFd = Filesys::open(m_outFilePath, Filesys::RW | Filesys::CREATE);

    if(outFd == -1) { std::cout << "error opening search index file" << std::endl; return; }

    static std::vector<const char*> s_dummyTags = {
        "",
        ""
    };

    Filesys::handlers_t handler;
    auto sep = ", ";

    handler.onFile = [&](const std::string &fileName, const std::string &filePath, int depth)
    {
        if(!std::ends_with(fileName, ".html")) return;

        // TODO: Tags and other search meta data must be generated and retrieved on a per file basis and included to the lines
        std::string dummyTag1(getDummyTag1());
        std::string dummyTag2(getDummyTag2());
        std::string url = filePath.substr(m_rootPath.length());
        std::string displayName = fileName.substr(0, fileName.find("."));
        std::replace(displayName.begin(), displayName.end(), '_', ' ');

        auto line = url + sep + displayName + sep + dummyTag1 + sep + dummyTag2 + "\n";

        Filesys::write(outFd, line);
    };

    Filesys::iterateDir(m_rootPath.c_str(), handler);
}