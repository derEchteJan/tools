#pragma once

class MarkdownFile;

class Navigation
{
    MarkdownFile *m_parentFile;
public:
    Navigation(MarkdownFile *parentFile)
        : m_parentFile(parentFile)
    {
    }
    
    /*void parse();*/
    void serialize();

    private:
    void iterateDir(const char *path, const char *name = "ROOT", int indent = 0, int oddEvenCount = 0);
};