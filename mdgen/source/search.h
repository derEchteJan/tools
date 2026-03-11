#pragma once

#include <string>

class Search
{
private:
    std::string m_rootPath;
    std::string m_outFilePath;

public:
    Search(const std::string &rootPath);

    void createIndex();
};