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

int input_maxlen = 0, input_callback = 0, input_refresh = 0;
char *input_str = NULL, *input_cur = NULL, *input_scr = NULL, *input_end = NULL, input_buf[256];
extern int seltool;

/**
 * Draw the toolbox
 */
void ui_toolbox(int idx)
{
    int i;
    char un[256];
    ui_win_t *win = &wins[idx];

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
            ui_icon(win, 4 + i * 24, 4, ICON_ABOUT + i, wins[idx].tool == -1 || (i > 1 && !ctx.filename) ? 1 : 0);
    } else {
        for(i = 0; i < 3; i++)
            ui_icon(win, 4 + i * 24, 4, ICON_MEASURES + i, 0);
        if(wins[idx].unicode >= SSFN_LIG_FIRST && wins[idx].unicode <= SSFN_LIG_LAST) {
            ui_input(win, 6 + 3 * 24, 3, 32, ctx.ligatures[wins[idx].unicode - SSFN_LIG_FIRST], wins[idx].field == 3, 15,
                1024 + wins[idx].unicode - SSFN_LIG_FIRST);
            sprintf(un, "%s #%d", lang[GLYPHS_LIGATURE], wins[idx].unicode - SSFN_LIG_FIRST);
        } else {
            ui_box(win, 6 + 3 * 24, 3, 32, 18, theme[THEME_DARK], theme[THEME_DARK], theme[THEME_DARK]);
            ui_text(win, 8 + 3 * 24, 4, utf8(wins[idx].unicode));
            sprintf(un, "%s", !wins[idx].uniname || !wins[idx].uniname[0] ? lang[GLYPHS_UNDEF] : wins[idx].uniname);
        }
        ui_text(win, 42 + 3 * 24, 3, un);
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
    uint8_t *a, *b;

    if(x < 0 || y < 0 || x >= win->w || y >= win->h) return;
    w = win->w > x + 16 ? 16 : win->w - x;
    h = win->h > y + 16 ? 16 : win->h - y;
    for(k = y*win->p + x, m = (icon * 256 + ((16 - h) << 8) + (16 - w)) << 2, j = 0; j < 16 && y + j < ssfn_dst.h;
        j++, k += win->p, m += 16 * 4) {
        for(i = 0; i < w; i++) {
            if(tools[m + (i<<2)+3]) {
                a = (uint8_t*)&win->data[k+i];
                b = (uint8_t*)&tools[m + (i<<2)];
                if(inactive)
                    a[0] = a[1] = a[2] = ((int)(b[0] + b[1] + b[2])*b[3]/6 + (256 - b[3])*a[2]) >> 8;
                else {
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

    ui_box(win, x, y, w+4, 20, theme[active ? THEME_CURSOR : THEME_DARKER], theme[THEME_DARK],
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
    if(active == -1) {
        l = d = b = B = theme[pressed < 2 ? THEME_LIGHT: THEME_BTN1BD];
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
    sprintf(numstr, "%d", num);
    ui_box(win, x, y, 44, 20, theme[active ? THEME_CURSOR : THEME_DARKER], theme[THEME_INPBG],
        theme[active ? THEME_CURSOR : THEME_LIGHT]);
    ui_box(win, x + 32, y + 1, 11, 9, theme[!sel ? THEME_DARKER : THEME_LIGHT], theme[THEME_BG],
        theme[!sel ? THEME_LIGHT : THEME_DARKER]);
    ui_tri(win, x + 34, y + 3, 1);
    ui_box(win, x + 32, y + 10, 11, 9, theme[sel == 1 ? THEME_DARKER : THEME_LIGHT], theme[THEME_BG],
        theme[sel == 1 ? THEME_LIGHT : THEME_DARKER]);
    ui_tri(win, x + 34, y + 12, 0);
    ssfn_dst.bg = 0;
    ui_text(win, x + 2, y + 1, numstr);
}

/**
 * Display a number with extra small glyphs
 */
void ui_number(ui_win_t *win, int x, int y, int n, uint32_t c)
{
    int i, j, k, m, d = 0, p = y * win->p + x;
    static uint32_t numbers[] = {0x4AAA4,0x26222,0xc248e,0xc2c2c,0x24ae2,0xe8c2c,0x68eac,0xe2444,0x4a4a4,0x4a62c};

    if(!n) { p += 12; goto zero; }
    if(n < 0) {
        win->data[p + 2*win->p ] = win->data[p + 2*win->p + 1] = win->data[p + 2*win->p + 2] = c;
        n = -n;
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
