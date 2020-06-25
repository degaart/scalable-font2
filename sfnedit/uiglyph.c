/*
 * sfnedit/uiglyph.c
 *
 * Copyright (C) 2020 bzt (bztsrc@gitlab)
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
 * @brief Glyph renderer for the editor window
 *
 */

#include <stdio.h>
#include "libsfn.h"
#include "ui.h"
#include "lang.h"

/**
 * Render a glyph's layer or all layers
 */
void ui_glyph(ui_win_t *win, int x, int y, int size, int unicode, int layer)
{
    uint8_t cs;
    int j, k, m, n, w, h;
    uint32_t P, O, *Op, *Ol;
    unsigned long int sR, sG, sB, sA;
    int ox, oy, y0, y1, Y0, Y1, x0, x1, X0, X1, X2, xs, ys, yp, pc, af, fB, fG, fR, fA, bB, bG, bR;
    sfngc_t g;

    g.p = g.h = 0;
    if(!sfn_glyph(size, unicode, layer, 1, &g)) {
        if(g.p * g.h >= (260 + 260 / SSFN_ITALIC_DIV) << 8)
            fprintf(stderr, "sfnedit: %s U+%06X: p %d h %d size %d\n",lang[ERR_SIZE],unicode,g.p,g.h,size);
        return;
    }
    ssfn_dst.ptr = (uint8_t*)win->data;
    ssfn_dst.p = win->p*4;
    ssfn_dst.x = x + size / 2 + 1;
    ssfn_dst.y = y;
    h = size;
    w = g.p * h / g.h;
    if(w > size - 4) { h = h * (size - 4) / w; w = size - 4; ssfn_dst.y += (size - h) / 2; }
    n = size > 16 ? 2 : 1;
    if(w < n) w = n;
    if(ssfn_dst.ptr) {
        ox = w / 2;
        if(ox > size / 2) ox = size / 2;
        oy = 0;
        j = ssfn_dst.w < 0 ? -ssfn_dst.w : ssfn_dst.w;
        cs = ssfn_dst.w < 0 ? 16 : 0;
#if 0
        printf("Scaling to w %d h %d (glyph %d %d, cache %d %d, font %d)\n",
            w,h,ctx.glyphs[unicode].width,ctx.glyphs[unicode].width,g.p,g.h,ctx.height);
#endif
        fR = (ssfn_dst.fg >> 16) & 0xFF; fG = (ssfn_dst.fg >> 8) & 0xFF; fB = (ssfn_dst.fg >> 0) & 0xFF;
        fA = (ssfn_dst.fg >> 24) & 0xFF;
        Op = (uint32_t*)(ssfn_dst.ptr + ssfn_dst.p * (ssfn_dst.y - oy) + ((ssfn_dst.x - ox) << 2));
        for (y = 0; y < h && ssfn_dst.y + y - oy < ssfn_dst.h; y++, Op += ssfn_dst.p >> 2) {
            if(ssfn_dst.y + y - oy < 0) continue;
            y0 = (y << 8) * g.h / h; Y0 = y0 >> 8; y1 = ((y + 1) << 8) * g.h / h; Y1 = y1 >> 8; Ol = Op;
            for (x = 0; x < w && ssfn_dst.x + x - ox < j; x++, Ol++) {
                if(ssfn_dst.x + x - ox < 0) continue;
                m = 0; sR = sG = sB = sA = 0;
                /* real linear frame buffers should be accessed only as uint32_t on 32 bit boundary */
                O = *Ol;
                bR = (O >> (16 - cs)) & 0xFF;
                bG = (O >> 8) & 0xFF;
                bB = (O >> cs) & 0xFF;
                x0 = (x << 8) * g.p / w; X0 = x0 >> 8; x1 = ((x + 1) << 8) * g.p / w; X1 = x1 >> 8;
                for(ys = y0; ys < y1; ys += 256) {
                    if(ys >> 8 == Y0) { yp = 256 - (ys & 0xFF); ys &= ~0xFF; if(yp > y1 - y0) yp = y1 - y0; }
                    else if(ys >> 8 == Y1) yp = y1 & 0xFF;
                    else yp = 256;
                    X2 = (ys >> 8) * g.p;
                    for(xs = x0; xs < x1; xs += 256) {
                        if (xs >> 8 == X0) {
                            k = 256 - (xs & 0xFF); xs &= ~0xFF; if(k > x1 - x0) k = x1 - x0;
                            pc = k == 256 ? yp : (k * yp) >> 8;
                        } else
                        if (xs >> 8 == X1) { k = x1 & 0xFF; pc = k == 256 ? yp : (k * yp) >> 8; }
                        else pc = yp;
                        m += pc;
                        k = g.data[X2 + (xs >> 8)];
                        P = *((uint32_t*)(ctx.cpal + (k << 2)));
                        if(k == 0xFF) {
                            sB += bB * pc; sG += bG * pc; sR += bR * pc;
                        } else
                        if(k == 0xFE || !P) {
                            af = (256 - fA) * pc;
                            sB += fB * af; sG += fG * af; sR += fR * af; sA += fA * pc;
                        } else {
                            af = (256 - (P >> 24)) * pc;
                            sR += (((P >> 16) & 0xFF) * af);
                            sG += (((P >> 8) & 0xFF) * af);
                            sB += (((P >> 0) & 0xFF) * af);
                            sA += (((P >> 24) & 0xFF) * pc);
                        }
                    }
                }
                if(m) { sR /= m; sG /= m; sB /= m; sA /= m; }
                else { sR >>= 8; sG >>= 8; sB >>= 8; sA >>= 8; }
                if(sA > 15) {
                    *Ol = ((sA > 255 ? 255 : sA) << 24) | ((sR > 255 ? 255 : sR) << (16 - cs)) |
                        ((sG > 255 ? 255 : sG) << 8) | ((sB > 255 ? 255 : sB) << cs);
                }
            }
        }
    }
    return;
}

/**
 * This is a special renderer that must not use anti-aliasing
 * and uses crop to render the glyph just partially
 */
void ui_edit(ui_win_t *win, int x, int y, int w, int h, int layer, int p, uint32_t *d)
{
    int i, j, k, first = 1;
    if(win->unicode > 0x10FFFF) return;
}
