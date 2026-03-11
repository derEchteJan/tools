#pragma once

class TemplateFile;

class Navigation
{
    TemplateFile *m_parentFile;
public:
    Navigation(TemplateFile *parentFile)
        : m_parentFile(parentFile)
    {
    }
    
    /*void parse();*/
    void serialize();

    private:
    void iterateDir(const char *path, const char *name = "ROOT", int indent = 0, int oddEvenCount = 0);
};

class OverviewNavigation
{
    TemplateFile *m_parentFile;

public:
    OverviewNavigation(TemplateFile *parentFile)
        : m_parentFile(parentFile)
    {
    }
    
    void serialize();

    private:
    void iterateDir(const char *rootPath, int depth = 0);
};