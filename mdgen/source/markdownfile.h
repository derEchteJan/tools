#pragma once

#include <vector>
#include <string>

class MarkdownElement;

class TemplateFile
{
protected:
    std::string m_rootPath;
    std::string m_templatePath;
    std::string m_outPath;
    std::string m_pageTitle;

    FILE *m_templateFile;
    FILE *m_outFile;
    int m_fdOut;

    bool m_parsed;
    bool m_indentNext;

public:
    TemplateFile();
    ~TemplateFile();

    void serialize();
    virtual bool parse() = 0;

protected:
    virtual void serializeSection(const std::string &section) = 0;

public:
    std::string getRootPath() { return m_rootPath; };
    void serialize_write(const std::string &str = "", int indent = 0);
    void serialize_writeln(const std::string &line = "", int indent = 0);
};

class MarkdownFile : public TemplateFile
{
    std::vector<MarkdownElement*> m_elements;
    std::vector<MarkdownElement*> m_prototypeElements;
    MarkdownElement *m_currentElement = nullptr;

    std::string m_scriptPath;

    FILE *m_inFile;
    int m_fdSrc;

public:
    MarkdownFile(const std::string &filePath/*, const std::string &rootPath*/);

    ~MarkdownFile();

    virtual bool parse() override;

protected:
    virtual void serializeSection(const std::string &section) override;

private:
    void parseLine(const std::string &line);
};

class OverviewFile : public TemplateFile
{
public:
    OverviewFile(const std::string &filePath);
    ~OverviewFile();
    
    OverviewFile(const OverviewFile &) = delete;
    OverviewFile(const OverviewFile &&) = delete;

    virtual bool parse() override;

protected:
    virtual void serializeSection(const std::string &section) override;

private:
    void serializeOverview();
    void formatFileName(/*in-out*/ std::string &fileName);
};