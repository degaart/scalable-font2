/*
 * sfnedit/uiwgt.c
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
 * @brief User interface widgets
 *
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include "libsfn.h"
#include "ui.h"
#include "lang.h"

uint32_t numbers[] = {0x4aaa4,0x4c444,0xc248e,0xc2c2c,0x24ae2,0xe8c2c,0x68eac,0xe2444,0x4a4a4,0xea62c};
uint32_t letters[] = {0x4aeaa,0xcacac,0x68886,0xcaaac,0xe8e8e,0xe8e88,0xaa4aa,0xaa444 };

int input_maxlen = 0, input_callback = 0, input_refresh = 0;
char *input_str = NULL, *input_cur = NULL, *input_scr = NULL, *input_end = NULL, input_buf[256];
extern int seltool;
extern char ksearch[];

/**
 * Draw the toolbox
 */
void ui_toolbox(int idx)
{
    ui_win_t *win = &wins[idx];
    int i, w;

    if(!idx)
        ui_text(win, wins[idx].w - 7*8, 4, verstr);

    for(i = 0; i < (idx ? 3 : 6); i++)
        if(wins[idx].field == i)
            ui_box(win, 1 + i * 24, 1, 22, 22, theme[THEME_CURSOR], theme[THEME_LIGHT], theme[THEME_CURSOR]);
        else if(seltool == i)
            ui_box(win, 1 + i * 24, 1, 22, 22, theme[THEME_DARK], theme[THEME_BG], theme[THEME_LIGHT]);
        else
            ui_box(win, 1 + i * 24, 1, 22, 22, theme[THEME_LIGHT], theme[THEME_BG], theme[THEME_DARK]);

    if(!idx) {
        for(i = 0; i < 6; i++)
            ui_icon(win, 4 + i * 24, 4, ICON_ABOUT + i, wins[idx].tool == -1 ? 1 : 0);
    } else {
        w = ssfn_dst.w; ssfn_dst.w = 144;
        for(i = 0; i < 3; i++)
            ui_icon(win, 4 + i * 24, 4, ICON_MEASURES + i, 0);
        if(wins[idx].unicode >= SSFN_LIG_FIRST && wins[idx].unicode <= SSFN_LIG_LAST) {
            ui_input(win, 6 + 3 * 24, 2, 56, ctx.ligatures[wins[idx].unicode - SSFN_LIG_FIRST], wins[idx].field == 3, 15,
                1024 + wins[idx].unicode - SSFN_LIG_FIRST);
        } else {
            ui_box(win, 6 + 3 * 24, 2, 56, 18, theme[THEME_BG], theme[THEME_BG], theme[THEME_BG]);
            ui_text(win, 8 + 3 * 24, 4, utf8(wins[idx].unicode));
        }
        ssfn_dst.w = w;
    }
}

/**
 * Display a rectangle
 */
void ui_rect(ui_win_t *win, int x, int y, int w, int h, uint32_t l, uint32_t d)
{
    int i, j, p = y * win->p + x, p2 = (y + h - 1) * win->p + x, p3;

    if(x < 0 || y < 0 || x >= ssfn_dst.w || y >= ssfn_dst.h || w < 1 || h < 1) return;
    l &= 0xFFFFFF;
    d &= 0xFFFFFF;
    for(i=0; i + 1 < w && x + i + 1 < ssfn_dst.w; i++) {
        win->data[p + i] = l;
        if(y + h - 1 < ssfn_dst.h) win->data[p2 + i + 1] = d;
    }
    p+=win->p;
    for(j=1, p3=p; j+1 < h && y + j < ssfn_dst.h; j++, p3 += win->p) {
        win->data[p3] = l;
        if(x + w - 1 < ssfn_dst.w) win->data[p3 + w - 1] = d;
    }
}

/**
 * Display a filled box
 */
void ui_box(ui_win_t *win, int x, int y, int w, int h, uint32_t l, uint32_t b, uint32_t d)
{
    int i, j, p = y * win->p + x, p2 = (y + h - 1) * win->p + x, p3;

    if(x < 0 || y < 0 || x >= ssfn_dst.w || y >= ssfn_dst.h || w < 1 || h < 1) return;
    l &= 0xFFFFFF;
    b &= 0xFFFFFF;
    d &= 0xFFFFFF;
    for(i=0; i + 1 < w && x + i + 1 < ssfn_dst.w; i++) {
        win->data[p + i] = l;
        if(y + h - 1 < ssfn_dst.h) win->data[p2 + i + 1] = d;
    }
    win->data[p + i] = l == d ? l : b;
    if(y + h - 1 < ssfn_dst.h) win->data[p2] = l == d ? l : b;
    p+=win->p;
    for(j=1, p3=p; j+1 < h && y + j < ssfn_dst.h; j++, p3 += win->p) {
        win->data[p3] = l;
        if(x + w - 1 < ssfn_dst.w) win->data[p3 + w - 1] = d;
    }
    for(j=1; j + 1 < h && y + j < ssfn_dst.h; j++, p += win->p)
        for(i=1; i + 1 < w && x + i < ssfn_dst.w; i++)
            win->data[p + i] = b;
}

/**
 * Blit an icon on window
 */
void ui_icon(ui_win_t *win, int x, int y, int icon, int inactive)
{
    int i, j, m, k, w, h;
    uint8_t *a, *b, n;

    if(x < 0 || y < 0 || x >= ssfn_dst.w || y >= ssfn_dst.h) return;
    w = win->w > x + 16 ? 16 : ssfn_dst.w - x;
    h = win->h > y + 16 ? 16 : ssfn_dst.h - y;
    for(k = y*win->p + x, m = (icon * 256 + ((16 - h) << 8) + (16 - w)) << 2, j = 0; j < 16 && y + j < ssfn_dst.h;
        j++, k += win->p, m += 16 * 4) {
        for(i = 0; i < w; i++) {
            if(tools[m + (i<<2)+3]) {
                a = (uint8_t*)&win->data[k+i];
                b = (uint8_t*)&tools[m + (i<<2)];
                if(inactive) {
                    n = (b[3]>>(inactive - 1));
                    a[0] = a[1] = a[2] = ((int)(b[0] + b[1] + b[2])*n/6 + (256 - n)*a[2]) >> 8;
                } else {
                    a[2] = (b[0]*b[3] + (256 - b[3])*a[2]) >> 8;
                    a[1] = (b[1]*b[3] + (256 - b[3])*a[1]) >> 8;
                    a[0] = (b[2]*b[3] + (256 - b[3])*a[0]) >> 8;
                }
            }
        }
    }
}

/**
 * Display text
 */
void ui_text(ui_win_t *win, int x, int y, char *str)
{
    char *s = str;
    ssfn_dst.ptr = (uint8_t*)win->data;
    ssfn_dst.p = win->p*4;
    ssfn_dst.x = x;
    ssfn_dst.y = y;
    while(*s && ssfn_dst.x < win->w)
        ssfn_putc(ssfn_utf8(&s));
}

/**
 * Display a text input box
 */
char *ui_input(ui_win_t *win, int x, int y, int w, char *str, int active, int maxlen, int callback)
{
    char *s;
    int i;

    if(x < 0 || y < 0 || x >= ssfn_dst.w || y >= ssfn_dst.h || w < 1) return NULL;
    if(x + w > win->w) w = win->w - x;
    if(w < 0) return NULL;

    ui_box(win, x, y, w+4, 20, theme[active ? THEME_CURSOR : THEME_DARKER], theme[THEME_INPBG],
        theme[active ? THEME_CURSOR : THEME_LIGHT]);
    ssfn_dst.bg = theme[THEME_INPBG];
    ssfn_dst.w = x + w;
    if(active) {
        if(!input_maxlen) {
            if(!input_str) {
                if(callback > 2) {
                    input_str = input_buf;
                    if(str) strcpy(input_buf, str);
                    else input_str[0] = 0;
                } else
                    input_str = str;
                input_cur = input_end = input_str + strlen(input_str);
                input_scr = input_str;
            }
            input_maxlen = maxlen;
            input_callback = callback;
        }
        ssfn_dst.ptr = (uint8_t*)win->data;
        ssfn_dst.p = win->p*4;
        ssfn_dst.x = x + 2;
        ssfn_dst.y = y + 1;
        if(input_cur < input_scr) input_scr = input_cur;
        do {
            for(i = ssfn_dst.x, s = input_scr; *s && s < input_cur && i < ssfn_dst.w; i += ws[ssfn_utf8(&s)]);
            if(i < ssfn_dst.w) break;
            do { input_scr++; } while(input_scr < input_end && (*input_scr & 0xC0) == 0x80);
        } while(1);
        s = input_scr;
        while(ssfn_dst.x < ssfn_dst.w) {
            if(s == input_cur) {
                for(i = 2; i < 16; i++)
                    win->data[(ssfn_dst.y + i) * win->p + ssfn_dst.x - 1] = theme[THEME_CURSOR];
                win->data[(ssfn_dst.y+1) * win->p + ssfn_dst.x - 2] =
                win->data[(ssfn_dst.y+1) * win->p + ssfn_dst.x] =
                win->data[(ssfn_dst.y+i) * win->p + ssfn_dst.x - 2] =
                win->data[(ssfn_dst.y+i) * win->p + ssfn_dst.x] = theme[THEME_CURSOR];
            }
            if(!*s || s == input_end) break;
            ssfn_putc(ssfn_utf8(&s));
        }
    } else if(str && *str) {
        ui_text(win, x + 2, y + 1, str);
    }
    ssfn_dst.w = win->w;
    ssfn_dst.bg = 0;
    return NULL;
}

/**
 * Display a button
 */
void ui_button(ui_win_t *win, int x, int y, int w, char *str, int pressed, int active)
{
    int i, j, p = y * win->p + x, p2 = (y + 20 - 1) * win->p + x, p3;
    uint32_t l, b, B, d;

    if(pressed < 2) {
        l = theme[THEME_BTN0L]; b = theme[THEME_BTN0BL]; B = theme[THEME_BTN0BD]; d = theme[THEME_BTN0D];
    } else {
        l = theme[THEME_BTN1L]; b = theme[THEME_BTN1BL]; B = theme[THEME_BTN1BD]; d = theme[THEME_BTN1D];
    }
    if(x < 0 || y < 0 || x >= ssfn_dst.w || y >= ssfn_dst.h || w < 1) return;
    if(x + w > win->w) w = win->w - x;
    if(w < 0) return;
    if(active == -1) {
        l = d = b = B = theme[pressed < 2 ? THEME_DARK: THEME_BTN1BD];
        ssfn_dst.fg = theme[THEME_LIGHTER]; active = 0;
    }
    for(i=0; i < w && x + i < ssfn_dst.w; i++) {
        win->data[p + i - win->p] = theme[THEME_BTNB];
        win->data[p + i] = active > 0 ? theme[THEME_CURSOR] : (pressed & 1 ? d : l);
        if(y + 20 - 1 < ssfn_dst.h) {
            win->data[p2 + i] = active ? theme[THEME_CURSOR] : (pressed & 1 ? l : d);
            win->data[p2 + i + win->p] = theme[THEME_BTNB];
        }
    }
    p+=win->p;
    for(j=1, p3=p; j+1 < 20 && y + j < ssfn_dst.h; j++, p3 += win->p) {
        win->data[p3 - 1] = theme[THEME_BTNB];
        win->data[p3] = active > 0 ? theme[THEME_CURSOR] : (pressed & 1 ? d : l);
        if(x + w - 1 < ssfn_dst.w) {
            win->data[p3 + w - 1] = active > 0 ? theme[THEME_CURSOR] : (pressed & 1 ? l : d);
            win->data[p3 + w] = theme[THEME_BTNB];
        }
    }
    for(j=1; j + 1 < 20 && y + j < ssfn_dst.h; j++, p += win->p)
        for(i=1; i + 1 < w && x + i < ssfn_dst.w; i++)
            win->data[p + i] = pressed & 1 ? (j < 8 ? B : b) : (j < 12 ? b : B);

    ssfn_dst.bg = 0;
    ssfn_dst.w = x + w;
    x += (w - ui_textwidth(str)) / 2;
    ui_text(win, x, y + 1, str);
    ssfn_dst.w = win->w;
}

/**
 * Display a checkbox
 */
void ui_bool(ui_win_t *win, int x, int y, char *s, int state, int active)
{
    int i;
    ui_box(win, x, y + 4, 12, 12, theme[active ? THEME_CURSOR : THEME_DARKER], theme[THEME_INPBG],
        theme[active ? THEME_CURSOR : THEME_LIGHT]);
    if(state) {
        for(i = 0; i < 6; i++) {
            win->data[(y + 7 + i) * win->p + x + 8 - i] = theme[THEME_FG];
            win->data[(y + 7 + i) * win->p + x + 7 - i] = theme[THEME_FG];
        }
        for(i = 0; i < 3; i++) {
            win->data[(y + 9 + i) * win->p + x + 2] = theme[THEME_FG];
            win->data[(y + 9 + i) * win->p + x + 3] = theme[THEME_FG];
        }
    }
    ui_text(win, x + 16, y, s);
}

/**
 * Display a triangle
 */
void ui_tri(ui_win_t *win, int x, int y, int up)
{
    int i, j = win->p * y + x;
    if(x < 0 || y < 0 || x >= ssfn_dst.w || y >= ssfn_dst.h) return;
    if(up) {
        win->data[j+3] = theme[THEME_DARK];
        j += win->p;
        win->data[j+2] = theme[THEME_DARKER];
        win->data[j+3] = theme[THEME_DARK];
        win->data[j+4] = theme[THEME_LIGHT];
        j += win->p;
        win->data[j+1] = theme[THEME_DARKER];
        win->data[j+2] = theme[THEME_DARK];
        win->data[j+3] = theme[THEME_DARK];
        win->data[j+4] = theme[THEME_DARK];
        win->data[j+5] = theme[THEME_LIGHT];
        j += win->p;
        win->data[j] = theme[THEME_DARKER];
        win->data[j+1] = theme[THEME_DARK];
        win->data[j+2] = theme[THEME_DARK];
        win->data[j+3] = theme[THEME_DARK];
        win->data[j+4] = theme[THEME_DARK];
        win->data[j+5] = theme[THEME_DARK];
        win->data[j+6] = theme[THEME_LIGHT];
        j += win->p;
        for(i = 0; i < 7; i++)
            win->data[j + i] = theme[THEME_LIGHTER];
    } else {
        for(i = 0; i < 7; i++)
            win->data[j + i] = theme[THEME_DARKER];
        j += win->p;
        win->data[j] = theme[THEME_LIGHT];
        win->data[j+1] = theme[THEME_DARK];
        win->data[j+2] = theme[THEME_DARK];
        win->data[j+3] = theme[THEME_DARK];
        win->data[j+4] = theme[THEME_DARK];
        win->data[j+5] = theme[THEME_DARK];
        win->data[j+6] = theme[THEME_LIGHTER];
        j += win->p;
        win->data[j+1] = theme[THEME_LIGHT];
        win->data[j+2] = theme[THEME_DARK];
        win->data[j+3] = theme[THEME_DARK];
        win->data[j+4] = theme[THEME_DARK];
        win->data[j+5] = theme[THEME_LIGHTER];
        j += win->p;
        win->data[j+2] = theme[THEME_LIGHT];
        win->data[j+3] = theme[THEME_DARK];
        win->data[j+4] = theme[THEME_LIGHTER];
        j += win->p;
        win->data[j+3] = theme[THEME_LIGHTER];
    }
}

/**
 * Number box
 */
void ui_num(ui_win_t *win, int x, int y, int num, int active, int sel)
{
    char numstr[16];
    sprintf(numstr, "%4d", num);
    if(x < 0 || y < 0 || x >= ssfn_dst.w || y >= ssfn_dst.h) return;
    ui_box(win, x, y, 52, 20, theme[active ? THEME_CURSOR : THEME_DARKER], theme[THEME_INPBG],
        theme[active ? THEME_CURSOR : THEME_LIGHT]);
    ui_box(win, x + 40, y + 1, 11, 9, theme[!sel ? THEME_DARKER : THEME_LIGHT], theme[THEME_BG],
        theme[!sel ? THEME_LIGHT : THEME_DARKER]);
    ui_tri(win, x + 42, y + 3, 1);
    ui_box(win, x + 40, y + 10, 11, 9, theme[sel == 1 ? THEME_DARKER : THEME_LIGHT], theme[THEME_BG],
        theme[sel == 1 ? THEME_LIGHT : THEME_DARKER]);
    ui_tri(win, x + 42, y + 12, 0);
    ssfn_dst.bg = 0;
    ui_text(win, x + 2, y + 1, numstr);
}

/**
 * Display a number with extra small glyphs
 */
void ui_number(ui_win_t *win, int x, int y, int n, uint32_t c)
{
    int i, j, k, m = 0, d = 0, p = y * win->p + x;

    if(!n) { p += 12; goto zero; }
    if(n < 0) {
        n = -n;
        j = n < 100 ? (n < 10 ? 8 : 4) : 0;
        win->data[p + 2*win->p + j] = win->data[p + 2*win->p + 1 + j] = win->data[p + 2*win->p + 2 + j] = c;
    }
    for(m = 1000; m > 0; m /= 10, p += 4)
        if(n >= m) {
            d = (n/m)%10;
zero:
            for(k = 1<<19, j = 0; j < 5 && y + j < win->h; j++)
                for(i = 0; i < 4; i++, k >>= 1)
                    if((numbers[d] & k) && x + i < win->w) win->data[p + j*win->p +i] = c;
        }
}

/**
 * Display a hex digit with extra small glyphs
 */
void ui_hex(ui_win_t *win, char c)
{
    uint32_t d = 0;
    int i, j, k, p = ssfn_dst.y * win->p + ssfn_dst.x;
    if(c >= '0' && c <= '9') d = numbers[c - '0']; else
    if(c >= 'A' && c <= 'F') d = letters[c - 'A']; else
    if(c >= 'a' && c <= 'f') d = letters[c - 'a']; else
    if(c >= 'X' && c <= 'Y') d = letters[c - 'X' + 6];
    for(k = 1<<19, j = 0; j < 5 && ssfn_dst.y + j < ssfn_dst.h; j++)
        for(i = 0; i < 4; i++, k >>= 1)
            if((d & k) && ssfn_dst.x + i < ssfn_dst.w) win->data[p + j*win->p +i] = ssfn_dst.fg;
    ssfn_dst.x += 5;
}

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

/**
 * Display a box with color
 */
void ui_argb(ui_win_t *win, int x, int y, int w, int h, uint32_t c)
{
    int i, j, k = w < 8 ? (w < 4 ? 2 : 4) : 8, p = y * win->p + x;
    uint8_t *d, *a = (uint8_t*)&theme[THEME_LIGHT], *b = (uint8_t*)&theme[THEME_DARK], *C = (uint8_t*)&c;

    for(j=0; j < h && y + j < ssfn_dst.h; j++, p += win->p)
        for(i=0; i < w && x + i < ssfn_dst.w; i++) {
            d = (j & k) ^ (i & k) ? a : b;
            ((uint8_t*)&win->data[p+i])[0] = (C[0]*C[3] + (256 - C[3])*d[0])>>8;
            ((uint8_t*)&win->data[p+i])[1] = (C[1]*C[3] + (256 - C[3])*d[1])>>8;
            ((uint8_t*)&win->data[p+i])[2] = (C[2]*C[3] + (256 - C[3])*d[2])>>8;
        }
}
