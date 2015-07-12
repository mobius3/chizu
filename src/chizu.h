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


#ifndef CHIZU_H
#define CHIZU_H

#if defined(_WIN32)
#   if defined(CHIZU_EXPORTS)
#       define  CHIZU_API  __declspec(dllexport)
#   else
#       define  CHIZU_API  __declspec(dllimport)
#   endif
#else
#   define CHIZU_API __attribute__ ((visibility("default")))
#endif

struct chizu;
typedef struct chizu chizu;

/**
 * Status of the subimage insertion.
 * @sa chizu_insert
 */
typedef enum chizu_insert_status {
    CHIZU_INSERT_OK = 0,
    CHIZU_INSERT_FILEOPEN_FAIL,
    CHIZU_INSERT_NOSPACE,
    CHIZU_INSERT_FAIL
} chizu_insert_status;

/**
 * Status of the atlas export
 * @sa chizu_export
 */
typedef enum chizu_export_status {
    CHIZU_EXPORT_OK,
    CHIZU_EXPORT_SPEC_FAIL,
    CHIZU_EXPORT_TEXTURE_FAIL,
    CHIZU_EXPORT_SPEC_AND_TEXTURE_FAIL,
    CHIZU_EXPORT_FAIL
} chizu_export_status;

/**
 * Texture formats supported when exporting.
 * @sa chizu_export
 */
typedef enum chizu_export_format {
    CHIZU_FORMAT_BMP,
    CHIZU_FORMAT_PNG,
    CHIZU_FORMAT_TGA,
    CHIZU_FORMAT_HDR
} chizu_export_format;

/**
 * Status of Chizu initialization routines.
 * @sa chizu_init
 */
typedef enum chizu_init_status {
    CHIZU_INIT_OK,
    CHIZU_INIT_FAIL
} chizu_init_status;

/**
 * @brief Data record passed from chizu to custom exporting functions.
 */
typedef struct czexport {
    const char * subfile; /** The file name that should go in this position */
    unsigned x, y, w, h;  /** The position of the subimage in the target */
} czexport;


/**
 * @brief Type of the custom export function.
 * @param node The node being read.
 * @param priv The private data you asked to pass.
 */
typedef chizu_export_status (*chizu_custom_export_func)(const czexport * node, void * priv);

/**
 * @brief Type of the pixel data querying function.
 * @param pixels The pixels of the target image.
 * @param width The width of the target image.
 * @param height The height of the target image.
 * @param depth How many bytes per pixel.
 * @param priv The custom private pointer.
 */
typedef void (*chizu_receive_pixel_data_func)(const void * pixels, unsigned width, unsigned height, unsigned depth, void * priv);

/**
 * @brief czinit Initializes image loading features of Chizu
 * @return CHIZU_INIT_OK if propertly initialized or CHIZU_INIT_FAIL if failed.
 * @details This must be called before any other chizu_* function
 */
CHIZU_API int chizu_init();

/**
 * @brief chizu_quit Quits image loading features of Chizu
 * @details Must be called after you finish using Chizu
 */
CHIZU_API void chizu_quit();

/**
 * @brief chizu_create Creates a new textyre atlas of width/height.
 * @return An chizu * atlas instance.
 */
CHIZU_API chizu * chizu_create();

/**
 * @brief chizu_insert Inserts a new subimage in the atlas.
 * @param atlas The atlas instance to put the image into.
 * @param file The path of the file to load
 * @return CHIZU_INSERT_OK if the subimage was added.
 * @return CHIZU_INSERT_FILEOPEN_FAIL if image could not be added.
 * @return CHIZU_INSERT_NOSPACE if there is no enough space to add.
 * @return CHIZU_INSERT_FAIL if an unknown error happened.
 */
CHIZU_API chizu_insert_status chizu_insert(chizu * atlas, const char * file);

/**
 * @brief chizu_export Exports the resulting atlas to a spec and texture file.
 * @param atlas The atlas to export.
 * @param spec The spec file to export to, will contain file path rect information.
 * @param texture The texture file to export to.
 * @param format The format of the texture file.
 * @return CHIZU_EXPORT_OK if both the spec and texture were exported.
 * @return CHIZU_EXPORT_SPEC_FAIL if exporting the spec file failed.
 * @return CHIZU_EXPORT_TEXTURE_FAIL if exporting the texture failed.
 * @return CHIZU_EXPORT_SPEC_AND_TEXTURE_FAIL if both texture and spec failed.
 * @return CHIZU_EXPORT_FAIL for unknown errors.
 * @sa chizu_export_format
 */
CHIZU_API chizu_export_status chizu_export(chizu * atlas, const char * spec, const char * texture, chizu_export_format format);

/**
 * @brief chizu_custom_export Calls your custom function on each subrect.
 * @param atlas The atlas that will be exported
 * @param f The function to be called for every atlas subrect. It must accept a czexport structure and a priv pointer.
 * @param priv The private data pointer that will be passed to your custom export function.
 * @return CHIZU_EXPORT_OK if f always returns CHIZU_EXPORT_OK or CHIZU_EXPORT_FAIL if f returns anything else.
 */
CHIZU_API chizu_export_status chizu_custom_export(chizu * atlas, chizu_custom_export_func f, void * priv);

/**
 * @brief chizu_pixel_data Queries the current pixel data of this atlas.
 * @param atlas The atlas to query the pixel data.
 * @param f The function that will receive the pixel data.
 * @param priv Custom private pointer to be passed back to f.
 * @details This function is useful in case of custom exporting.
 * The containing pixel data is always RGBA (or ABGR on low-endian).
 *
 * Thus, the size of this buffer is always (width * height * 4) bytes, if you
 * want to copy it.
 *
 * Do not store the pixels pointer passed to you as they may be invalid after
 * your function returns.
 */
CHIZU_API void chizu_pixel_data(chizu * atlas, chizu_receive_pixel_data_func f, void * priv);

/**
 * @brief chizu_destroy Destroys and frees the memory used by a chizu atlas instance
 * @param atlas The atlas to destroy.
 * @details The atlas becomes invalid after this, together with any pointer
 * returned. Using it will result in doomsday.
 */
CHIZU_API void chizu_destroy(chizu * atlas);

#endif
