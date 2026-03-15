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
        CREATE = 4,
        APPEND = 8
    };

    typedef std::function<void(const std::string &name, const std::string &abs, int depth)> dir_handler_t;
    typedef std::function<void(const std::string &name, const std::string &abs, int depth)> file_handler_t;
    typedef std::function<void(const std::string &line)> line_handler_t;

    typedef struct handlers_t
    {
        dir_handler_t onDirEnter = nullptr;
        dir_handler_t onDirExit = nullptr;
        file_handler_t onFile = nullptr;
        int maxDepth = -1; // -1 = recursive
        bool includeDirLeaves = false;
    } handlers_t;

    static void iterateDir(const char *path, handlers_t handlers, int depth = 0);
    static void readLines(int fd, line_handler_t forEachLine);

    static int open(const std::string &file, Flags flags);
    static int open(const std::string &file, int flags);
    static void close(int fd);

    static void write(int fd, const char *cstr);
    static void write(int fd, const std::string &file);
};