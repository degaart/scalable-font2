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
            ui_input(win, 6 + 3 * 24, 2, 54, ctx.ligatures[wins[idx].unicode - SSFN_LIG_FIRST], wins[idx].field == 3, 15,
                1024 + wins[idx].unicode - SSFN_LIG_FIRST);
        } else {
            ui_box(win, 6 + 3 * 24, 2, 54, 18, theme[THEME_BG], theme[THEME_BG], theme[THEME_BG]);
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
    win->data[p + i] = l == d ? l : theme[THEME_BG];
    if(y + h - 1 < ssfn_dst.h) win->data[p2] = l == d ? l : theme[THEME_BG];
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
 * Display grid coordinates
 */
void ui_grid(ui_win_t *win, int w, int h)
{
    int sw, sh, p, i, j, k, tw = ssfn_dst.w - 20, th = ssfn_dst.h - 36, ox, oy, px, py;
    uint32_t g = theme[THEME_DIM] & 0xFFFFFF;
    if(win->zoom < 1) {
        i = (tw > th) ? th : tw;
        k = (w > h) ? w : h;
        i = k > 0 ? i / k : 1;
        win->zoom = i > 1 ? i : 1;
    }
    if(win->zoom > 64) win->zoom = 64;
    sw = w * win->zoom;
    sh = h * win->zoom;
    if(win->rc) {
        win->zx = (tw - sw) / 2;
        win->zy = (th - sh) / 2;
    }
    win->rc = 0;
    ox = (win->zx > 0 ? win->zx : 0);
    oy = (win->zy > 0 ? win->zy : 0);
    px = (win->zx < 0 ? -win->zx : 0);
    py = (win->zy < 0 ? -win->zy : 0);
    j = win->zoom * 10; if(j > sw || j > sh) j = win->zoom * 5;
    ui_box(win,20,27,tw,9,theme[THEME_BG],theme[THEME_BG],theme[THEME_BG]);
    if(sw > 0) {
        for(p = 35 * win->p + ox + 20, i = 0; i + px < sw && ox + i < tw; i++) {
            if(!((i + px) % j))
                win->data[p - win->p - win->p + i] = win->data[p - win->p + i] = g;
            win->data[p + i] = g;
        }
    }
    ui_box(win,0,36,20,th,theme[THEME_BG],theme[THEME_BG],theme[THEME_BG]);
    if(sh > 0) {
        for(p = (36 + oy) * win->p + 19, i = 0; i + py < sh && oy + i < th; i++, p += win->p) {
            if(!((i + py) % j)) win->data[p - 2] = win->data[p - 1] = g;
            win->data[p] = g;
        }
    }
    if(ox < ssfn_dst.w) ui_number(win, ox + 7, 27, px / win->zoom, g);
    if(win->tool == GLYPH_TOOL_COORD) {
        i = ox + 7 + ctx.glyphs[win->unicode].ovl_x * win->zoom - px; if(i > tw + 7) i = tw + 7; if(i < 0) i = 0;
        if(ctx.glyphs[win->unicode].ovl_x && i < ssfn_dst.w) ui_number(win, i, 27, ctx.glyphs[win->unicode].ovl_x,
        theme[THEME_OVL]);
    }
    if(win->tool != GLYPH_TOOL_LAYER) {
        i = ox + 7 + (ctx.glyphs[win->unicode].rtl ? ctx.glyphs[win->unicode].width - ctx.glyphs[win->unicode].adv_x :
            ctx.glyphs[win->unicode].adv_x) * win->zoom - px; if(i > tw + 7) i = tw + 7; if(i < 0) i = 0;
        if(!ctx.glyphs[win->unicode].adv_y && i < ssfn_dst.w) ui_number(win, i, 27, ctx.glyphs[win->unicode].adv_x,
            theme[THEME_ADV]);
    }
    i = ox + 7 + sw - px; if(i > tw + 7) i = tw + 7;
    if(i >= 20 && i < ssfn_dst.w) ui_number(win, i, 27, (px + i - ox - 7) / win->zoom, g);
    ssfn_dst.h += 4;
    i = oy + 33 - py + ctx.baseline * win->zoom; if(i > th + 33) i = th + 33;
    if(i >= 33 && i < ssfn_dst.h - 6) ui_number(win, 0, i, ctx.baseline, theme[THEME_BASE]);
    if(win->tool == GLYPH_TOOL_COORD) {
        i = oy + 33 - py + ctx.underline * win->zoom; if(i > th + 33) i = th + 33;
        if(i >= 33 && i < ssfn_dst.h - 6) ui_number(win, 0, i, ctx.underline, theme[THEME_UNDER]);
    }
    if(win->tool != GLYPH_TOOL_LAYER) {
        i = oy + 33 - py + ctx.glyphs[win->unicode].adv_y * win->zoom; if(i > th + 33) i = th + 33;
        if(!ctx.glyphs[win->unicode].adv_x && i >= 33 && i < ssfn_dst.h - 6) ui_number(win, 0, i, ctx.glyphs[win->unicode].adv_y,
            theme[THEME_ADV]);
    }
    if(oy + 34 < ssfn_dst.h - 6) ui_number(win, 0, oy + 34, py / win->zoom, g);
    i = oy + 33 + sh - py; if(i > th + 33) i = th + 33;
    if(i >= 33 && i < ssfn_dst.h - 6) ui_number(win, 0, i, (py + i - oy - 33) / win->zoom, g);
    ssfn_dst.h -= 4;
}

/**
 * Display grid background
 */
void ui_gridbg(ui_win_t *win, int x, int y, int w, int h, int a, uint32_t *d, int gx, int gy)
{
    int i, j, px, py, l = y * win->p + x;
    uint32_t g = theme[THEME_GRID] & 0xFFFFFF, b = theme[THEME_DARKER] & 0xFFFFFF;

    if(x + w < 0 || y + h < 0 || x >= ssfn_dst.w || y >= ssfn_dst.h || w < 1 || h < 1) return;
    if(!ctx.glyphs[win->unicode].adv_x && !ctx.glyphs[win->unicode].adv_y) a = 0;
    px = (win->zx < 0 ? -win->zx : 0);
    py = (win->zy < 0 ? -win->zy : 0);
    for(j=(y < 36 ? 36 - y : 0); j + py < h && y + j < ssfn_dst.h; j++, l += win->p)
        for(i=(x < 20 ? 20 - x : 0); i + px < w && x + i < ssfn_dst.w; i++)
            d[l + i] = !((j + py) % win->zoom) ? ((j + py) / win->zoom == ctx.baseline ? theme[THEME_BASE] : g) :
                (!((i + px) % win->zoom) ? g : b);
    if(gx != -1) {
        i = x - px + gx * win->zoom;
        if(i >= 20 && i < ssfn_dst.w) {
            l = y * win->p + i;
            for(j=(y < 36 ? 36 - y : 0); j + py <= h && y + j < ssfn_dst.h; j++, l+=win->p) d[l] = theme[THEME_GUIDE];
        }
    }
    if(gy != -1) {
        j = y - py + gy * win->zoom;
        if(j >= 36 && j < ssfn_dst.h) {
            l = j * win->p + x;
            for(i=(x < 20 ? 20 - x : 0); i + px <= w && x + i < ssfn_dst.w; i++) d[l + i] = theme[THEME_GUIDE];
        }
    }
    if(win->tool == GLYPH_TOOL_COORD) {
        j = y - py + ctx.underline * win->zoom;
        if(j >= 36 && j < ssfn_dst.h) {
            l = j * win->p + x;
            for(i=(x < 20 ? 20 - x : 0); i + px <= w && x + i < ssfn_dst.w; i++) d[l + i] = theme[THEME_UNDER];
        }
        i = x - px + ctx.glyphs[win->unicode].ovl_x * win->zoom;
        if(ctx.glyphs[win->unicode].ovl_x && i >= 20 && i < ssfn_dst.w) {
            l = y * win->p + i;
            for(j=(y < 36 ? 36 - y : 0); j + py <= h && y + j < ssfn_dst.h; j++, l+=win->p) d[l] = theme[THEME_OVL];
        }
    }
    if(win->tool != GLYPH_TOOL_LAYER && a) {
        j = y - py + ctx.glyphs[win->unicode].adv_y * win->zoom;
        if(!ctx.glyphs[win->unicode].adv_x && j >= 36 && j < ssfn_dst.h) {
            l = j * win->p + x;
            for(i=(x < 20 ? 20 - x : 0); i + px <= w && x + i < ssfn_dst.w; i++) d[l + i] = theme[THEME_ADV];
        }
        i = x - px + (ctx.glyphs[win->unicode].rtl ? ctx.glyphs[win->unicode].width - ctx.glyphs[win->unicode].adv_x :
        ctx.glyphs[win->unicode].adv_x) * win->zoom;
        if(!ctx.glyphs[win->unicode].adv_y && i >= 20 && i < ssfn_dst.w) {
            l = y * win->p + i;
            for(j=(y < 36 ? 36 - y : 0); j + py <= h && y + j < ssfn_dst.h; j++, l+=win->p) d[l] = theme[THEME_ADV];
        }
    }
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
                if(inactive < 0) {
                    win->data[k+i] = theme[inactive == -1 ? THEME_BG : THEME_CURSOR];
                } else if(inactive) {
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
    uint32_t l, b, B, d, t = theme[THEME_FG];

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
        t = theme[THEME_LIGHTER]; active = 0;
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
    if(t != theme[THEME_LIGHTER]) {
        ssfn_dst.fg = theme[THEME_BTNB];
        ui_text(win, x - 1, y + (pressed & 1 ? 1:0), str);
    }
    ssfn_dst.fg = t;
    ui_text(win, x, y + (pressed & 1 ? 2:1), str);
    ssfn_dst.fg = theme[THEME_FG];
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
            for(k = 1<<19, j = 0; j < 5 && y + j < ssfn_dst.h; j++)
                for(i = 0; i < 4; i++, k >>= 1)
                    if((numbers[d] & k) && x + i >= 0 && x + i < ssfn_dst.w) win->data[p + j*win->p +i] = c;
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
            if((d & k) && ssfn_dst.x + i >= 0 && ssfn_dst.x + i < ssfn_dst.w) win->data[p + j*win->p +i] = ssfn_dst.fg;
    ssfn_dst.x += 5;
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

/**
 * Horizontal scrollbar
 */
void ui_hscrbar(ui_win_t *win, int x, int y, int w, int h, int scroll, int page, int num, int pressed)
{
    if(!num || page > num) {
        ui_box(win, x, y, w, h, theme[pressed ? THEME_DARK : THEME_LIGHT], theme[THEME_BG], theme[pressed ? THEME_LIGHT : THEME_DARK]);
        return;
    }
    if(scroll + page > num) scroll = num - page;
    if(scroll < 0) scroll = 0;
    ui_box(win, x, y, w, h, theme[THEME_DARKER], theme[THEME_DARKER], theme[THEME_DARKER]);
    ui_box(win, x, y + (h - 20) * scroll / num, w, 20 + (h - 20) * page / num,
        theme[pressed ? THEME_DARK : THEME_LIGHT], theme[THEME_BG], theme[pressed ? THEME_LIGHT : THEME_DARK]);
}
