/*
 * sfntest8.c
 *
 * Copyright (C) 2022 bzt (bztsrc@gitlab)
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * @brief rendering Scalable Screen Font off-screen into a buffer and save it to an image file
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#define SSFN_IMPLEMENTATION
#include "ssfn.h"

/**
 * Load a font
 */
ssfn_font_t *load_file(char *filename, int *size)
{
    char *fontdata = NULL;
    FILE *f;

    f = fopen(filename, "rb");
    if(!f) { fprintf(stderr,"unable to load %s\n", filename); exit(3); }
    *size = 0;
    fseek(f, 0, SEEK_END);
    *size = (int)ftell(f);
    fseek(f, 0, SEEK_SET);
    if(!*size) { fprintf(stderr,"unable to load %s\n", filename); exit(3); }
    fontdata = (char*)malloc(*size);
    if(!fontdata) { fprintf(stderr,"memory allocation error\n"); exit(2); }
    fread(fontdata, *size, 1, f);
    fclose(f);
    return (ssfn_font_t*)fontdata;
}

/**
 * Save a buffer into an image file
 */
void dump_buffer(ssfn_buf_t *buffer)
{
    unsigned char tmp[18] = { 0 };
    FILE *f;

    f = fopen("buffer.tga", "wb");
    /* write the header */
    tmp[2] = 2;
    *((uint16_t*)(tmp + 10)) = *((uint16_t*)(tmp + 12)) = buffer->w;
    *((uint16_t*)(tmp + 14)) = buffer->h;
    tmp[16] = 4 * 8;
    tmp[17] = 40;
    fwrite(tmp, 18, 1, f);
    /* write the data */
    fwrite(buffer->ptr, buffer->w * 4 * buffer->h, 1, f);
    fclose(f);
}

/**
 * testing the SSFN library (normal renderer)
 */
void do_test(char *fontfn)
{
    ssfn_t ctx;
    ssfn_font_t *font;
    ssfn_buf_t *buf;
    int ret, size;

    /* initialize the normal renderer */
    memset(&ctx, 0, sizeof(ssfn_t));

    /* load and select a font */
    font = load_file(fontfn ? fontfn : "../fonts/FreeSerif.sfn", &size);
    ret = ssfn_load(&ctx, font);
    if(ret != SSFN_OK) { fprintf(stderr, "ssfn load error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }
    ret = ssfn_select(&ctx, SSFN_FAMILY_ANY, NULL, SSFN_STYLE_REGULAR, 16);
    if(ret != SSFN_OK) { fprintf(stderr, "ssfn select error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }

    /* generate a buffer and save it to file */
    buf = ssfn_text(&ctx, "Lorem ipsum dolor sic amet", 0xffffffff);
    if(!buf || !buf->ptr) { fprintf(stderr, "ssfn text: no buffer returned\n"); exit(2); }
    printf("Buffer allocated: width %d height %d left %d top %d\n", buf->w, buf->h, buf->x, buf->y);
    dump_buffer(buf);

    free(buf->ptr);
    free(buf);
    ssfn_free(&ctx);
    free(font);
}

/**
 * Main procedure
 */
int main(int argc __attribute__((unused)), char **argv)
{
    do_test(argv[1]);
    return 0;
}
