#pragma once

#include <vector>
#include <string>

class MarkdownElement;

class MarkdownFile
{
    std::vector<MarkdownElement*> m_prototypeElements;
    MarkdownElement *m_currentElement = nullptr;

    std::string m_srcPath, m_outPath, m_templatePath, m_rootPath;
    std::string m_pageTitle;
    bool m_parsed;
    FILE *m_inFile, *m_outFile, *m_templateFile;
    int m_fdSrc;
    int m_fdOut;
    std::vector<MarkdownElement*> m_elements;

public:
    MarkdownFile(const std::string &filePath/*, const std::string &rootPath*/);

    ~MarkdownFile();

    bool parse();
    void serialize();

    std::string getRootPath();

    // global way to write data

    void serialize_write(const std::string &str);
    void serialize_writeln(const std::string &line);

private:
    void parseLine(const std::string &line);
};