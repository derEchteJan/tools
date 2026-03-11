#pragma once

#include <string>
#include <functional>

class Filesys
{
public:
    enum Flags
    {
        R = 1, 
        RW = 2, 
        CREATE = 4
    };

    typedef std::function<void(const std::string &name, const std::string &abs, int depth)> dir_handler_t;
    typedef std::function<void(const std::string &name, const std::string &abs, int depth)> file_handler_t;

    typedef struct handlers_t
    {
        dir_handler_t onDirEnter = nullptr;
        dir_handler_t onDirExit = nullptr;
        file_handler_t onFile = nullptr;
        int maxDepth = 1;
        bool includeDirLeaves = false;
    } handlers_t;

    static void iterateDir(const char *path, handlers_t handlers, int depth = 0);

    static int open(const std::string &file, Flags flags);
    static int open(const std::string &file, int flags);

    static void write(int fd, const char *cstr);
    static void write(int fd, const std::string &file);
};