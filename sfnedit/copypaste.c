/*
 * sfnedit/copypaste.c
 *
 * Copyright (C) 2019 bzt (bztsrc@gitlab)
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
 * @brief Copypaste functions
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libsfn.h"
#ifdef __WIN32__
#include <windows.h>
#endif

/* Don't use zlib, too slow on big sets. Disk cache is much faster. */
/*
#ifdef HAS_ZLIB
#include <zlib.h>
#define CPFILE gzFile
#define CPOPEN gzopen
#define CPCLOSE gzclose
#define CPWRITE(a,b,c) gzwrite(c,a,b)
#define CPREAD(a,b,c) gzread(c,a,b)
#define CPEOF gzeof
#else
*/
#define CPFILE FILE*
#define CPOPEN fopen
#define CPCLOSE fclose
#define CPWRITE(a,b,c) fwrite(a,b,1,c)
#define CPREAD(a,b,c) fread(a,b,1,c)
#define CPEOF feof
/*
#endif
*/

/* clipboard file */
char *copypaste_fn = NULL;

/**
 * Initialize clipboard file.Uses local files as that's the simplest and most portable way to
 * share data between separate processes (sfnedit might run in multiple instances).
 */
int copypaste_init()
{
#ifdef __WIN32__
    char *s;
    wchar_t tmp[MAX_PATH];
    DWORD i, l;
#else
    char *home;
#endif
    if(!copypaste_fn) {
#ifdef __WIN32__
        tmp[0] = 0; l = GetTempPathW(MAX_PATH, tmp);
        if(!tmp[0] || l < 1 || l >= MAX_PATH || !(copypaste_fn = (char*)malloc(l + 16))) return 0;
        for(i = 0, s = copypaste_fn; tmp[i]; i++) {
            if(tmp[i] < 0x80) {
                *s++ = tmp[i];
            } else if(tmp[i] < 0x800) {
                *s++ = ((tmp[i]>>6)&0x1F)|0xC0;
                *s++ = (tmp[i]&0x3F)|0x80;
            } else {
                *s++ = ((tmp[i]>>12)&0x0F)|0xE0;
                *s++ = ((tmp[i]>>6)&0x3F)|0x80;
                *s++ = (tmp[i]&0x3F)|0x80;
            }
        }
        strcpy(s, "\\sfneditcb.dat");
#else
        home = getenv("HOME");
        if(!home) home = "/tmp";
        copypaste_fn = (char*)malloc(strlen(home) + 16);
        if(copypaste_fn) sprintf(copypaste_fn, "%s/.sfneditcb", home);
        else return 0;
#endif
    }
    return 1;
}

/**
 * Clean up resources
 */
void copypaste_fini()
{
    if(copypaste_fn) free(copypaste_fn);
    copypaste_fn = NULL;
}

/**
 * Clear clipboard and start copy session
 */
void copypaste_start(int minunicode)
{
    CPFILE f;

    if(!copypaste_init()) return;
    f = CPOPEN(copypaste_fn, "wb");
    if(f) {
        CPWRITE("CLBD", 4, f);
        CPWRITE(&minunicode, sizeof(int), f);
        CPCLOSE(f);
    }
}

/**
 * Copy a layer or all layers to clipboard (aka serialize to binary blob)
 */
void copypaste_copy(int fromunicode, int layer)
{
    CPFILE f;
    int i, j, s, e;
    uint32_t c;

    if(!copypaste_init() || fromunicode < 0 || fromunicode > 0x10FFFF || layer >= ctx.glyphs[fromunicode].numlayer ||
     (!ctx.glyphs[fromunicode].numlayer && !ctx.glyphs[fromunicode].adv_x && !ctx.glyphs[fromunicode].adv_y)) return;
    f = CPOPEN(copypaste_fn, "ab");
    if(f) {
        if(layer < 0) {
            CPWRITE("C", 1, f);
            CPWRITE(&fromunicode, sizeof(int), f);
            CPWRITE(&ctx.glyphs[fromunicode].width, 1, f);
            CPWRITE(&ctx.glyphs[fromunicode].height, 1, f);
            CPWRITE(&ctx.glyphs[fromunicode].ovl_x, 1, f);
            CPWRITE(&ctx.glyphs[fromunicode].adv_x, 1, f);
            CPWRITE(&ctx.glyphs[fromunicode].adv_y, 1, f);
            CPWRITE(&ctx.glyphs[fromunicode].rtl, 1, f);
            CPWRITE(&ctx.glyphs[fromunicode].hintv, 33, f);
            CPWRITE(&ctx.glyphs[fromunicode].hinth, 33, f);
            CPWRITE(&ctx.glyphs[fromunicode].numkern, sizeof(int), f);
            if(ctx.glyphs[fromunicode].numkern)
                CPWRITE(ctx.glyphs[fromunicode].kern, sizeof(sfnkern_t) * ctx.glyphs[fromunicode].numkern, f);
            s = 0; e = ctx.glyphs[fromunicode].numlayer;
        } else { s = layer; e = layer + 1; }
        for(i = s; i < e; i++) {
            CPWRITE("L", 1, f);
            CPWRITE(&fromunicode, sizeof(int), f);
            CPWRITE(&ctx.glyphs[fromunicode].layers[i].type, 1, f);
            c = ctx.glyphs[fromunicode].layers[i].color == 0xFF ? 0x00FFFFFF : (ctx.glyphs[fromunicode].layers[i].color == 0xFE ?
                0x00FEFEFE : ctx.cpal[ctx.glyphs[fromunicode].layers[i].color]);
            CPWRITE(&c, 4, f);
            CPWRITE(&ctx.glyphs[fromunicode].width, 1, f);
            CPWRITE(&ctx.glyphs[fromunicode].height, 1, f);
            if(ctx.glyphs[fromunicode].layers[i].data)
                switch(ctx.glyphs[fromunicode].layers[i].type) {
                    case SSFN_FRAG_CONTOUR:
                        CPWRITE(&ctx.glyphs[fromunicode].layers[i].len, sizeof(int), f);
                        CPWRITE(ctx.glyphs[fromunicode].layers[i].data, sizeof(sfncont_t) * ctx.glyphs[fromunicode].layers[i].len,
                            f);
                    break;
                    case SSFN_FRAG_BITMAP:
                        CPWRITE(ctx.glyphs[fromunicode].layers[i].data, ctx.glyphs[fromunicode].width *
                            ctx.glyphs[fromunicode].height, f);
                    break;
                    case SSFN_FRAG_PIXMAP:
                        for(j = 0; j < ctx.glyphs[fromunicode].width * ctx.glyphs[fromunicode].height; j++) {
                            c = ctx.glyphs[fromunicode].layers[i].data[j] == 0xFF ? 0x00FFFFFF :
                                (ctx.glyphs[fromunicode].layers[i].data[j] == 0xFE ?
                                0x00FEFEFE : ctx.cpal[ctx.glyphs[fromunicode].layers[i].data[j]]);
                            CPWRITE(&c, 4, f);
                        }
                    break;
            }
        }
        CPCLOSE(f);
    }
}

/**
 * Paste data from clipboard (aka deserialize from binary blob)
 */
void copypaste_paste(int tounicode, int oneunicode)
{
    CPFILE f;
    char tmp;
    unsigned char c[72], *data = NULL;
    int unicode = 0, minunicode = 0, last = -1, i, j, l = 0, t = 0, w = 0, h = 0;
    sfnlayer_t *lyr;

    if(!copypaste_init()) return;
    f = CPOPEN(copypaste_fn, "rb");
    if(f) {
        CPREAD(&c, 4, f);
        CPREAD(&minunicode, sizeof(int), f);
        if(c[0] == 'C' && c[1] == 'L' && c[2] == 'B' && c[3] == 'D')
            while(!CPEOF(f)) {
                tmp = 0; CPREAD(&tmp, 1, f); if(!tmp) break;
                switch(tmp) {
                    case 'C':   /* character chunk */
                        CPREAD(&unicode, sizeof(int), f); if(tounicode) { unicode -= minunicode; unicode += tounicode; }
                        if(oneunicode) {
                            unicode = tounicode;
                            CPREAD(&c, 72, f);
                            CPREAD(&i, sizeof(int), f);
                            for(; i > 0; i--)
                                CPREAD(&c, sizeof(sfnkern_t), f);
                        } else {
                            sfn_chardel(unicode);
                            CPREAD(&ctx.glyphs[unicode].width, 1, f);
                            CPREAD(&ctx.glyphs[unicode].height, 1, f);
                            CPREAD(&ctx.glyphs[unicode].ovl_x, 1, f);
                            CPREAD(&ctx.glyphs[unicode].adv_x, 1, f);
                            CPREAD(&ctx.glyphs[unicode].adv_y, 1, f);
                            CPREAD(&ctx.glyphs[unicode].rtl, 1, f);
                            CPREAD(&ctx.glyphs[unicode].hintv, 33, f);
                            CPREAD(&ctx.glyphs[unicode].hinth, 33, f);
                            CPREAD(&ctx.glyphs[unicode].numkern, sizeof(int), f);
                            if(ctx.glyphs[unicode].numkern) {
                                ctx.glyphs[unicode].kern = (sfnkern_t*)malloc(sizeof(sfnkern_t) * ctx.glyphs[unicode].numkern);
                                if(ctx.glyphs[unicode].kern)
                                    CPREAD(ctx.glyphs[unicode].kern, sizeof(sfnkern_t) * ctx.glyphs[unicode].numkern, f);
                            }
                        }
                    break;
                    case 'L':   /* glyph layer chunk */
                        CPREAD(&unicode, sizeof(int), f);  if(tounicode) { unicode -= minunicode; unicode += tounicode; }
                        if(last == -1) last = unicode;
                        else if(oneunicode && last != unicode) { CPCLOSE(f); return; }
                        if(oneunicode) unicode = tounicode;
                        CPREAD(&t, 1, f);
                        CPREAD(&c, 4, f); i = !c[3] && c[0] == 0xFE ? 0xFE : (!c[3] && c[0] == 0xFF ? 0xFF :
                            sfn_cpaladd(c[2], c[1], c[0], c[3]));
                        w = 0; CPREAD(&w, 1, f);
                        h = 0; CPREAD(&h, 1, f);
                        switch(t) {
                            case SSFN_FRAG_CONTOUR:
                                CPREAD(&l, sizeof(int), f);
                                if(l > 0 && (lyr = sfn_layeradd(unicode, t, 0, 0, w, h, i, NULL)) != NULL) {
                                    lyr->data = realloc(lyr->data, l * sizeof(sfncont_t));
                                    if(lyr->data) { lyr->len = l; CPREAD(lyr->data, l * sizeof(sfncont_t), f); }
                                }
                            break;
                            case SSFN_FRAG_BITMAP:
                                data = realloc(data, w * h);
                                if(data) {
                                    CPREAD(data, w * h, f);
                                    sfn_layeradd(unicode, t, 0, 0, w, h, i, data);
                                }
                            break;
                            case SSFN_FRAG_PIXMAP:
                                data = realloc(data, w * h);
                                if(data) {
                                    for(j = 0; j < w * h; j++) {
                                        CPREAD(&c, 4, f);
                                        data[j] = !c[3] && c[0] == 0xFE ? 0xFE : (!c[3] && c[0] == 0xFF ? 0xFF :
                                            sfn_cpaladd(c[2], c[1], c[0], c[3]));
                                    }
                                    sfn_layeradd(unicode, t, 0, 0, w, h, i, data);
                                }
                            break;
                        }
                        if(ctx.glyphs[unicode].width > ctx.width) ctx.width = ctx.glyphs[unicode].width;
                        if(ctx.glyphs[unicode].height > ctx.height) ctx.height = ctx.glyphs[unicode].height;
                        if(ctx.glyphs[unicode].ovl_x > 63) ctx.glyphs[unicode].ovl_x = 63;
                        if(ctx.glyphs[unicode].adv_x) ctx.glyphs[unicode].adv_y = 0;
                    break;
                    case 'P':   /* contour path chunk */
                    break;
                }
            }
        CPCLOSE(f);
    }
}

