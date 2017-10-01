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

#include "czmap.h"
#include "SDL.h"

/* internal forward declarations */
static void czmap_internal_split(czmap * node, unsigned width, unsigned height);
static czmap * czmap_internal_find_space(czmap * node, unsigned width, unsigned height);
static czmap * czmap_internal_alloc(unsigned x, unsigned y, unsigned w, unsigned h);
static void czmap_internal_free(czmap * map);

/* public stuff */
struct czmap
{
    struct czmap * left;
    struct czmap * right;
    void * data;
    czrect rect;
};

typedef struct czmap_inserter_data {
    czmap * dst;
    unsigned char nospace;
} czmap_inserter_data;

czmap * czmap_create(unsigned width, unsigned height) {
    struct czmap * r = czmap_internal_alloc(0, 0, width, height);
    return r;
}

void czmap_destroy(czmap * map, czdestroyfunc func)  {
    if (map->left != NULL)
        czmap_destroy(map->left, func);
    if (map->right != NULL)
        czmap_destroy(map->right, func);
    if (func != NULL)
        func(map->data);
    czmap_internal_free(map);
}

czrect czmap_lease(czmap * map, unsigned width, unsigned height, void * data) {
    czrect r = { 0, 0, 0, 0 };
    czmap * node = czmap_internal_find_space(map, width, height);
    if (node == NULL)
        return r;

    node->data = data;
    return node->rect;
}

void czmap_foreach(czmap * map, czwalkfunc func, void * priv) {
    if (map->data != NULL)
        func(map->rect, map->data, priv);
    if (map->left != NULL)
        czmap_foreach(map->left, func, priv);
    if (map->right != NULL)
        czmap_foreach(map->right, func, priv);
}

void czmap_internal_inserter(czrect rect, void * data, void * priv) {
    czmap_inserter_data * idata = (czmap_inserter_data*) priv;
    if (idata->nospace) return;

    czrect result = czmap_lease(idata->dst, rect.w, rect.h, data);
    if (czrect_is_empty(result))
        idata->nospace = 1;
}

czmap_copy_status czmap_copy(czmap * src, czmap * dst) {
    czmap_inserter_data idata;
    idata.dst = dst;
    idata.nospace = 0;

    /* inserter will find space. idata.nospace will be = 1 if
     * it failed somewhere */
    czmap_foreach(src, czmap_internal_inserter, &idata);

    if (idata.nospace == 0) return CZMAP_COPY_OK;
    else return CZMAP_COPY_NOSPACE;
}


/* internal functions */

static czmap * czmap_internal_find_space(czmap * node, unsigned width, unsigned height)
{
    czmap * found = NULL;
    if (node->left == NULL && node->right == NULL) { /* this node might be it */
        if (width > node->rect.w|| height > node->rect.h)
            return NULL;
        czmap_internal_split(node, width, height);
        return node;
    } else {
        if (node->left != NULL) { /* tries left */
            found = czmap_internal_find_space(node->left, width, height);
            if (found != NULL)
                return found;
        }
        if (node->right != NULL) { /* tries right */
            found = czmap_internal_find_space(node->right, width, height);
            if (found != NULL)
                return found;
        }
    }

    return found;
}

static void czmap_internal_split(czmap * node, unsigned width, unsigned height) {
    unsigned resultw = node->rect.w - width;
    unsigned resulth = node->rect.h - height;
    if (resultw <= resulth) {
        node->left = czmap_internal_alloc(node->rect.x + width, node->rect.y, resultw, height);
        node->right = czmap_internal_alloc(node->rect.x, node->rect.y + height, node->rect.w, resulth);
    } else {
        node->left = czmap_internal_alloc(node->rect.x, node->rect.y + height, width, resulth);
        node->right = czmap_internal_alloc(node->rect.x + width, node->rect.y, resultw, node->rect.h);
    }
    node->rect.w = width;
    node->rect.h = height;
}

static czmap * czmap_internal_alloc(unsigned x, unsigned y, unsigned w, unsigned h) {
    czmap * r = calloc(sizeof(czmap), 1);
    r->rect.x = x;
    r->rect.y = y;
    r->rect.w = w;
    r->rect.h = h;
    return r;
}

static void czmap_internal_free(czmap * map) {
    free(map);
}
