#include "pathexpander.hpp"

#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <wordexp.h>
#include <string.h>
#include <unistd.h>

const QString expandPath(const char *path)
{
    static auto appendCwd = [](const char *path, char *dst) {
        if (path[0] == '/') {
            strcpy(dst, path);
        } else {
            (void) getcwd(dst, PATH_MAX);
            strcat(dst, "/");
            strcat(dst, path);
        }
    };

    static auto removeJunk = [](char *begin, char *end) {
        while(*end != 0) { *begin++ = *end++; }
            *begin = 0;
    };

    static auto manualPathFold = [](char *path) -> char* {
        char *s, *priorSlash;
        while ((s=strstr(path, "/../")) != NULL) {
            *s = 0;
            if ((priorSlash = strrchr(path, '/')) == NULL) { /* oops */ *s = '/'; break; }
            removeJunk(priorSlash, s+3);
        }
        while ((s=strstr(path, "/./"))!=NULL) { removeJunk(s, s+2); }
        while ((s=strstr(path, "//"))!=NULL) { removeJunk(s, s+1); }
        s = path + (strlen(path)-1);
        if (s != path && *s == '/') { *s = 0; }
        return path;
    };

    static auto abspath = [](const char *file_name, char *resolved_name) -> char* {
        char buff[PATH_MAX+1];
        wordexp_t p;
        if (wordexp(file_name, &p, 0)==0) {
            appendCwd(p.we_wordv[0], buff);
            wordfree(&p);
        } else {
            appendCwd(file_name, buff);
        }
        if (realpath(buff, resolved_name) == NULL) { strcpy(resolved_name, manualPathFold(buff)); }
        return resolved_name;
    };

    char buff[PATH_MAX+1];
    return QString::fromUtf8(abspath(path, buff));
}

const QString expandPath(const QString &path)
{
    return expandPath(path.toUtf8().constData());
}
