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

#include "chizu.h"
#include "czsurface.h"
#include "czmap.h"
#include <stdio.h>
#include <string.h>
#include <malloc.h>

static char * chizu_internal_strdup(const char *s);

/* data type declarations */

typedef struct czdata {
    char * file;
    czsurface * surface;
    struct chizu * atlas;
} czdata;

typedef struct czfuncdata {
    chizu_custom_export_func func;
    chizu_export_status status;
    void * data;
} czfuncdata;

struct chizu {
    czmap * map;
    czsurface * target;
    czsize size;
    FILE * output;
};



/* internal forward declarations */

static unsigned chizu_internal_next_power_of_2(unsigned n);
static chizu * chizu_internal_alloc();
static czdata * czdata_internal_alloc();
static void chizu_internal_free(chizu * cz);
static void czdata_internal_free(czdata * d);
static void czdata_internal_destroy(void * d);
static void czdata_internal_rect_export(czrect r, void * d, void * priv);
static void czdata_internal_custom_rect_blit(czrect r, void * d, void * priv);
static void chizu_internal_custom_rect_export(czrect r, void * d, void * priv);
static chizu_export_status chizu_internal_export_map(chizu * atlas, const char * spec);
static czrect chizu_internal_lease_or_enlarge(chizu * atlas, unsigned width, unsigned height, void * data);

chizu * chizu_create() {
    chizu * cz = chizu_internal_alloc();
    cz->size.w = 2;
    cz->size.h = 2;

    cz->map = czmap_create(cz->size.w, cz->size.h);
    if (cz->map == NULL) {
        chizu_internal_free(cz);
        return NULL;
    }
    return cz;
}

chizu_insert_status chizu_insert(chizu * atlas, const char * file) {
    czsize surfsize;
    czdata * data = czdata_internal_alloc();
    czsurface * surface = czsurface_load(file);
    if (surface == NULL) {
        czdata_internal_free(data);
        return CHIZU_INSERT_FILEOPEN_FAIL;
    }
    surfsize = czsurface_size(surface);
    data->file = chizu_internal_strdup(file);
    data->atlas = atlas;
    data->surface = surface;

    chizu_internal_lease_or_enlarge(atlas, surfsize.w, surfsize.h, data);
    return CHIZU_INSERT_OK;
}


chizu_export_status chizu_export(chizu * atlas, const char * spec, const char * texture, chizu_export_format format) {
    /* create surface target */
    atlas->target = czsurface_create(atlas->size.w, atlas->size.h);
    if (atlas->target == NULL) {
        return CHIZU_EXPORT_TEXTURE_FAIL;
    }

    chizu_export_status status = CHIZU_EXPORT_OK;
    if (chizu_internal_export_map(atlas, spec) != CHIZU_EXPORT_OK) {
        status = CHIZU_EXPORT_SPEC_FAIL;
    }

    czsurface_save_format sf;
    switch (format) {
        case CHIZU_FORMAT_PNG: sf = CZSURFACE_FORMAT_PNG; break;
        case CHIZU_FORMAT_TGA: sf = CZSURFACE_FORMAT_TGA; break;
        case CHIZU_FORMAT_HDR: sf = CZSURFACE_FORMAT_HDR; break;
        case CHIZU_FORMAT_BMP: sf = CZSURFACE_FORMAT_BMP; break;
        default: return CHIZU_EXPORT_FAIL;
    }

    if (czsurface_save(atlas->target, texture, sf) != CZSURFACE_SAVE_OK) {
        if (status == CHIZU_EXPORT_OK)
            status = CHIZU_EXPORT_TEXTURE_FAIL;
        else
            status = CHIZU_EXPORT_SPEC_AND_TEXTURE_FAIL;
    }

    return status;
}

chizu_export_status chizu_custom_export(chizu * atlas, chizu_custom_export_func f, void * priv) {
    czfuncdata data;
    data.func = f;
    data.data = priv;
    czmap_foreach(atlas->map, chizu_internal_custom_rect_export, &data);
    return data.status;
}

void chizu_pixel_data(chizu * atlas, chizu_receive_pixel_data_func f, void * priv) {
    czsurface * output = czsurface_create(atlas->size.w, atlas->size.h);
    if (f != NULL) {
        void * pixels = czsurface_pixels(output);
        czmap_foreach(atlas->map, czdata_internal_custom_rect_blit, output);
        f(pixels, atlas->size.w, atlas->size.h, 32, priv);
    }
}

void chizu_destroy(chizu * atlas) {
    czmap_destroy(atlas->map, czdata_internal_destroy);
    czsurface_destroy(atlas->target);
    chizu_internal_free(atlas);
}


/* internal stuff */

static unsigned chizu_internal_next_power_of_2(unsigned n) {
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n++;
    return n;
}

static chizu * chizu_internal_alloc() {
    chizu * cz = calloc(sizeof(chizu), 1);
    return cz;
}

static czdata * czdata_internal_alloc() {
    czdata * d = calloc(sizeof(czdata), 1);
    return d;
}

static void chizu_internal_free(chizu * cz) {
    free(cz);
}

static void czdata_internal_free(czdata * d) {
    free(d);
}

static void czdata_internal_destroy(void * d) {
    czdata * data = (czdata *) d;
    if (d == NULL)
        return;
    free(data->file);
    czsurface_destroy(data->surface);
    czdata_internal_free(data);
}

static void czdata_internal_rect_export(czrect r, void * d, void * priv) {
    czdata * data = (czdata *) d;
    czpoint dst = { r.x, r.y };
    FILE * out = data->atlas->output;
    fprintf(out, "%s %d %d %d %d\n", data->file, r.x, r.y, r.w, r.h);
    czsurface_blit(data->surface, data->atlas->target, dst);
}

static void czdata_internal_custom_rect_blit(czrect r, void * d, void * priv) {
    czsurface * output = (czsurface *) priv;
    czdata * data = (czdata *) d;
    czpoint dst = { r.x, r.y };
    czsurface_blit(data->surface, output, dst);
}


static void chizu_internal_custom_rect_export(czrect r, void * d, void * priv) {
    czfuncdata * funcdata = (czfuncdata *) priv;
    czdata * data = (czdata *) d;
    czexport exportdata;
    exportdata.subfile = data->file;
    exportdata.x = r.x;
    exportdata.y = r.y;
    exportdata.w = r.w;
    exportdata.h = r.h;
    funcdata->status = funcdata->func(&exportdata, priv);
    if (funcdata->status != CHIZU_EXPORT_OK)
        funcdata->status = CHIZU_EXPORT_FAIL;
}


static chizu_export_status chizu_internal_export_map(chizu * atlas, const char * spec) {
    atlas->output = fopen(spec, "w+");
    if (atlas->output == NULL) {
        return CHIZU_EXPORT_SPEC_FAIL;
    }
    czmap_foreach(atlas->map, czdata_internal_rect_export, NULL);
    fclose(atlas->output);
    atlas->output = NULL;
    return CHIZU_EXPORT_OK;
}

static czrect chizu_internal_lease_or_enlarge(chizu * atlas, unsigned width, unsigned height, void * data) {
    czrect resultrect;
    unsigned inc = 0;
    czmap * newmap = NULL;
    resultrect = czmap_lease(atlas->map, width, height, data);
    if (!czrect_is_empty(resultrect))
        return resultrect;

    /* find the largest size to expand map with */
    inc = width;
    if (width < height) inc = height;

    if (atlas->size.w > atlas->size.h)
        atlas->size.h += inc;
    else
        atlas->size.w += inc;

    atlas->size.w = chizu_internal_next_power_of_2(atlas->size.w);
    atlas->size.h = chizu_internal_next_power_of_2(atlas->size.h);

    /* create a new map and copy contents */
    newmap = czmap_create(atlas->size.w, atlas->size.h);
    czmap_copy(atlas->map, newmap);
    czmap_destroy(atlas->map, NULL);
    atlas->map = newmap;

    /* try again until space was found */
    return chizu_internal_lease_or_enlarge(atlas, width, height, data);
}

static char * chizu_internal_strdup(const char * s) {
    size_t n = strlen(s);
    char * p = (char *) malloc(n+1);
    if (p) strcpy(p,s);
    return p;
}
