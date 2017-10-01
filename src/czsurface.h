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

#ifndef CZSURFACE_H
#define CZSURFACE_H

#include "czsize.h"
#include "czpoint.h"

struct czsurface;
typedef struct czsurface czsurface;

typedef enum czsurface_blit_status {
    CZSURFACE_BLIT_OK = 0,
    CZSURFACE_BLIT_FAIL
} czsurface_blit_status;

typedef enum czsurface_save_status {
    CZSURFACE_SAVE_OK = 0,
    CZSURFACE_SAVE_OPEN_FAIL,
    CZSURFACE_SAVE_FAIL
} czsurface_save_status;

typedef enum czsurface_save_format {
    CZSURFACE_FORMAT_PNG,
    CZSURFACE_FORMAT_BMP,
    CZSURFACE_FORMAT_TGA,
    CZSURFACE_FORMAT_HDR
} czsurface_save_format;

czsurface * czsurface_load(const char * file);
czsurface * czsurface_create(unsigned width, unsigned height);
czsurface_blit_status czsurface_blit(czsurface * src, czsurface * dst, czpoint dstpoint);
czsurface_save_status czsurface_save(czsurface * src, const char * dest, czsurface_save_format format);
void czsurface_destroy(czsurface * surface);
czsize czsurface_size(czsurface * surface);
void * czsurface_pixels(czsurface * surface);


#endif
