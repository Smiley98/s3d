#include "File.h"
#include <cstdio>
#include <cstring>
#include <cassert>

bool IsFileExtension(const char* fileName, const char* ext)
{
    bool result = false;
    const char* fileExt;

    if ((fileExt = strrchr(fileName, '.')) != NULL)
    {
        if (strcmp(fileExt, ext) == 0) result = true;
    }

    return result;
}

void SaveFileText(const char* fileName, char* text)
{
    FILE* file = fopen(fileName, "wt");
    int count = fprintf(file, "%s", text);
    assert(count != 0);
    fclose(file);
}
