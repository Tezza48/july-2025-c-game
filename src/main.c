#define TZL_IMPLEMENTATION
#include "tzl.h"

#define STB_DS_IMPLEMENTATION
#include "vendor/stb_ds.h"

#define STB_IMAGE_IMPLEMENTATION
#include "vendor/stb_image.h"

#include "app.h"

int main(int argc, char **argv)
{
    stbi_set_flip_vertically_on_load(true);

    app app = app_create();
    app_start(&app);

    app_cleanup(&app);

    return 0;
}