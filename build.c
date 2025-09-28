// gcc build.c -o build.exe && ./build.exe

#include <dirent.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>

#define STB_DS_IMPLEMENTATION
#include "vendor/stb_ds.h"

#define SRC_DIR "src"
#define DIST_DIR "dist"

#define ERROR(msg)                                   \
    do                                               \
    {                                                \
        printf("Error (%d): %s\n", __LINE__, (msg)); \
    } while (0)

bool ensure_dir_exists(const char *dir)
{
    struct stat st;
    if (stat(dir, &st) == -1)
    {
        if (mkdir(dir) != 0)
        {
            ERROR("Failed to create directory");
            return false;
        }
    }
    else if (!S_ISDIR(st.st_mode))
    {
        ERROR("Path exists but is not a directory");
        return false;
    }

    return true;
}

bool compile_program()
{

#define APPEND(buf, fmt, ...) \
    snprintf((buf) + strlen(buf), sizeof(buf) - strlen(buf), fmt, __VA_ARGS__)

    char **includes = NULL;
    arrput(includes, "-Iinclude/");

    char **src_files = NULL;
    arrput(src_files, SRC_DIR "/*.c");
    arrput(src_files, "vendor/*.c");

    char **flags = NULL;
    arrput(flags, "-Wall");
    arrput(flags, "-O0");
    arrput(flags, "-g");

    char **libs = NULL;
    arrput(libs, "-lopengl32");
    arrput(libs, "-lgdi32");

    char *output = DIST_DIR "/game.exe";

    char command[1024] = {0};
    APPEND(command, "%s", "gcc");

#define APPEND_LIST(list)                  \
    for (int i = 0; i < arrlen(list); ++i) \
    APPEND(command, " %s", list[i])

    APPEND_LIST(includes);
    APPEND_LIST(src_files);
    APPEND_LIST(flags);
    APPEND_LIST(libs);
    APPEND(command, " -o %s", output);
#undef APPEND_LIST

    printf("Executing: (%s)\n", command);

    int ret = system(command);
    printf("Compiler exited with code %d\n", ret);

    // optional cleanup
    arrfree(includes);
    arrfree(src_files);
    arrfree(flags);
    arrfree(libs);

    return ret == 0;
}

// bool compile_shaders_to_spv()
// {
//     if(system("glslc shaders/shader.vert --target-env=opengl -o dist/shader.vert.spv") != 0)
//     {
//         printf("Failed to compile vertex shader\n");
//         return false;
//     }

//     if(system("glslc shaders/shader.frag --target-env=opengl -o dist/shader.frag.spv") != 0)
//     {
//         printf("Failed to compile fragment shader\n");
//         return false;
//     }

//     return true;
// }

// File types that stat can determine
typedef enum file_type
{
    file_type_unknown = 0,
    file_type_regular,
    file_type_directory,
    file_type_count
} file_type;

// Use stat to determine file type and convert to file_type enum
file_type get_file_type(const char *file_path)
{
    struct stat file_stat;
    if (stat(file_path, &file_stat) != 0)
    {
        return file_type_unknown; // Error getting file info
    }

    if (S_ISREG(file_stat.st_mode))
    {
        return file_type_regular;
    }
    else if (S_ISDIR(file_stat.st_mode))
    {
        return file_type_directory;
    }
    else
    {
        return file_type_unknown; // Other types not handled
    }
}

bool copy_shaders_to_dist()
{
    char *in_dir = "shaders";

    // Move all .vert and .frag files from shaders/ to dist/ in a portable way
    DIR *dir = opendir(in_dir);
    if (!dir)
    {
        ERROR("Failed to open shaders directory");
        return false;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {

        char file_path[512];
        snprintf(file_path, sizeof(file_path), "%s/%s", in_dir, entry->d_name);

        file_type type = get_file_type(file_path);
        if (type == file_type_regular)
        {
            if (strstr(entry->d_name, ".vert") || strstr(entry->d_name, ".frag"))
            {
                char src_path[512];
                snprintf(src_path, sizeof(src_path), "%s/%s", in_dir, entry->d_name);

                char dest_path[512];
                snprintf(dest_path, sizeof(dest_path), "%s/%s", DIST_DIR, entry->d_name);

                // Copy the file to dest path
                FILE *src_file = fopen(src_path, "rb");
                if (!src_file)
                {
                    ERROR("Failed to open source shader file");
                    closedir(dir);
                    return false;
                }
                FILE *dest_file = fopen(dest_path, "wb");
                if (!dest_file)
                {
                    ERROR("Failed to open destination shader file");
                    fclose(src_file);
                    closedir(dir);
                    return false;
                }

                // size of file from src
                fseek(src_file, 0, SEEK_END);
                long file_size = ftell(src_file);
                fseek(src_file, 0, SEEK_SET);

                // Allocate buffer and read the file
                char *buffer = malloc(file_size);
                if (!buffer)
                {
                    ERROR("Failed to allocate memory for shader file");
                    fclose(src_file);
                    fclose(dest_file);
                    closedir(dir);
                    return false;
                }
                fread(buffer, 1, file_size, src_file);
                fwrite(buffer, 1, file_size, dest_file);

                free(buffer);
                fclose(src_file);
                fclose(dest_file);
            }
        }
    }

    closedir(dir);

    return true;
}

#define RUN_TASK(task)                                          \
    printf("Running task: \x1b[34m%s\x1b[0m\n", #task);         \
    if (!(task)())                                              \
    {                                                           \
        printf("  Task failed: \x1b[34m%s\x1b[0m\n", #task);    \
        return 1;                                               \
    }                                                           \
    else                                                        \
    {                                                           \
        printf("  Task succeeded: \x1b[34m%s\x1b[0m\n", #task); \
    }

bool ensure_dist_directory()
{
    if (!ensure_dir_exists(DIST_DIR))
    {
        ERROR("Failed to ensure dist directory exists");
        return false;
    }
    printf("Ensured dist directory exists: %s\n", DIST_DIR);

    return true;
}

int main()
{
    printf("Building project...\n\n");

    RUN_TASK(ensure_dist_directory);
    // RUN_TASK(compile_shaders_to_spv);
    RUN_TASK(copy_shaders_to_dist);
    RUN_TASK(compile_program);

    printf("\nBuild completed successfully!\n");

    return 0;
}