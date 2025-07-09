#include <dirent.h>
#include <stdio.h>
#include <stdbool.h>

// This is win32 only, linux has dirent::d_type
#include <sys/types.h>
#include <sys/stat.h>

#define SRC_PATH "./src/"

bool is_directory(const char *path, const struct dirent *entry)
{
    char fullpath[512];
    snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);

    struct stat st;
    if (stat(fullpath, &st) != 0)
        return false;

    // Check whether the "if direcory" flag is set
    return S_ISDIR(st.st_mode);
}

bool is_file(const char *path, const struct dirent *entry)
{
    char fullpath[512];
    snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);

    struct stat st;
    if (stat(fullpath, &st) != 0)
        return false;

    return S_ISREG(st.st_mode);
}

int main()
{
    DIR *dir;
    struct dirent *entry;

    dir = opendir(SRC_PATH);
    if (dir == NULL)
    {
        perror("opendir");
        return 1;
    }

    while ((entry = readdir(dir)) != NULL)
    {

        if (is_directory(SRC_PATH, entry))
            continue;

        printf("%s\n", entry->d_name);
    }

    closedir(dir);

    return 0;
}