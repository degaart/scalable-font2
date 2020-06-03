/*
 * sfnedit/uiglph.c
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
 * @brief Glyph renderer
 *
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "libsfn.h"
#include "ui.h"
#include "lang.h"

/* add a line to contour */
static void _ui_l(int p, int h, int x, int y)
{
    if(x < 0 || y < 0 || x >= p || y >= h || (
        ((ctx.lx + (1 << (SSFN_PREC-1))) >> SSFN_PREC) == ((x + (1 << (SSFN_PREC-1))) >> SSFN_PREC) &&
        ((ctx.ly + (1 << (SSFN_PREC-1))) >> SSFN_PREC) == ((y + (1 << (SSFN_PREC-1))) >> SSFN_PREC))) return;
    if(ctx.ap <= ctx.np) {
        ctx.ap = ctx.np + 512;
        ctx.p = (uint16_t*)realloc(ctx.p, ctx.ap * sizeof(uint16_t));
        if(!ctx.p) { ctx.ap = ctx.np = 0; return; }
    }
    if(!ctx.np) {
        ctx.p[0] = ctx.mx;
        ctx.p[1] = ctx.my;
        ctx.np += 2;
    }
    ctx.p[ctx.np+0] = x;
    ctx.p[ctx.np+1] = y;
    ctx.np += 2;
    ctx.lx = x; ctx.ly = y;
}

/* add a Bezier curve to contour */
static void _ui_b(int p,int h, int x0,int y0, int x1,int y1, int x2,int y2, int x3,int y3, int l)
{
    int m0x, m0y, m1x, m1y, m2x, m2y, m3x, m3y, m4x, m4y,m5x, m5y;
    if(l<4 && (x0!=x3 || y0!=y3)) {
        m0x = ((x1-x0)/2) + x0;     m0y = ((y1-y0)/2) + y0;
        m1x = ((x2-x1)/2) + x1;     m1y = ((y2-y1)/2) + y1;
        m2x = ((x3-x2)/2) + x2;     m2y = ((y3-y2)/2) + y2;
        m3x = ((m1x-m0x)/2) + m0x;  m3y = ((m1y-m0y)/2) + m0y;
        m4x = ((m2x-m1x)/2) + m1x;  m4y = ((m2y-m1y)/2) + m1y;
        m5x = ((m4x-m3x)/2) + m3x;  m5y = ((m4y-m3y)/2) + m3y;
        _ui_b(p,h, x0,y0, m0x,m0y, m3x,m3y, m5x,m5y, l+1);
        _ui_b(p,h, m5x,m5y, m4x,m4y, m2x,m2y, x3,y3, l+1);
    }
    if(l) _ui_l(p,h, x3, y3);
}

/**
 * Render a glyph's layer or all layers
 */
void ui_glyph(ui_win_t *win, int x, int y, int size, int unicode, int layer)
{
    uint8_t ci = 0, cb = 0, cs;
    uint16_t *r;
    int i, j, k, l, p, m, n, o, w, h, a, A, b, B, nr;
    uint32_t P, O, *Op, *Ol;
    unsigned long int sR, sG, sB, sA;
    int ox, oy, y0, y1, Y0, Y1, x0, x1, X0, X1, X2, xs, ys, yp, pc, af, fB, fG, fR, fA, bB, bG, bR;
    ssfn_glyph_t g;
    sfncont_t *cont;

    if(unicode < 0 || unicode > 0x10FFFF || !ctx.glyphs[unicode].numlayer) return;
    ssfn_dst.ptr = (uint8_t*)win->data;
    ssfn_dst.p = win->p*4;
    ssfn_dst.x = x + size / 2 + 1;
    ssfn_dst.y = y;
    h = (size > ctx.height ? (size + 4) & ~3 : ctx.height);
    w = ctx.glyphs[unicode].width * h / ctx.height;
    p = w + (ci ? h / SSFN_ITALIC_DIV : 0) + cb;
    g.p = p;
    g.h = h;
    g.x = ctx.glyphs[unicode].adv_x;
    g.y = ctx.glyphs[unicode].adv_y;
    g.o = ctx.glyphs[unicode].ovl_x;
    if(p * h >= SSFN_DATA_MAX) {
        fprintf(stderr, "sfnedit: %s: unicode %x w %d h %d p %d size %d\n",lang[ERR_SIZE],unicode,w,h,p,size);
        return;
    }
    memset(&g.data, 0xFF, p * h);
    r = (uint16_t*)realloc(NULL, p * 2 * sizeof(uint16_t));
    for(n = (layer == -1 ? 0 : layer); n < (layer == -1 ? ctx.glyphs[unicode].numlayer : layer + 1); n++) {
        switch(ctx.glyphs[unicode].layers[n].type) {
            case SSFN_FRAG_CONTOUR:
                for(i = 0, ctx.np = 0, cont = (sfncont_t*)ctx.glyphs[unicode].layers[n].data;
                    i < ctx.glyphs[unicode].layers[n].len; i++, cont++) {
                        k = (cont->px << SSFN_PREC) * h / ctx.height; m = (cont->py << SSFN_PREC) * h / ctx.height;
                        switch(cont->type) {
                            case SSFN_CONTOUR_MOVE: ctx.mx = ctx.lx = k; ctx.my = ctx.ly = m; break;
                            case SSFN_CONTOUR_LINE: _ui_l(p << SSFN_PREC, h << SSFN_PREC, k, m); break;
                            case SSFN_CONTOUR_QUAD:
                                a = (cont->c1x << SSFN_PREC) * h / ctx.height;
                                A = (cont->c1y << SSFN_PREC) * h / ctx.height;
                                _ui_b(p << SSFN_PREC,h << SSFN_PREC, ctx.lx,ctx.ly, ((a-ctx.lx)/2)+ctx.lx,
                                    ((A-ctx.ly)/2)+ctx.ly, ((k-a)/2)+a,((A-m)/2)+m, k,m, 0);
                            break;
                            case SSFN_CONTOUR_CUBIC:
                                a = (cont->c1x << SSFN_PREC) * h / ctx.height;
                                A = (cont->c1y << SSFN_PREC) * h / ctx.height;
                                b = (cont->c2x << SSFN_PREC) * h / ctx.height;
                                B = (cont->c2y << SSFN_PREC) * h / ctx.height;
                                _ui_b(p << SSFN_PREC,h << SSFN_PREC, ctx.lx,ctx.ly, a,A, b,B, k,m, 0);
                            break;
                        }
                }
                if(ctx.mx != ctx.lx || ctx.my != ctx.ly) _ui_l(p << SSFN_PREC, h << SSFN_PREC, ctx.mx, ctx.my);
                if(ctx.np > 4) {
                    for(b = A = B = o = 0; b < h; b++, B += p) {
                        a = b << SSFN_PREC;
                        for(nr = 0, i = 0; i < ctx.np - 3; i += 2) {
                            if( (ctx.p[i+1] < a && ctx.p[i+3] >= a) ||
                                (ctx.p[i+3] < a && ctx.p[i+1] >= a)) {
                                    if((ctx.p[i+1] >> SSFN_PREC) == (ctx.p[i+3] >> SSFN_PREC))
                                        x = (((int)ctx.p[i]+(int)ctx.p[i+2])>>1);
                                    else
                                        x = ((int)ctx.p[i]) + ((a - (int)ctx.p[i+1])*
                                            ((int)ctx.p[i+2] - (int)ctx.p[i])/
                                            ((int)ctx.p[i+3] - (int)ctx.p[i+1]));
                                    x >>= SSFN_PREC;
                                    if(ci) x += (h - b) / SSFN_ITALIC_DIV;
                                    if(cb && !o) {
                                        if(g.data[B + x] == 0xFF) { o = -cb; A = cb; }
                                        else { o = cb; A = -cb; }
                                    }
                                    for(k = 0; k < nr && x > r[k]; k++);
                                    for(l = nr; l > k; l--) r[l] = r[l-1];
                                    r[k] = x;
                                    nr++;
                            }
                        }
                        if(nr > 1 && nr & 1) { r[nr - 2] = r[nr - 1]; nr--; }
                        if(nr) {
                            for(i = 0; i < nr - 1; i += 2) {
                                l = r[i] + o; m = r[i + 1] + A;
                                if(l < 0) l = 0;
                                if(m > p) m = p;
                                if(i > 0 && l < r[i - 1] + A) l = r[i - 1] + A;
                                for(; l < m; l++)
                                    g.data[B + l] = g.data[B + l] == 0xFF ? ctx.glyphs[unicode].layers[n].color : 0xFF;
                            }
                        }
                    }
                }
            break;
            case SSFN_FRAG_BITMAP:
            case SSFN_FRAG_PIXMAP:
                B = ctx.glyphs[unicode].width; A = ctx.glyphs[unicode].height;
                b = B * h / ctx.height; a = A * h / ctx.height;
                for(j = 0; j < a; j++) {
                    k = j * A / a * B;
                    for(i = 0; i < b; i++) {
                        l = ctx.glyphs[unicode].layers[n].data[k + i * B / b];
                        if(l != 0xFF)
                            g.data[j * p + i] =
                                ctx.glyphs[unicode].layers[n].type == SSFN_FRAG_BITMAP ? ctx.glyphs[unicode].layers[n].color : l;
                    }
                }
            break;
        }
    }
    free(r);
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
        cb = (h + 64) >> 6;
#if 0
        printf("Scaling to w %d h %d (glyph %d %d, cache %d %d, font %d)\n",
            w,h,ctx.glyphs[unicode].width,ctx.glyphs[unicode].width,g.p,g.h,ctx.height);
#endif
        fR = (ssfn_dst.fg >> 16) & 0xFF; fG = (ssfn_dst.fg >> 8) & 0xFF; fB = (ssfn_dst.fg >> 0) & 0xFF; fA = (ssfn_dst.fg >> 24) & 0xFF;
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
