#include <dirent.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>

#define SRC_FILE_EXT ".c"
#define SRC_PATH "./src/"

int main()
{
    // have to define the winnt, lean and mean and nominmax for RGFW apparently
    int ret = system("gcc -Iinclude/ -Wall -O3 src/*.c -lopengl32 -lgdi32 -o game.exe");
    printf("Compiler exited with code %d\n", ret);

    return 0;
}