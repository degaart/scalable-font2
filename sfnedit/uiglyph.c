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
#include <string.h>
#include "libsfn.h"
#include "ui.h"
#include "lang.h"

/**
 * Render a glyph's layer or all layers
 */
void ui_glyph(ui_win_t *win, int x, int y, int size, uint32_t unicode, int layer)
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
 * Draw a dot or X
 */
void ui_dot(ui_win_t *win, int x, int y, int t)
{
    uint32_t c = theme[t ? THEME_CPOINT : THEME_POINT];
    int i, j, n, z = win->zoom & ~1;
    x >>= 8; y >>= 8;
    if(z < 3) z = 3;
    y -= z/2; x -= z/2;
    for(i = 0, n = y * win->p + x; i <= z; i++, n += win->p) {
        if(y + i >= 36 && y + i < ssfn_dst.h) {
            if(!t) {
                for(j = 0; j < z; j++)
                    if(x + j >= 20 && x + j < ssfn_dst.w) win->data[n + j] = c;
            } else {
                if(x + i >= 20 && x + i < ssfn_dst.w) win->data[n + i] = c;
                if(x + z - i >= 20 && x + z - i < ssfn_dst.w) win->data[n + z - i] = c;
            }
        }
    }
}

/**
 * Draw line
 */
void ui_line(ui_win_t *win, int x0, int y0, int x1, int y1, int dot, uint32_t c)
{
    int sx, sy, dx, dy, x, y, m;
    x0 >>= 8; y0 >>= 8; x1 >>= 8; y1 >>= 8;
    if((x0 < 20 && x1 < 20) || (x0 >= ssfn_dst.w && x1 >= ssfn_dst.w) ||
        (y0 < 36 && y1 < 36) || (y0 >= ssfn_dst.h && y1 >= ssfn_dst.h) || (x0 == x1 && y0 == y1)) return;
    sx = x1 >= x0? 1 : -1; sy = y1 >= y0? 1 : -1; dx = x1 - x0; dy = y1 - y0;
    if(sx * dx >= sy * dy && dx) {
        for(x = x0, m = sx * dx; m > 0; m -= dot, x += sx*dot) {
            y = y0 + ((x - x0) * dy / dx);
            if(y >= 36 && y < ssfn_dst.h && x >= 20 && x < ssfn_dst.w) win->data[y * win->p + x] = c;
        }
    } else {
        for(y = y0, m = sy * dy; m > 0; m -= dot, y += sy*dot) {
            x = x0 + ((y - y0) * dx / dy);
            if(x >= 20 && x < ssfn_dst.w && y >= 36 && y < ssfn_dst.h) win->data[y * win->p + x] = c;
        }
    }
}

/**
 * Draw Bezier curve
 */
void ui_bezier(ui_win_t *win, int x0,int y0, int x1,int y1, int x2,int y2, int x3,int y3, uint32_t c, int l)
{
    int m0x, m0y, m1x, m1y, m2x, m2y, m3x, m3y, m4x, m4y,m5x, m5y;
    if(l<8 && (x0!=x3 || y0!=y3)) {
        m0x = ((x1-x0)/2) + x0;     m0y = ((y1-y0)/2) + y0;
        m1x = ((x2-x1)/2) + x1;     m1y = ((y2-y1)/2) + y1;
        m2x = ((x3-x2)/2) + x2;     m2y = ((y3-y2)/2) + y2;
        m3x = ((m1x-m0x)/2) + m0x;  m3y = ((m1y-m0y)/2) + m0y;
        m4x = ((m2x-m1x)/2) + m1x;  m4y = ((m2y-m1y)/2) + m1y;
        m5x = ((m4x-m3x)/2) + m3x;  m5y = ((m4y-m3y)/2) + m3y;
        ui_bezier(win, x0,y0, m0x,m0y, m3x,m3y, m5x,m5y, c, l+1);
        ui_bezier(win, m5x,m5y, m4x,m4y, m2x,m2y, x3,y3, c, l+1);
    } else
    if(l) ui_line(win, x0,y0, x3,y3, 1, c);
}

/**
 * Render one layer
 */
void ui_layer(ui_win_t *win, int x, int y, uint32_t unicode, int layer, int hl)
{
    int i, j, k, l, m, n, p, lx, ly, mx, my, ox, oy, sx, sy, w = ctx.glyphs[unicode].width, h = ctx.glyphs[unicode].height;
    int a,b,A,B,C,D;
    uint32_t c = 0, f, f0, *uc = (uint32_t*)&ctx.cpal;
    uint8_t *bc = (uint8_t*)&f, *bc0 = (uint8_t*)&f0;
    sfnlayer_t *lyr = &ctx.glyphs[unicode].layers[layer];
    sfncont_t *cont;

    if(layer < 0 || layer >= ctx.glyphs[unicode].numlayer) return;
    cont = (sfncont_t*)ctx.glyphs[unicode].layers[layer].data;
    if(lyr->color >= 0xFE) f = theme[THEME_FG];
    else f = uc[lyr->color];
    if(!hl) f = (f & 0xFF000000) | ((f >> 1) & 0x7F7F7F);
    switch(lyr->type) {
        case SSFN_FRAG_CONTOUR:
            mx = my = lx = ly = -1; f0 = f;
            for(j = 0; j < lyr->len; j++, cont++) {
                if(hl == 2) { a = ((j+1) * 64 / lyr->len) - 64; bc[0] = bc0[0] + a; bc[1] = bc0[1] + a; bc[2] = bc0[2] + a; }
                a = (x + cont->px * win->zoom) << 8; b = (y + cont->py * win->zoom) << 8;
                switch(cont->type) {
                    case SSFN_CONTOUR_MOVE: mx = a; my = b; break;
                    case SSFN_CONTOUR_LINE: ui_line(win, lx, ly, a, b, 1, f); break;
                    case SSFN_CONTOUR_QUAD:
                        A = (x + cont->c1x * win->zoom) << 8; B = (y + cont->c1y * win->zoom) << 8;
                        if(hl == 2) {
                            ui_line(win, lx, ly, A, B, 5, theme[THEME_CURVE]);
                            ui_line(win, A, B, a, b, 5, theme[THEME_CURVE]);
                        }
                        ui_bezier(win, lx, ly, (A - lx)/2+lx, (B - ly)/2+ly, (a - A)/2 + A, (b - B)/2 + B, a, b, f, 0);
                    break;
                    case SSFN_CONTOUR_CUBIC:
                        A = (x + cont->c1x * win->zoom) << 8; B = (y + cont->c1y * win->zoom) << 8;
                        C = (x + cont->c2x * win->zoom) << 8; D = (y + cont->c2y * win->zoom) << 8;
                        if(hl == 2) {
                            ui_line(win, lx, ly, A, B, 5, theme[THEME_CURVE]);
                            ui_line(win, A, B, C, D, 5, theme[THEME_CURVE]);
                            ui_line(win, C, D, a, b, 5, theme[THEME_CURVE]);
                        }
                        ui_bezier(win, lx, ly, A, B, C, D, a, b, f, 0);
                    break;
                }
                lx = a; ly = b;
            }
            if(hl != 2 && ((lx > 0 && mx > 0 && lx != mx) || (ly > 0 && mx > 0 && ly != my)))
                ui_line(win, lx, ly, mx, my, 1, f);
            if(hl == 2) {
                cont = (sfncont_t*)ctx.glyphs[unicode].layers[layer].data;
                for(j = 0; j < lyr->len; j++, cont++) {
                    a = (x + cont->px * win->zoom) << 8; b = (y + cont->py * win->zoom) << 8;
                    switch(cont->type) {
                        case SSFN_CONTOUR_MOVE: mx = a; my = b; break;
                        case SSFN_CONTOUR_QUAD:
                            A = (x + cont->c1x * win->zoom) << 8; B = (y + cont->c1y * win->zoom) << 8;
                            ui_dot(win, A,B, 1);
                        break;
                        case SSFN_CONTOUR_CUBIC:
                            A = (x + cont->c1x * win->zoom) << 8; B = (y + cont->c1y * win->zoom) << 8;
                            C = (x + cont->c2x * win->zoom) << 8; D = (y + cont->c2y * win->zoom) << 8;
                            ui_dot(win, A,B, 1);
                            ui_dot(win, C,D, 1);
                        break;
                    }
                    ui_dot(win, a, b, 0);
                    lx = a; ly = b;
                }
            }
        break;
        case SSFN_FRAG_BITMAP:
        case SSFN_FRAG_PIXMAP:
            for(j = 0; j < h && y < ssfn_dst.h; j++, y += win->zoom) {
                if(y + win->zoom >= 36) {
                    if(y < 36) { oy = 36-y; sy = y + win->zoom - 36; } else { oy = 0; sy = win->zoom; }
                    for(i = 0, k = x; i < w && k < ssfn_dst.w; i++, k += win->zoom)
                        if(k + win->zoom >= 20) {
                            if(k < 20) { ox = 20-k; sx = k + win->zoom - 20; } else { ox = 0; sx = win->zoom; }
                            l = lyr->data[j * w + i];
                            if(l < 0xFF) {
                                if(l == 0xFE) c = f;
                                else {
                                    c = uc[l];
                                    if(!hl) c = (c & 0xFF000000) | ((c >> 1) & 0x7F7F7F);
                                }
                                p = (y + oy) * win->p + (k + ox);
                                for(m=0; m < sy && y + oy + m < ssfn_dst.h; m++, p += win->p)
                                    for(n=0; n < sx && k + ox + n < ssfn_dst.w; n++)
                                        win->data[p + n] = c;
                            }
                        }
                }
            }
        break;
    }
}

/**
 * This is a special renderer that must not use anti-aliasing
 * and uses crop to render the glyph just partially
 */
void ui_edit(ui_win_t *win, int x, int y, uint32_t unicode, int layer)
{
    int i;
    if(unicode > 0x10FFFF) return;
    for(i = 0; i < ctx.glyphs[unicode].numlayer; i++)
        if(i != layer)
            ui_layer(win, x, y, unicode, i, layer == -1 ? 1 : 0);
    if(layer != -1 && layer < ctx.glyphs[unicode].numlayer)
        ui_layer(win, x, y, unicode, layer, 2);
}
