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

#include "czsurface.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

struct czsurface {
    int width;
    int height;
    int bpp;
    unsigned char * pixels;
};

static czsurface * czsurface_internal_alloc();
static void czsurface_internal_destroy(czsurface *);
static void czsurface_internal_blit(czsurface * srcsurface, czsurface * dstsurface, czpoint where);

czsurface * czsurface_load(const char * file) {
    czsurface * r = czsurface_internal_alloc();
    r->pixels = stbi_load(file, &(r->width), &(r->height), NULL, 4);
    r->bpp = 4;
    return r;
}

void czsurface_destroy(czsurface * surface) {
    if (surface == NULL)
        return;
    free(surface->pixels);
    czsurface_internal_destroy(surface);
}

czsize czsurface_size(czsurface * surface) {
    czsize r;
    r.w = (unsigned) surface->width;
    r.h = (unsigned) surface->height;
    return r;
}

czsurface * czsurface_create(unsigned width, unsigned height) {
    czsurface * s = czsurface_internal_alloc();
    s->pixels = calloc(height, width * 4);
    s->width = width;
    s->height = height;
    s->bpp = 4;
    return s;
}

czsurface_blit_status czsurface_blit(czsurface * src, czsurface * dst, czpoint dstpoint) {
    czsurface_internal_blit(src, dst, dstpoint);
    return CZSURFACE_BLIT_OK;
}

czsurface_save_status czsurface_save(czsurface * src, const char * dest, czsurface_save_format format) {
    int status = 0;
    void * pixels = src->pixels;
    switch (format) {
        case CZSURFACE_FORMAT_PNG: status = stbi_write_png(dest, src->width, src->height, 4, pixels, src->width * 4); break;
        case CZSURFACE_FORMAT_BMP: status = stbi_write_bmp(dest, src->width, src->height, 4, pixels); break;
        case CZSURFACE_FORMAT_TGA: status = stbi_write_tga(dest, src->width, src->height, 4, pixels); break;
        case CZSURFACE_FORMAT_HDR: status = stbi_write_hdr(dest, src->width, src->height, 4, pixels); break;
    }

    if (status > 0) {
        return CZSURFACE_SAVE_FAIL;
    }
    return CZSURFACE_SAVE_OK;
}

void * czsurface_pixels(czsurface * surface) {
    return surface->pixels;
}

/* internal stuff */
static czsurface * czsurface_internal_alloc() {
    czsurface * cz = calloc(sizeof(czsurface), 1);
    return cz;
}

static void czsurface_internal_destroy(czsurface * surface) {
    free(surface);
}

static void czsurface_internal_blit(czsurface * srcsurface, czsurface * dstsurface, czpoint where) {
    unsigned char * src = srcsurface->pixels;
    unsigned char * dst = dstsurface->pixels + ((where.y * dstsurface->width + where.x) * dstsurface->bpp);
    unsigned i = 0;
    int srcpitch = srcsurface->width * srcsurface->bpp;
    int dstpitch = dstsurface->width * dstsurface->bpp;
    for (i = 0; i < srcsurface->height; i++) {
        memcpy(dst, src, (unsigned) dstsurface->bpp * srcsurface->width);
        dst += dstpitch;
        src += srcpitch;
    }
}
