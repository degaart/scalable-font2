/*
 * sfnedit/g_kern.c
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
 * @brief Glyph window kerning tool
 *
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "libsfn.h"
#include "ui.h"
#include "lang.h"

char ksearch[32] = { 0 };
int selkern = 0, scrollkern = 0, pagekern = 0, numk = 0, kres[0x110000], kernn, kernx, kerny;
extern int mousex, mousey, isclick;
void ctrl_zoom_in(int idx, int mx, int my);
void ctrl_zoom_out(int idx, int mx, int my);
void ctrl_move(int idx, int mx, int my);

/**
 * Kerning window
 */
void view_kern(int idx)
{
    ui_win_t *win = &wins[idx];
    int i, j, k, l, n, x = win->w - 74, ox = win->zx > 0 ? win->zx : 0, oy = win->zy > 0 ? win->zy : 0;
    char st[8];
    uint32_t c;

    if(x < 0) x = 0;
    ssfn_dst.w = x - 6;
    ssfn_dst.h = win->h - 24;
    pagekern = (win->h - 47 - 29) / 22; if(pagekern < 1) pagekern = 1;
    if(!numk) input_refresh = 1;
    if(input_refresh && !ksearch[0]) {
        ksearch[0] = 0;
        for(i = 0; i < ctx.glyphs[win->unicode].numkern; i++) kres[i] = ctx.glyphs[win->unicode].kern[i].n;
        numk = ctx.glyphs[win->unicode].numkern;
    } else
    if(input_refresh) {
        numk = scrollkern = 0;
        l = strlen(ksearch);
        n = (ksearch[0] == 'U' || ksearch[0] == 'u') && ksearch[1] == '+' ? (int)gethex(ksearch + 2, 6) : -1;
        /* exact matches first */
        for(i = 0; i < UNICODE_NUMNAMES; i++) {
            if(ctx.glyphs[i].height && (!strcmp(utf8(uninames[i].unicode), ksearch) || uninames[i].unicode == n))
                kres[numk++] = uninames[i].unicode;
        }
        /* name matches after */
        for(i = 0; i < UNICODE_NUMNAMES; i++) {
            if(ctx.glyphs[i].height && (strcmp(utf8(uninames[i].unicode), ksearch) && uninames[i].unicode != n)) {
                k = strlen(uninames[i].name);
                if(k < l) continue;
                k -= l;
                for(j = 0; j <= k && ui_casecmp(uninames[i].name + j, ksearch, l); j++);
                if(j > k) continue;
                kres[numk++] = uninames[i].unicode;
            }
        }
    }
    if(selkern >= numk) selkern = numk - 1;
    if(selkern < 0) selkern = 0;
    if(scrollkern + pagekern > numk) scrollkern = numk-pagekern;
    if(scrollkern < 0) scrollkern = 0;
    input_refresh = 0;

    kernn = -1; kernx = 0; kerny = 0;
    ui_grid(win, ctx.glyphs[win->unicode].width, ctx.glyphs[win->unicode].height);
    if(ctx.glyphs[win->unicode].width && ctx.glyphs[win->unicode].height) {
        if(!ctx.glyphs[win->unicode].adv_y) {
            ui_gridbg(win, 20 + ox + (ctx.glyphs[win->unicode].rtl ? -win->zoom * ctx.width :
                win->zoom * ctx.glyphs[win->unicode].width),
                36 + oy, win->zoom * ctx.width, win->zoom * ctx.height, 0, win->data, -1, -1);
            ui_gridbg(win, 20 + ox, 36 + oy, win->zoom * ctx.glyphs[win->unicode].width, win->zoom * ctx.height,
                1, win->data, -1, -1);
        } else if(!ctx.glyphs[win->unicode].adv_x) {
            i = win->zoom * (ctx.glyphs[win->unicode].width - ctx.width) / 2;
            ui_gridbg(win, 20 + ox + i, 36 + oy + win->zoom * ctx.glyphs[win->unicode].height,
                win->zoom * ctx.width, win->zoom * ctx.height, 0, win->data, -1, -1);
            ui_gridbg(win, 20 + ox + i, 36 + oy, win->zoom * ctx.width, win->zoom * ctx.glyphs[win->unicode].height,
                1, win->data, -1, -1);
        }
        ui_edit(win, 20 + win->zx, 36 + win->zy, win->unicode, -2);
        if(selkern < numk && kres[selkern] > 32) {
            kernn = kres[selkern];
            for(i = 0; i < ctx.glyphs[win->unicode].numkern && ctx.glyphs[win->unicode].kern[i].n != kres[selkern]; i++);
            if(i < ctx.glyphs[win->unicode].numkern) {
                kernx = ctx.glyphs[win->unicode].kern[i].x;
                kerny = ctx.glyphs[win->unicode].kern[i].y;
            }
            if(!ctx.glyphs[win->unicode].adv_y) {
                ui_edit(win, 20 + win->zx + win->zoom * (ctx.glyphs[win->unicode].rtl ? ctx.glyphs[win->unicode].width -
                    ctx.glyphs[win->unicode].adv_x - ctx.glyphs[kres[selkern]].width - kernx :
                    ctx.glyphs[win->unicode].adv_x + kernx), 36 + win->zy + kerny * win->zoom, kres[selkern], -1);
            } else if(!ctx.glyphs[win->unicode].adv_x) {
                i = win->zoom * ctx.glyphs[kres[selkern]].width / 2;
                ui_edit(win, 20 + win->zx, 36 + win->zy + (ctx.glyphs[win->unicode].adv_y+kerny) * win->zoom, kres[selkern], -1);
            }
        }
    }
    ssfn_dst.w = win->w - 1;
    ssfn_dst.h = win->h - 20;

    ui_icon(win, win->w - 130 - 16, 4, ICON_SEARCH, 0);
    ui_input(win, win->w - 128, 2, 120, ksearch, win->field == 4, 31, 0);

    ui_box(win, x, win->h - 42, 22, 22, theme[win->field == 6 ? THEME_CURSOR : (selfield == 0 ? THEME_DARKER : THEME_LIGHT)],
        theme[win->field == 6 ? THEME_LIGHT : THEME_BG],
        theme[win->field == 6 ? THEME_CURSOR : (selfield == 0 ? THEME_LIGHT : THEME_DARKER)]);
    ui_icon(win, x+3, win->h - 39, ICON_RHORIZ, ctx.glyphs[win->unicode].adv_y > 0);

    ui_box(win, x+24, win->h - 42, 22, 22, theme[win->field == 7 ? THEME_CURSOR : (selfield == 1 ? THEME_DARKER : THEME_LIGHT)],
        theme[win->field == 7 ? THEME_LIGHT : THEME_BG],
        theme[win->field == 7 ? THEME_CURSOR : (selfield == 1 ? THEME_LIGHT : THEME_DARKER)]);
    ui_icon(win, x+27, win->h - 39, ICON_HORIZ, ctx.glyphs[win->unicode].adv_y > 0);

    ui_box(win, x+48, win->h - 42, 22, 22, theme[win->field == 8 ? THEME_CURSOR : (selfield == 2 ? THEME_DARKER : THEME_LIGHT)],
        theme[win->field == 8 ? THEME_LIGHT : THEME_BG],
        theme[win->field == 8 ? THEME_CURSOR : (selfield == 2 ? THEME_LIGHT : THEME_DARKER)]);
    ui_icon(win, x+51, win->h - 39, ICON_DELETE, ctx.glyphs[win->unicode].numkern < 1);

    if(selkern >= ctx.glyphs[win->unicode].numkern) selkern = ctx.glyphs[win->unicode].numkern - 1;
    if(selkern < 0) selkern = 0;

    ui_box(win, x, 26, 70, win->h - 26 - 46, theme[win->field == 5 ? THEME_CURSOR : THEME_DARKER], theme[THEME_BG],
        theme[win->field == 5 ? THEME_CURSOR : THEME_LIGHT]);
    ssfn_dst.w = x + 68; ssfn_dst.h = win->h - 49; ssfn_dst.y = 29; x += 3;
    for(i = scrollkern, n = 0; i < numk && ssfn_dst.y < ssfn_dst.h; i++, ssfn_dst.y += 11) {
        if(i == selkern) {
            c = theme[THEME_SELBG];
            ssfn_dst.fg = theme[THEME_SELFG];
        } else {
            c = theme[THEME_DARK];
            ssfn_dst.fg = theme[THEME_FG];
        }
        ui_box(win, x, ssfn_dst.y, 64, 20, c, c, c);
        ssfn_dst.x = x + 2; ssfn_dst.y++;
        ssfn_putc(kres[i]);
        sprintf(st, "%06X", kres[i]);
        ssfn_dst.x = x + 22; ssfn_dst.y += 3;
        ui_hex(win, st[0]); ui_hex(win, st[1]); ui_hex(win, st[2]);
        ssfn_dst.x = x + 22; ssfn_dst.y += 7;
        ui_hex(win, st[3]); ui_hex(win, st[4]); ui_hex(win, st[5]);
        for(n = 0; n < ctx.glyphs[win->unicode].numkern && ctx.glyphs[win->unicode].kern[n].n != kres[i]; n++);
        if(n < ctx.glyphs[win->unicode].numkern && kres[i] == ctx.glyphs[win->unicode].kern[n].n &&
            (ctx.glyphs[win->unicode].kern[n].x || ctx.glyphs[win->unicode].kern[n].y)) {
            ssfn_dst.x = x + 40; ssfn_dst.y -= 7;
            ui_hex(win, 'X'); ui_number(win, x + 46, ssfn_dst.y, ctx.glyphs[win->unicode].kern[n].x, theme[THEME_FG]);
            ssfn_dst.x = x + 40; ssfn_dst.y += 7;
            ui_hex(win, 'Y'); ui_number(win, x + 46, ssfn_dst.y, ctx.glyphs[win->unicode].kern[n].y, theme[THEME_FG]);
        }
    }
}

/**
 * On enter handler
 */
void ctrl_kern_onenter(int idx)
{
    ui_win_t *win = &wins[idx];
    int n;
    switch(win->field) {
        case 6: ctx.glyphs[win->unicode].rtl = 1; break;
        case 7: ctx.glyphs[win->unicode].rtl = 0; break;
        case 8:
            for(n = 0; n < ctx.glyphs[win->unicode].numkern && ctx.glyphs[win->unicode].kern[n].n != kres[selkern]; n++);
            if(n < ctx.glyphs[win->unicode].numkern) {
                memcpy(&ctx.glyphs[win->unicode].kern[n], &ctx.glyphs[win->unicode].kern[n+1],
                    (ctx.glyphs[win->unicode].numkern - n) * sizeof(sfnkern_t));
                ctx.glyphs[win->unicode].numkern--;
            }
            input_refresh = 1;
        break;
    }
}

/**
 * On key handler
 */
void ctrl_kern_onkey(int idx)
{
    ui_win_t *win = &wins[idx];
    if(win->field == 5) {
        switch(event.x) {
            case K_UP: if(selkern > 0) selkern--; break;
            case K_DOWN: if(selkern < numk - 1) selkern++; break;
        }
    } else
    if(event.x >= ' ') {
        strcpy(ksearch, (char*)&event.x);
        win->field = 4;
        input_refresh = 1;
        input_maxlen = 0;
        input_str = NULL;
    } else {
        switch(event.x) {
            case K_PGUP: if(selkern > 0) selkern--; break;
            case K_PGDN: if(selkern < numk - 1) selkern++; break;
            case K_UP: kerny--; break;
            case K_DOWN: kerny++; break;
            case K_LEFT: kernx--; break;
            case K_RIGHT: kernx++; break;
        }
        sfn_kernadd(win->unicode, kernn, kernx, kerny);
        modified++;
    }
    ui_refreshwin(idx, 20, 29, win->w - 20, win->h - 29 - 24);
}

/**
 * On button press handler
 */
void ctrl_kern_onbtnpress(int idx)
{
    ui_win_t *win = &wins[idx];
    int x = win->w - 74, ox = win->zx > 0 ? win->zx : 0, oy = win->zy > 0 ? win->zy : 0;
    if(x < 0) x = 0;
    selfield = win->field = mousex = mousey = -1;
    if(event.y < 26) {
        if(event.x > win->w - 130 - 16) win->field = 4;
    } else
    if(event.x > x) {
        if(event.y > 26 && event.y < win->h - 44 && event.x > x && event.x < x + 70) {
            if(event.w & 1) selkern = (event.y - 31) / 22 + scrollkern; else
            if(event.w & (1 << 3)) scrollkern--; else
            if(event.w & (1 << 4)) scrollkern++;
        } else
        if(event.y > win->h - 42 && event.x > x) {
            if(event.x > x && event.x < x + 24 && ctx.glyphs[win->unicode].adv_y < 1) selfield = 0; else
            if(event.x > x + 24 && event.x < x + 48 && ctx.glyphs[win->unicode].adv_y < 1) selfield = 1; else
            if(event.x > x + 48 && event.x < x + 72 && ctx.glyphs[win->unicode].numkern) selfield = 2;
        }
    } else
    if(event.y < win->h - 22) {
        if(event.w & (1 << 3)) ctrl_zoom_in(event.win, event.x, event.y); else
        if(event.w & (1 << 4)) ctrl_zoom_out(event.win, event.x, event.y); else
        if(event.x >= ox + 20 && event.y >= oy + 36 &&
            event.x <= ox + 20 + win->zoom * ctx.glyphs[win->unicode].width &&
            event.y <= oy + 36 + win->zoom * ctx.glyphs[win->unicode].height) {
                mousex = event.x; mousey = event.y;
                isclick = 1;
        }
    }
}

/**
 * On click (button release) handler
 */
void ctrl_kern_onclick(int idx)
{
    ui_win_t *win = &wins[idx];
    int x = win->w - 74, ox = win->zx > 0 ? win->zx : 0, oy = win->zy > 0 ? win->zy : 0;
    if(x < 0) x = 0;
    if(event.y < 26 && event.x > win->w - 130 - 16) win->field = 4; else
    if(event.x > x) {
        if(event.y > win->h - 42 && event.x > x) {
            if(event.x > x && event.x < x + 24 && selfield == 0) { win->field = 6; ctrl_kern_onenter(idx); } else
            if(event.x > x + 24 && event.x < x + 48 && selfield == 1) { win->field = 7; ctrl_kern_onenter(idx); } else
            if(event.x > x + 48 && event.x < x + 72 && ctx.glyphs[win->unicode].numkern && selfield == 2) {
                win->field = 8; ctrl_kern_onenter(idx);
            }
            win->field = -1;
        }
    } else
    if(isclick && event.x >= ox + 20 && event.y >= oy + 36 &&
        event.x <= ox + 20 + win->zoom * ctx.glyphs[win->unicode].width &&
        event.y <= oy + 36 + win->zoom * ctx.glyphs[win->unicode].height && posx != -1 && posy != -1) {
            printf("click\n");
    }
    cursor = CURSOR_PTR; isclick = 0;
    selfield = mousex = mousey = -1;
}

/**
 * On mouse move handler
 */
void ctrl_kern_onmove(int idx)
{
    ui_win_t *win = &wins[idx];
    int i, x = win->w - 74, ox = win->zx > 0 ? win->zx : 0, oy = win->zy > 0 ? win->zy : 0;
    if(x < 0) x = 0;
    posx = posy = -1; isclick = 0;
    if(event.x > x) {
        if(event.y > 26 && event.y < win->h - 44 && event.x > x && event.x < x + 70) {
            i = (event.y - 31) / 22 + scrollkern;
            if(i < numk) ui_chrinfo(kres[i]);
        } else
        if(event.y > win->h - 42) {
            if(event.x > x && event.x < x + 24) status = lang[COORDS_RTL]; else
            if(event.x > x + 24 && event.x < x + 48) status = lang[COORDS_LTR]; else
            if(event.x > x + 48 && event.x < x + 72) status = lang[KERN_DELETE];
        }
    } else
    if(event.x >= ox + 20 && event.y >= oy + 36 &&
        event.x <= ox + 20 + win->zoom * ctx.glyphs[win->unicode].width &&
        event.y <= oy + 36 + win->zoom * ctx.glyphs[win->unicode].height && event.y < win->h - 22) {
            if(mousex != -1 && mousey != -1) ctrl_move(event.win, event.x, event.y);
            posx = (event.x - ox - 20 - (win->zx < 0 ? win->zx : 0)) / win->zoom;
            if(posx >= ctx.glyphs[win->unicode].width) posx = -1;
            posy = (event.y - oy - 36 - (win->zy < 0 ? win->zy : 0)) / win->zoom;
            if(posy >= ctx.glyphs[win->unicode].height) posy = -1;
    }
}
