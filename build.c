#include <dirent.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>

// This is win32 only, linux has dirent::d_type
#include <sys/types.h>
#include <sys/stat.h>

#define SRC_FILE_EXT ".c"
#define SRC_PATH "./src/"

#define STR_ARENA_SIZE (1024 * 1024)
void *arena_next;
void *arena_start;

void arena_init()
{
    void *arena = calloc(STR_ARENA_SIZE, 1);
    assert(arena && "arena was null");

    arena_start = arena;
    arena_next = arena;
}

void *arena_alloc(size_t size)
{
    assert((int)((arena_next - arena_start) + size) <= STR_ARENA_SIZE);
    void *ptr = arena_next;
    arena_next += size;

    return ptr;
}

void arena_delete()
{
    free(arena_start);
}

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

bool is_regular_file(const char *path, const struct dirent *entry)
{
    char fullpath[512];
    snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);

    struct stat st;
    if (stat(fullpath, &st) != 0)
        return false;

    return S_ISREG(st.st_mode);
}

char *get_source_files_in_dir_as_str(const char *path)
{
    DIR *dir;
    struct dirent *entry;

    int maxSourceFiles = 1000;

    char **source_files = arena_alloc(maxSourceFiles * sizeof(char *));
    int numSourceFiles = 0;

    dir = opendir(path);
    if (dir == NULL)
    {
        assert(false && "opendir");
    }

    while ((entry = readdir(dir)) != NULL)
    {
        // push file path to source files array
        char fullpath[512];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);

        printf("File: %s\n", fullpath);

        if (!is_regular_file(path, entry))
            continue;

        int extLen = strlen(SRC_FILE_EXT);
        char *entryExt = &entry->d_name[strlen(entry->d_name) - extLen];

        // if the ext isnt correct, skip.
        if (!strcmpi(entryExt, SRC_FILE_EXT) == 0)
            continue;
        int pathLen = strlen(fullpath);

        char *copy = arena_alloc(pathLen * sizeof(char));
        strcpy(copy, fullpath);

        source_files[numSourceFiles++] = copy;

        printf("Pushed File To List: %s %s\n", entry->d_name, copy);
    }

    closedir(dir);

    int totalLength = 0;
}

int main()
{
    arena_init();

    char *src_files = get_source_files_in_dir_as_str(SRC_PATH);

    arena_delete();

    return 0;
}