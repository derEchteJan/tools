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

    virtual ~Navigation() = default;
    
    void serialize();

private:
    void iterateDir(const char *path, const char *name = nullptr, int indent = 0, int oddEvenCount = 0);
};

#if 0 /// NOTE JAN: unimplemented
class OverviewNavigation
{
    TemplateFile *m_parentFile;

public:
    OverviewNavigation(TemplateFile *parentFile)
        : m_parentFile(parentFile)
    {
    }

    virtual ~OverviewNavigation() = default;
    
    void serialize();

private:
    void iterateDir(const char *rootPath, int depth = 0);
};
#endif