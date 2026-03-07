#include "markdownfile.h"

#include "markdownelement.h"
#include "navigation.h"

#include <iostream>
#include <string>

#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>


MarkdownFile::MarkdownFile(const std::string &filePath)
        : m_srcPath(filePath)
        , m_outPath()
#if LIVE_PATHS
        , m_rootPath("/var/www/html/")
        , m_templatePath("/var/www/html/.templates/main.html")
#else
        , m_rootPath("/home/rico/Documents/mdgen/html/")
        , m_templatePath("/home/rico/Documents/mdgen/html/.templates/main.html")
#endif
        , m_pageTitle("Markdown Page")
        , m_parsed(false)
        , m_inFile(nullptr)
        , m_outFile(nullptr)
        , m_fdSrc(-1)
        , m_fdOut(-1)
{
    if(m_srcPath.find(".md") == m_srcPath.length() - 3)
    {
        m_outPath = m_srcPath.substr(0, m_srcPath.length() - 3).append(".html");
    }
    std::cout << "input file: " << m_srcPath << std::endl;
    std::cout << "output file: " << m_outPath << std::endl;
}

MarkdownFile::~MarkdownFile()
{
    m_elements.clear();
}

bool MarkdownFile::parse()
{
#if 1
    m_inFile = fopen(m_srcPath.c_str(), "r");
    if(!m_inFile)
    {
        std::cout << "unable to open input file '" << m_srcPath << "'" << std::endl;
        exit(1);
    }

    //FILE *fp = fdopen(m_fdSrc, "r");
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

void MarkdownFile::serialize()
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
    }
}

void MarkdownFile::serialize_write(const std::string &str)
{
#define USE_STDOUT 1
#if USE_STDOUT
    std::cout << str;
#endif
    if(m_fdOut != -1)
        write(m_fdOut, str.c_str(), str.length());
}

void MarkdownFile::serialize_writeln(const std::string &line)
{
    serialize_write(line);
    serialize_write("\n");
}

std::string MarkdownFile::getRootPath() { return m_rootPath; }

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