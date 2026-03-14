#include "filesys.h"

#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

//typedef std::function<void(const std::string &name, const std::string &abs, int depth)> dir_handler_t;
//typedef std::function<void(const std::string &name, const std::string &abs, int depth)> file_handler_t;

void Filesys::iterateDir(const char *path, handlers_t handlers, int depth)
{
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(path))) return;

    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == DT_DIR)
        {
            // iterate directories

            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0 || entry->d_name[0] == '.')
                continue; // skip hidden and system directories
            
            bool depthReached = handlers.maxDepth != -1 && depth >= handlers.maxDepth;
            bool leafIncluded = !depthReached or handlers.includeDirLeaves;

            if(handlers.onDirEnter && leafIncluded)
                handlers.onDirEnter(std::string(entry->d_name), std::string(path) + "/" + entry->d_name, depth);
            
            if(!depthReached)
            {
                char next_path[1024];
                snprintf(next_path, sizeof(next_path), "%s/%s", path, entry->d_name);
                iterateDir(next_path, handlers, depth + 1);
            }

            if(handlers.onDirExit && leafIncluded)
                handlers.onDirExit(std::string(entry->d_name), std::string(path) + "/" + entry->d_name, depth);
        }
    }

    if (!(dir = opendir(path))) return;

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG)
        {
            // iterate regular files

            if(handlers.onFile)
                handlers.onFile(std::string(entry->d_name), std::string(path) + "/" + entry->d_name, depth);
        }
    }

    closedir(dir);
}

int Filesys::open(const std::string &file, Flags flags)
{
    int fd = -1;

    #define JAN_MAP_FLAG(_flags, _fromFlag, _toFlag) (_toFlag & -( ((_flags & _fromFlag) == _fromFlag) ) )
    int oflags =
        JAN_MAP_FLAG(flags, Flags::R,         O_RDONLY) |
        JAN_MAP_FLAG(flags, Flags::RW,        O_RDWR  ) |
        JAN_MAP_FLAG(flags, Flags::CREATE,    O_CREAT );
        //JAN_MAP_FLAG(flags, Flags::TRUNCATE,  O_TRUNC );
    #undef JAN_MAP_FLAG

    oflags |= (flags & Flags::APPEND) != 0 ? O_APPEND : O_TRUNC;

    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    fd = ::open(file.c_str(), oflags, mode);
    return fd;
}

int Filesys::open(const std::string &file, int flags)
{
    return Filesys::open(file, (Flags)flags);
}

void Filesys::write(int fd, const char *cstr)
{
    if(fd == -1) return;
    ::write(fd, cstr, strlen(cstr));
}

void Filesys::write(int fd, const std::string &file)
{
    if(fd == -1) return;
    ::write(fd, file.c_str(), file.length());
}