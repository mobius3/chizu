# Chizu Texture Atlas Generator

Chizu is an atlas generator library and tool written in C99.

The library can be used to generate an atlas in runtime, while the tool can be used to pre-generate those atlas.

When pre-generating, Chizu outputs a png and text file containing the sub-images and their location, respectively.

## Dependencies

 - [SDL2](http://libsdl.org)
 - [SDL2\_image](https://www.libsdl.org/projects/SDL_image/)
 - [CMake](http://cmake.org) to build

## Build

- Clone the repo.
- Create a build folder inside.
- Inside the build folder, run `cmake ..`
- Run make (or mingw32-make depending on your platform)

An executable called `chizu` shoud be in the build folder, together with the library.

If you only want to build the library, you can pass `-DCHIZU_EXECUTABLE=OFF` in the
cmake call.

## Using the tool:

The chizu tool is a command-line executable to generate an atlas. It creates an image large enough to hold all the textures.

These are the arguments:

    ./chizu <base-file-name> <image1> <image2> [image3...]

Where

- `<base-file-name>` is the base name for the .txt and .png
- `<imageN>` a list files to put in the atlas. At leas two must be provided.

Example

    ./chizu characters player.png enemies.png npcs.png

Will generate a png called `characters.png` and a `characters.txt` file.

## Using the library:

Chizu can be integrated with a project that uses CMake already simply by adding `add_subdirectory(chizu)` in your project, assuming that `chizu` is a subfolder of it containing Chizu code.

Afterwards you can use `target_link_libraries(<target> chizu)` to link against the generated library.

If not using CMake, you can copy all the `.c` and `.h` files, except for `main.c`, and place in your project to compile it all together.

## Text output format:

The text file will contain the information of which image is stored where inside the atlas, in the following format:

    <input file> <x> <y> <width> <height>

Example

    player.png 0 0 128 128
    enemies.png 128 0 128 128
    npcs.png 256 0 128 128

## Image format:

The generated image is usually 32 bits per pixel (with alpha channel), if the output format allows.

## Example: generate and export an atlas.

    #include "chizu.h"

    int main() {
        chizu_init();
        chizu * atlas = chizu_create();

        chizu_insert(atlas, "player.png"); 
        chizu_insert(atlas, "enemies.png"); 
        chizu_insert(atlas, "npcs.png");
        chizu_export(atlas, "characters.txt", "characters.png", CHIZU_FORMAT_PNG);

        chizu_destroy(atlas);
        chizu_quit();
        return 0;
    }

## Example: Generate and obtain atlas in runtime.

    #include "chizu.h"

    chizu_export_status custom_export(czexport * node, void * priv) {
        printf("file %s is at %dx%d+%dx%d\n", node->subfile, node->x, node->y, node->w, node->h);
        return CHIZU_EXPORT_OK;
    }

    void read_pixels(const void * pixels, unsigned w, unsigned h, unsigned d, void * priv) {
        /* upload texture, convert pixels, whatever */
    }

    int main() {
        chizu_init();
        chizu * atlas = chizu_create();

        chizu_insert(atlas, "player.png"); 
        chizu_insert(atlas, "enemies.png"); 
        chizu_insert(atlas, "npcs.png");

        // custom_export is called for every subrect.
        // the third parameter will be passed in each call to custom_export
        chizu_custom_export(atlas, custom_export, NULL);

        // read_pixels is called with the resulting pixel data so far
        // the third parameter will be passed to read_pixels.
        chizu_pixel_data(atlas, read_pixels, NULL);

        chizu_destroy(atlas);
        chizu_quit();
        return 0;
    }

These two examples should cover all the public functions in Chizu.
