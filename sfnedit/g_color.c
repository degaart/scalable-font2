/*
 * sfnedit/g_color.c
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
 * @brief Glyph window color picker tool
 *
 */

#include <stdint.h>
#include <stdio.h>
#include "libsfn.h"
#include "ui.h"
#include "lang.h"

int hue = 0, sat = 0, val = 0, colorsel = 0;
extern int sellayers;

/**
 * Color conversion RGB to HSV
 */
void rgb2hsv(uint32_t c)
{
    int r = (int)(((uint8_t*)&c)[2]), g = (int)(((uint8_t*)&c)[1]), b = (int)(((uint8_t*)&c)[0]), m, d;

    m = r < g? r : g; if(b < m) m = b;
    val = r > g? r : g; if(b > val) val = b;
    d = val - m;
    if(!val) { sat = 0; return; }
    sat = d * 255 / val;
    if(!sat) { return; }

    if(r == val) hue = 43*(g - b) / d;
    else if(g == val) hue = 85 + 43*(b - r) / d;
    else hue = 171 + 43*(r - g) / d;
    if(hue < 0) hue += 256;
}

/**
 * Color conversion HSV to RGB
 */
uint32_t hsv2rgb(int a, int h, int s, int v)
{
    int i, f, p, q, t;
    uint32_t c = (a & 255) << 24;

    if(!s) { ((uint8_t*)&c)[2] = ((uint8_t*)&c)[1] = ((uint8_t*)&c)[0] = v; }
    else {
        if(h > 255) i = 0; else i = h / 43;
        f = (h - i * 43) * 6;
        p = (v * (255 - s) + 127) >> 8;
        q = (v * (255 - ((s * f + 127) >> 8)) + 127) >> 8;
        t = (v * (255 - ((s * (255 - f) + 127) >> 8)) + 127) >> 8;
        switch(i) {
            case 0:  ((uint8_t*)&c)[2] = v; ((uint8_t*)&c)[1] = t; ((uint8_t*)&c)[0] = p; break;
            case 1:  ((uint8_t*)&c)[2] = q; ((uint8_t*)&c)[1] = v; ((uint8_t*)&c)[0] = p; break;
            case 2:  ((uint8_t*)&c)[2] = p; ((uint8_t*)&c)[1] = v; ((uint8_t*)&c)[0] = t; break;
            case 3:  ((uint8_t*)&c)[2] = p; ((uint8_t*)&c)[1] = q; ((uint8_t*)&c)[0] = v; break;
            case 4:  ((uint8_t*)&c)[2] = t; ((uint8_t*)&c)[1] = p; ((uint8_t*)&c)[0] = v; break;
            default: ((uint8_t*)&c)[2] = v; ((uint8_t*)&c)[1] = p; ((uint8_t*)&c)[0] = q; break;
        }
    }
    return c;
}

/**
 * View for color picker
 */
void view_color(int idx)
{
    ui_win_t *win = &wins[idx];
    char tmp[10];
    int i, j, p, x = win->w - 74, w = (win->w - 16) / 32;
    uint32_t h, c, s, *uc = (uint32_t*)&ctx.cpal;
    uint8_t *d, *a = (uint8_t*)&theme[THEME_LIGHT], *b = (uint8_t*)&theme[THEME_DARK], *C = (uint8_t*)&c;

    if(x < 0) x = 0;
    if(w > (win->h - 50) / 8) w = (win->h - 50) / 8;
    if(w < 5) w = 5;

    ssfn_dst.w = win->w - 1;
    c = theme[THEME_BG];
    s = ((C[2] + C[1] + C[0]) / 3 > 0x7F) ? 0 : 0xFFFFFF;
    for(ctx.numcpal = 0; ctx.numcpal < 0xFE && ctx.cpal[(ctx.numcpal << 2) + 3]; ctx.numcpal++);
    c = colorsel <= ctx.numcpal ? uc[colorsel] : 0;
    if(hue > 254) hue = 254;

    ssfn_dst.w = win->w - 4;
    for(j = p = 0; j < 8; j++)
        for(i = 0; i < 32; i++, p += 4) {
            ui_rect(win, 8+i*w, j*w + 30, w-2, w-2, colorsel == j*32+i ? s : theme[THEME_DARKER],
                colorsel == j*32+i ? s : theme[THEME_LIGHTER]);
            ui_argb(win, 8+i*w+1, j*w + 30+1, w - 4, w - 4, p == 1016 ? theme[THEME_FG] : (p == 1020 ?
                theme[THEME_BG] : uc[p >> 2]));
        }
    if(colorsel >= 0xFE) {
        ui_box(win, 4, win->h - 260 - 20, 32 + 256 + 8 + 16, 260, theme[THEME_BG], theme[THEME_BG], theme[THEME_BG]);
        if(selfield >= 0 && selfield < 8) selfield = 8;
    } else {
        if(!c) c = 0xFF000000;
        ssfn_dst.x = x; ssfn_dst.y = win->h - 180;
        ssfn_putc('A'); ui_num(win, x+18, ssfn_dst.y, C[3], win->field == 4, selfield);
        ssfn_dst.x = x; ssfn_dst.y += 24;
        ssfn_putc('R'); ui_num(win, x+18, ssfn_dst.y, C[2], win->field == 5, selfield - 2);
        ssfn_dst.x = x; ssfn_dst.y += 24;
        ssfn_putc('G'); ui_num(win, x+18, ssfn_dst.y, C[1], win->field == 6, selfield - 4);
        ssfn_dst.x = x; ssfn_dst.y += 24;
        ssfn_putc('B'); ui_num(win, x+18, ssfn_dst.y, C[0], win->field == 7, selfield - 6);
        sprintf(tmp, "%02X%02X%02X%02X", C[3], C[2], C[1], C[0]);
        ui_box(win, x, ssfn_dst.y + 24, 72, 16, theme[THEME_BG], theme[THEME_BG], theme[THEME_BG]);
        ui_text(win, x, ssfn_dst.y + 24, tmp);
        ui_box(win, 4, win->h - 264 - 20, 4, 264, theme[THEME_BG], theme[THEME_BG], theme[THEME_BG]);
        ui_box(win, 32 + 256 + 8 + 16, win->h - 264 - 20, 4, 264, theme[THEME_BG], theme[THEME_BG], theme[THEME_BG]);
        for(p = (win->h - 260 - 20) * win->p + 8, j = 0; j < 256; j++, p += win->p) {
            if(C[3] == 255-j) {
                win->data[p - 4] = s; win->data[p - 3] = s; win->data[p - 2] = s; win->data[p - 3 - win->p] = s;
                win->data[p - 3 + win->p] = s; win->data[p - 4 - 2*win->p] = s;
                win->data[p - 4 - win->p] = s; win->data[p - 4 + win->p] = s; win->data[p - 4 + 2*win->p] = s;
                for(i = 0; i < 15 && 8 + i < win->w; i++)
                    win->data[p + i] = s;
            } else
                for(i = 0; i < 15 && 8 + i < win->w; i++) {
                    d = (j & 8) ^ (i & 8) ? a : b;
                    ((uint8_t*)&win->data[p+i])[0] = (d[0]*j + (256 - j)*C[0])>>8;
                    ((uint8_t*)&win->data[p+i])[1] = (d[1]*j + (256 - j)*C[1])>>8;
                    ((uint8_t*)&win->data[p+i])[2] = (d[2]*j + (256 - j)*C[2])>>8;
                }
        }
        rgb2hsv(0xFF000000 | c);
        for(p = (win->h - 260 - 20) * win->p + 32, j = 0; j < 256; j++, p += win->p) {
            for(i = 0; i < 256 && 32 + i < win->w; i++) {
                h = 255-j == val ? j+(((val * i) >> 8) & 0xFF) : j; h |= 0xFF000000 | (h << 16) | (h << 8);
                win->data[p + i] = sat == i || 255-j == val ? h : hsv2rgb(255, hue,i,255-j);
            }
        }
        for(p = (win->h - 260 - 20) * win->p + 32 + 256 + 8, j = 0; j < 256; j++, p += win->p) {
            h = hsv2rgb(255, j, 255, 255);
            if(hue == j && 32 + 256 + 8 < win->w) {
                h = s;
                win->data[p + 19] = h; win->data[p + 18] = h; win->data[p + 17] = h; win->data[p + 18 - win->p] = h;
                win->data[p + 18 + win->p] = h; win->data[p + 19 - 2*win->p] = h;
                win->data[p + 19 - win->p] = h; win->data[p + 19 + win->p] = h;
                win->data[p + 19 + 2*win->p] = h;
            }
            for(i = 0; i < 15 && 32 + 256 + 8 + i < win->w; i++)
                win->data[p + i] = h;
        }
    }
    ui_button(win, x, win->h - 42, 68, lang[COLORS_SET], selfield == 8, win->field == 8);
    posx = posy = -1;
}

/**
 * On enter handler
 */
void ctrl_colors_onenter(int idx)
{
    ui_win_t *win = &wins[idx];

    win->field = selfield = -1; win->tool = GLYPH_TOOL_LAYER;
    if(sellayers < ctx.glyphs[win->unicode].numlayer && ctx.glyphs[win->unicode].layers[sellayers].color != colorsel) {
        ctx.glyphs[win->unicode].layers[sellayers].color = colorsel;
        if(ctx.glyphs[win->unicode].layers[sellayers].type != SSFN_FRAG_PIXMAP) modified++;
    }
}

/**
 * On button press handler
 */
void ctrl_colors_onbtnpress(int idx)
{
    ui_win_t *win = &wins[idx];
    int i, x = win->w - 74, w = (win->w - 16) / 32;
    uint32_t c;
    uint8_t *C = (uint8_t*)&c;
    if(x < 0) x = 0;
    if(w > (win->h - 50) / 8) w = (win->h - 50) / 8;
    if(w < 5) w = 5;
    win->field = selfield = -1;
    if(event.y >= 30 && event.y < 30 + 8*w && event.x >= 8 && event.x < 8 + 32*w) {
        i = (event.y - 30) / w * 32 + (event.x - 8) / w;
        while(i > 0 && i < 0xFE && !ctx.cpal[(i << 2) - 1]) i--;
        if(colorsel == i) selfield = -2;
        else colorsel = i;
    } else
    if(event.x > x && event.x < x + 72) {
        if(colorsel < 0xFE) {
            i = colorsel << 2;
            if(event.y > win->h - 180 && event.y < win->h - 158) {
                if(event.w & (1 << 3)) ctx.cpal[i + 3]++; else
                if(event.w & (1 << 4)) ctx.cpal[i + 3]--; else
                if(event.x >= x + 58) selfield = 0 + (event.y - (win->h - 180) > 12 ? 1 : 0);
            } else
            if(event.y > win->h - 156 && event.y < win->h - 132) {
                if(event.w & (1 << 3)) ctx.cpal[i + 2]++; else
                if(event.w & (1 << 4)) ctx.cpal[i + 2]--; else
                if(event.x >= x + 58) selfield = 2 + (event.y - (win->h - 156) > 12 ? 1 : 0);
            } else
            if(event.y > win->h - 132 && event.y < win->h - 108) {
                if(event.w & (1 << 3)) ctx.cpal[i + 1]++; else
                if(event.w & (1 << 4)) ctx.cpal[i + 1]--; else
                if(event.x >= x + 58) selfield = 4 + (event.y - (win->h - 132) > 12 ? 1 : 0);
            } else
            if(event.y > win->h - 108 && event.y < win->h - 84) {
                if(event.w & (1 << 3)) ctx.cpal[i + 0]++; else
                if(event.w & (1 << 4)) ctx.cpal[i + 0]--; else
                if(event.x >= x + 58) selfield = 6 + (event.y - (win->h - 108) > 12 ? 1 : 0);
            }
        }
        if(event.y > win->h - 42) selfield = 8;
    } else
    if(colorsel < 0xFE && event.y >= win->h - 280 && event.y < win->h - 24) {
        if(event.x >= 8 && event.x < 24) ctx.cpal[(colorsel << 2) + 3] = 255 - (event.y - (win->h - 260 - 20)); else
        if(event.x >= 32 && event.x < 32+256) { sat = event.x - 32; val = 255 - (event.y - (win->h - 260 - 20)); goto set; } else
        if(event.x >= 32 + 256 + 8 && event.x < 32 + 256 + 8 + 16) {
            hue = (event.y - (win->h - 260 - 20));
set:        c = hsv2rgb(255, hue, sat, val);
            i = colorsel << 2;
            ctx.cpal[i + 0] = C[0];
            ctx.cpal[i + 1] = C[1];
            ctx.cpal[i + 2] = C[2];
            if(!ctx.cpal[i + 3]) ctx.cpal[i + 3] = 255;
        }
    }
}

/**
 * On click (button release) handler
 */
void ctrl_colors_onclick(int idx)
{
    ui_win_t *win = &wins[idx];
    int i, x = win->w - 74, w = (win->w - 16) / 32;
    if(x < 0) x = 0;
    if(w > (win->h - 50) / 8) w = (win->h - 50) / 8;
    if(w < 5) w = 5;
    if(event.y >= 30 && event.y < 30 + 8*w && event.x >= 8 && event.x < 8 + 32*w) {
        /* double click on palette entry */
        if(colorsel == ((event.y - 30) / w * 32 + (event.x - 8) / w) && selfield == -2) ctrl_colors_onenter(idx);
    } else
    if(event.x > x + 58 && event.x < x + 72 && colorsel < 0xFE) {
        i = colorsel << 2;
        if(event.y > win->h - 180 && event.y < win->h - 155) {
            if(selfield == 0) ctx.cpal[i + 3]++; else
            if(selfield == 1) ctx.cpal[i + 3]--;
        } else
        if(event.y > win->h - 156 && event.y < win->h - 132) {
            if(selfield == 2) ctx.cpal[i + 2]++; else
            if(selfield == 3) ctx.cpal[i + 2]--;
        } else
        if(event.y > win->h - 132 && event.y < win->h - 108) {
            if(selfield == 4) ctx.cpal[i + 1]++; else
            if(selfield == 5) ctx.cpal[i + 1]--;
        } else
        if(event.y > win->h - 108 && event.y < win->h - 84) {
            if(selfield == 6) ctx.cpal[i + 0]++; else
            if(selfield == 7) ctx.cpal[i + 0]--;
        }
    }
    if(event.y > win->h - 42 && event.x > x && event.x < x + 72 && selfield == 8) ctrl_colors_onenter(idx);
    selfield = -1;
}

/**
 * On mouse move handler
 */
void ctrl_colors_onmove(int idx)
{
    ui_win_t *win = &wins[idx];
    int x = win->w - 74;
    if(x < 0) x = 0;
    if(colorsel < 0xFE && (event.w & 1) && event.x >= 8 && event.x < 32 + 256 + 8 + 16 && event.y >= win->h - 280 &&
      event.y < win->h - 24) {
        ctrl_colors_onbtnpress(idx);
        view_color(idx);
        ui_flushwin(win, 0, 0, win->w, win->h);
    }
}
