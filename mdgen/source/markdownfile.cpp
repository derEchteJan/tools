#include "markdownfile.h"

#include "stdx.h"

#include "markdownelement.h"
#include "navigation.h"
#include "filesys.h"
#include "settings.h"

#include <iostream>
#include <string>
#include <algorithm>

#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

// class TemplateFile

TemplateFile::TemplateFile()
    : m_rootPath()
    , m_indentNext(false)
{
    m_pageTitle = "Rezepte"; // TODO: settings
    m_rootPath = Settings::documentRoot;
}

TemplateFile::~TemplateFile()
{
}

void TemplateFile::serialize()
{
    m_fdOut = open(m_outPath.c_str(), O_TRUNC | O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if(m_fdOut == -1)
    {
        std::cout << "unable to open output file '" << m_outPath << "'" << std::endl;
        exit(1);
    }

    m_templateFile = fopen(m_templatePath.c_str(), "r");
    if(!m_templateFile)
    {
        std::cout << "unable to open template file '" << m_templatePath << "'" << std::endl;
        exit(1);
    }

    // read lines of template
    char *line_cstr = nullptr;
    size_t len = 0;
    ssize_t read = -1;
    while ((read = getline(&line_cstr, &len, m_templateFile)) != -1)
    {
        std::string line(line_cstr);
        auto sectionPrefix = "__";
        auto prefixLen = strlen(sectionPrefix);
        if(line.find(sectionPrefix) == 0 && line.find(sectionPrefix, prefixLen) == line.length() - prefixLen - 1)
        {
            serializeSection(line.substr(prefixLen, line.length() - 2 * prefixLen - 1));
        }
        else
        {
            serialize_write(line);
        }
#if 0
        if(line == "__TITLE__\n")
        {
            serialize_writeln(m_pageTitle);
        }
        else if(line == "__NAV__\n")
        {
            Navigation nav(this);
            nav.serialize();
        }
        else if(line == "__CONTENT__\n")
        {
            if(!m_elements.empty())
            {
                for(auto element : m_elements)
                {
                    element->serialize();
                }
            }
            else
            {
                serialize_writeln("<p><span id=\"empty-placeholder\">Empty Page</span></p>");
            }
        }
        else
        {
            serialize_write(line);
        }
#endif
    }
}

void TemplateFile::serialize_write(const std::string &str, int indent)
{
    auto tab = "  ";
    auto tabLen = 2;
    auto baseIndent = 2; // TODO: set property via method

    if(m_indentNext)
    {
        if(indent != -1)
            indent += baseIndent;
        m_indentNext = false;
    }
    else
    {
        indent = 0;
    }

#define USE_STDOUT 0
#if USE_STDOUT
    for(int n = 0; n < indent; n++)
        std::cout << tab;
    std::cout << str;
#endif
    if(m_fdOut != -1)
    {
        for(int n = 0; n < indent; n++)
            write(m_fdOut, tab, tabLen);
        write(m_fdOut, str.c_str(), str.length());
    }
}

void TemplateFile::serialize_writeln(const std::string &line, int indent)
{
    serialize_write(line, indent);
    serialize_write("\n");
    m_indentNext = true;
}

// class MarkdownFile

MarkdownFile::MarkdownFile(const std::string &filePath)
        : TemplateFile()
        , m_scriptPath(filePath)
        //, m_outPath()

        //, m_pageTitle("Markdown Page")
        //, m_parsed(false)
        , m_inFile(nullptr)
        //, m_outFile(nullptr)
        , m_fdSrc(-1)
        //, m_fdOut(-1)
{
    m_templatePath = m_rootPath + "/.templates/main.html";

    if(m_scriptPath.find(".md") == m_scriptPath.length() - 3)
    {
        m_outPath = m_scriptPath.substr(0, m_scriptPath.length() - 3).append(".html");
    }
    std::cout << "input file: " << m_scriptPath << std::endl;
    std::cout << "output file: " << m_outPath << std::endl;
}

MarkdownFile::~MarkdownFile()
{
    m_elements.clear();
}

bool MarkdownFile::parse()
{
#if 1
    m_inFile = fopen(m_scriptPath.c_str(), "r");
    if(!m_inFile)
    {
        std::cout << "unable to open input file '" << m_scriptPath << "'" << std::endl;
        exit(1);
    }

    std::cout << "parsing file: " << m_scriptPath << std::endl << std::endl;

    char *line_cstr = nullptr;
    size_t len = 0;
    ssize_t read = -1;

    while ((read = getline(&line_cstr, &len, m_inFile)) != -1)
    {
        if(line_cstr[read - 1] == '\n')
            line_cstr[read - 1] = 0; // trunc off lb
        
        std::string line(line_cstr);
        if(line == "END") break;
        parseLine(line);
    }
    parseLine(""); // finish up open blocks
#endif

    return true;
}

void MarkdownFile::serializeSection(const std::string &section)
{
    if(section == "NAV")
    {
        Navigation nav(this);
        nav.serialize();
    }
    else if(section == "CONTENT")
    {
        if(!m_elements.empty())
        {
            for(auto element : m_elements)
            {
                element->serialize();
            }
        }
        else
        {
            serialize_writeln("<p><span id=\"empty-placeholder\">Empty Page</span></p>");
        }
    }
}

void MarkdownFile::parseLine(const std::string &line)
{
    // one time init
    if(m_prototypeElements.empty())
    {
        m_prototypeElements.emplace_back(new CodeBlock(this));
        m_prototypeElements.emplace_back(new Headline(this, 3));
        m_prototypeElements.emplace_back(new Headline(this, 2));
        m_prototypeElements.emplace_back(new Headline(this, 1));
        m_prototypeElements.emplace_back(new Blockquote(this));
        m_prototypeElements.emplace_back(new Table(this));
        m_prototypeElements.emplace_back(new Paragraph(this)); // paragraph always hits true so needs to be last
    }

    // actual method:

    std::cout << "-- >>> '" << line << "'" << std::endl;

    if(m_currentElement)
    {
        if(m_currentElement->endsWith(line))
        {
            std::cout << "-- end: " << m_currentElement->typeName() << std::endl << std::endl;
            m_elements.push_back(m_currentElement);
            m_currentElement = nullptr;
        }
        else
        {
            std::cout << "-- " << m_currentElement->typeName() << "::parseLine($line)" << std::endl;
            m_currentElement->parseLine(line); // TODO: needs isLastLine so blocks may decide what to do with it
        }
    }
    else // no current element
    {
        if(line.empty())
        {
            std::cout << "-- skip" << std::endl;
            return;
        }
        for(auto prototype : m_prototypeElements)
        {
            if(prototype->startsWith(line))
            {
                m_currentElement = prototype->clone();
                break;
            }
        }
        std::cout << "-- begin: " << m_currentElement->typeName() << std::endl;
        if(m_currentElement)
        {
            std::cout << "-- " << m_currentElement->typeName() << "::parseLine($line, true)" << std::endl;
            m_currentElement->parseLine(line, /*isFirstLine:*/true);

            if(!m_currentElement->isBlock())
            {
                std::cout << "-- end: " << m_currentElement->typeName() << std::endl << std::endl;
                m_elements.push_back(m_currentElement);
                m_currentElement = nullptr;
            }
        }
        // TODO: parse start line attributes if implemented later
    }
    
}

// class Overview

OverviewFile::OverviewFile(const std::string &filePath)
    : TemplateFile()
{
    m_templatePath = m_rootPath + "/.templates/overview.html";
    m_outPath = m_rootPath + "/" + filePath;
}

OverviewFile::~OverviewFile() = default;

void OverviewFile::serializeSection(const std::string &section)
{
    if(section == "OVERVIEW")
    {
        serializeOverview();
    }
    if(section == "TITLE")
    {
        serialize_writeln(m_pageTitle);
    }
}

bool OverviewFile::parse()
{
    // TODO
    return true;
}

void OverviewFile::serializeOverview()
{
    // TODO
    serialize_writeln("<!--~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~-->");
    //serialize_writeln("<h2>Pages</h2>"); 

    Filesys::handlers_t handlers;

    int section = 0;
    const int nSections = 2; // number of sections of subdirs to include
    const int nListCols = 3; // TODO: settings

    handlers.maxDepth = 1;
    
    auto br = "<br>";

    handlers.onDirEnter = [&](const std::string &dirName, const std::string &absPath, int depth)
    {
        if(section >= nSections) return;

        std::string formattedName = dirName;
        formatFileName(formattedName);

        serialize_writeln("<h3>" + formattedName + "</h3>", depth);
        serialize_writeln("<ul class=\"overviewlist\">", depth);
    };

    handlers.onDirExit = [&](const std::string &dirName, const std::string &absPath, int depth)
    {
        if(section >= nSections) return;

        section++;

        serialize_writeln("</ul>", depth);
    };

    handlers.onFile = [&](const std::string &fileName, const std::string &absPath, int depth)
    {   
        if(depth != 0 && section >= nSections) return;

        if(!std::ends_with(fileName, ".html")) return;

        if(fileName == m_outPath) return;

        std::cout << " - " << fileName << std::endl;

        std::string formattedName = fileName;
        formatFileName(formattedName);

        std::string url = absPath.substr(m_rootPath.length());

        serialize_writeln("<li><a class=\"overview-pagelink\" href=\"" + url + "\">" + formattedName + "</a></li>", depth);
    };

    std::cout << "Overview: collecting files" << std::endl;

    Filesys::iterateDir(m_rootPath.c_str(), handlers);

    serialize_writeln("<!--~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~-->");
}

void OverviewFile::formatFileName(std::string &fileName)
{
    auto pos = fileName.find(".html");
    if(pos == fileName.length() - 5)
        fileName = fileName.substr(0, fileName.length() - 5);
    std::replace(fileName.begin(), fileName.end(), '_', ' ');
}