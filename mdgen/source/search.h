#pragma once

#include "markdownfile.h"

#include <string>

class Search
{
private:

public:
    //Search(const std::string &rootPath);

    static void createRootIndex(const std::string &rootPath);

    static void createIndexFor(const MarkdownFile &file);
};