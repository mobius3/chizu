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
#include "czmap.h"
#include "SDL.h"
#include "SDL_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

struct czsurface {
    SDL_Surface * sdl_surface;
};

static czsurface * czsurface_internal_alloc();

czsurface * czsurface_load(const char * file) {
    czsurface * r = NULL;
    SDL_Surface * surface = IMG_Load(file);
    if (surface != NULL) {
         r = czsurface_internal_alloc();
         r->sdl_surface = surface;
         SDL_SetSurfaceBlendMode(r->sdl_surface, SDL_BLENDMODE_BLEND);
    } else {
        printf("%s\n", IMG_GetError());
    }

    return r;
}

void czsurface_destroy(czsurface * surface) {
    SDL_FreeSurface(surface->sdl_surface);
    free(surface);
}

czsize czsurface_size(czsurface * surface) {
    czsize r;
    r.w = surface->sdl_surface->w;
    r.h = surface->sdl_surface->h;
    return r;
}

czsurface * czsurface_create(unsigned width, unsigned height) {
    unsigned rmask, gmask, bmask, amask;
    SDL_Surface * s = NULL;
    czsurface * czs = NULL;
  #if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
  #else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
  #endif
    s = SDL_CreateRGBSurface(0, width, height, 32, rmask, gmask, bmask, amask);
    if (s != NULL) {
        SDL_SetSurfaceBlendMode(s, SDL_BLENDMODE_NONE);
        czs = czsurface_internal_alloc();
        czs->sdl_surface = s;
    }
    return czs;
}

czsurface_blit_status czsurface_blit(czsurface * src, czsurface * dst, czpoint dstpoint) {
    SDL_Rect dstrect;
    dstrect.x = dstpoint.x;
    dstrect.y = dstpoint.y;
    dstrect.w = dst->sdl_surface->w;
    dstrect.h = dst->sdl_surface->h;
    if (SDL_BlitSurface(src->sdl_surface, NULL, dst->sdl_surface, &dstrect) != 0) {
        return CZSURFACE_BLIT_FAIL;
    }

    return CZSURFACE_BLIT_OK;
}

czsurface_save_status czsurface_save(czsurface * src, const char * dest, czsurface_save_format format) {
    int status = 0;
    SDL_Surface * surface = src->sdl_surface;
    SDL_LockSurface(surface);
    void * pixels = src->sdl_surface->pixels;
    switch (format) {
        case CZSURFACE_FORMAT_PNG: status = stbi_write_png(dest, surface->w, surface->h, 4, pixels, surface->pitch); break;
        case CZSURFACE_FORMAT_BMP: status = stbi_write_bmp(dest, surface->w, surface->h, 4, pixels); break;
        case CZSURFACE_FORMAT_TGA: status = stbi_write_tga(dest, surface->w, surface->h, 4, pixels); break;
        case CZSURFACE_FORMAT_HDR: status = stbi_write_hdr(dest, surface->w, surface->h, 4, pixels); break;
    }
    SDL_UnlockSurface(surface);

    if (status > 0) {
        return CZSURFACE_SAVE_FAIL;
    }

    return CZSURFACE_SAVE_OK;
}

void * czsurface_lock(czsurface * surface) {
    if (SDL_MUSTLOCK(surface->sdl_surface)) {
        SDL_LockSurface(surface->sdl_surface);
    }
    return surface->sdl_surface->pixels;
}

void czsurface_unlock(czsurface * surface) {
    if (SDL_MUSTLOCK(surface->sdl_surface)) {
        SDL_UnlockSurface(surface->sdl_surface);
    }
}

/* internal stuff */
static czsurface * czsurface_internal_alloc() {
    czsurface * cz = calloc(sizeof(czsurface), 1);
    return cz;
}
