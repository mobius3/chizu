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

#ifndef CZMAP_H
#define CZMAP_H

#include "czrect.h"

struct czmap;
typedef struct czmap czmap;

typedef void (*czwalkfunc)(czrect rect, void * data, void * priv);
typedef void (*czdestroyfunc)(void * data);

typedef enum czmap_copy_status {
    CZMAP_COPY_OK,
    CZMAP_COPY_NOSPACE
} czmap_copy_status;

czmap * czmap_create(unsigned width, unsigned height);
void czmap_destroy(czmap * map, czdestroyfunc func);
czrect czmap_lease(czmap * map, unsigned width, unsigned height, void * data);
void czmap_foreach(czmap * map, czwalkfunc func, void * priv);
czmap_copy_status czmap_copy(czmap * src, czmap * dst);

#endif
