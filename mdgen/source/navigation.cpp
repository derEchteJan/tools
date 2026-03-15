#include "navigation.h"

#include "stdx.h"
#include "markdownfile.h"
#include "filesys.h"

#include <algorithm>

#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

static auto s_oddColor = "var(--surface2)";
static auto s_evenColor = "var(--surface3)";

void Navigation::serialize()
{
    auto file = m_parentFile;
    if(!file) return;

    //iterateDir(m_parentFile->getRootPath().c_str());
    int rootPathLen = file->getRootPath().length();

    Filesys::handlers_t handlers;
    handlers.onDirEnter = [&](const std::string &name, const std::string &path, int depth) {
        file->serialize_writeln("<div>");

        file->serialize_write("<span class=\"navdir\"><i>");
        file->serialize_write(name);
        file->serialize_writeln("</i></span><br>");

        file->serialize_writeln("<div style=\"margin-left: 10pt\">");
    };
    handlers.onDirExit = [&](const std::string &name, const std::string &path, int depth) {
        file->serialize_writeln("</div>");
        file->serialize_writeln("</div>");
    };
    handlers.onFile = [&](const std::string &name, const std::string &path, int depth) {
        if(!std::ends_with(name, ".html")) return;
        
        std::string url = path.substr(rootPathLen);
        std::string displayName = name.substr(0, name.length() - strlen(".html"));
        std::replace(displayName.begin(), displayName.end(), '_', ' ');
        
        file->serialize_write("<a class=\"navitem\" href=\"");
        file->serialize_write(url);
        file->serialize_write("\">- ");
        file->serialize_write(displayName);
        file->serialize_writeln("</a><br>");
    };

    file->serialize_write("<span class=\"navdir\"><i>");
    file->serialize_write("<a href=\"/\">");
    file->serialize_write("HOME");
    file->serialize_write("</a>");
    file->serialize_writeln("</i></span><br>");

    file->serialize_writeln("<div style=\"margin-left: 10pt\">");

    Filesys::iterateDir(file->getRootPath().c_str(), handlers);

    file->serialize_writeln("</div>");
}

void Navigation::iterateDir(const char *path, const char *name, int indent, int oddEvenCount)
{
    auto file = m_parentFile;
    if(!file) return;

    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(path))) return;

    std::string bg = indent % 2 == 0 ? s_evenColor : s_oddColor;
    
    std::string divType = oddEvenCount % 2 == 0 ? "naveven" : "navodd";
    oddEvenCount++;

    file->serialize_writeln("<div class=\"" + divType + "\">");
    
    file->serialize_write("<span class=\"navdir\"><i>");
    if(name)
        file->serialize_write(name);
    else
    {
        file->serialize_write("<a href=\"/\">");
        file->serialize_write("HOME");
        file->serialize_write("</a>");
    }
    file->serialize_write("</i></span>");
    file->serialize_writeln("<br>");

    file->serialize_writeln("<div style=\"margin-left: 10pt\">");

    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == DT_DIR)
        {
            // iterate directories

            char next_path[1024];
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0 || entry->d_name[0] == '.')
                continue;

            snprintf(next_path, sizeof(next_path), "%s/%s", path, entry->d_name);
            iterateDir(next_path, entry->d_name, indent + 1, oddEvenCount++);
        }
    }

    if (!(dir = opendir(path))) return;

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG)
        {
            // iterate regular files

            // TODO: skip certain file types that should be hidden

            std::string url(path);
            url.append("/").append(entry->d_name);
            url = url.substr(m_parentFile->getRootPath().length());

            //file->serialize_write(std::string("<a href=\"") + url + "\">" + entry->d_name + "</a>");
            file->serialize_write("<a class=\"navitem\" href=\"");
            file->serialize_write(url);
            file->serialize_write("\">- ");
            file->serialize_write(entry->d_name);
            file->serialize_write("</a><br>");
        }
    }

    closedir(dir);

    file->serialize_writeln("</div>");
    file->serialize_writeln("</div>");
}