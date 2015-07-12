/*
The MIT License (MIT)

Copyright (c) 2015 Leonardo G. de Freitas

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "chizu.h"

/*
 * Chizu atlas generator, to demonstrate libchizu.
 * Usage:
 *  ./chizu <width> <height> <spec-output> <texture-output> <file 1> <file 2> [<file 3> ...]
 *
 * Chizu uses http://www.blackpawn.com/texts/lightmaps/ as its algorthimg.
 */
int main(int argc, char ** argv) {
    const char * helptext =
        "Chizu atlas generator, to demonstrate libchizu.\n"
        "Usage:\n"
        "  ./chizu <square-size> <output-base> <file 1> <file 2> [<file 3> ...]\n"
        "\n"
        "Chizu uses http://www.blackpawn.com/texts/lightmaps/ as its algorthimg.\n";
    int i = 0;
    chizu * atlas = NULL;

    /* check if minimum number of arguments supplied */
    if (argc < 6) {
        printf("%s\n", helptext);
        return 0;
    }

    /* initializes Chizu library */
    chizu_init();

    const char * base = argv[1];
    if (strlen(base) > 1019) {
        printf("Output base filename too big!");
        chizu_quit();
        return 0;
    }

    /* generate outputs */
    char spec[1024] = {0};
    char tex[1024] = {0};
    strcpy(spec, base);
    strcat(spec, ".txt");

    strcpy(tex, base);
    strcat(tex, ".png");

    /* Creates a new chizu atlas */
    atlas = chizu_create();

    /* Insert every file passed in in the atlas*/
    for (i = 2; i < argc; i++) {
        printf("Inserting %s... ", argv[i]);
        chizu_insert_status status = chizu_insert(atlas, argv[i]);
        switch(status) {
            case CHIZU_INSERT_FILEOPEN_FAIL:
                printf("FAILED: Failed to open file.\n");
            break;
            case CHIZU_INSERT_NOSPACE:
                printf("FAILED: Not enough space for it.\n");
            break;
            case CHIZU_INSERT_OK:
                printf("OK\n");
            break;
            case CHIZU_INSERT_FAIL:
            default:
                printf("FAILED\n");
            break;
        }
    }

    printf("Exporting to %s and %s... ", spec, tex);
    chizu_export(atlas, spec, tex, CHIZU_FORMAT_PNG);
    chizu_destroy(atlas);

    printf(" OK\n");
    chizu_quit();
    return 0;
}
