/*
 * sfnedit/m_about.c
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
 * @brief Main window About
 *
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "libsfn.h"
#include "ui.h"
#include "lang.h"

extern ssfn_t logofnt;
char repo_url[] = "https://gitlab.com/bztsrc/scalable-font2";

void about_open_repo()
{
    char cmd[256];
    if(!fork()) {
        fclose(stdin);
        fclose(stdout);
        fclose(stderr);
        sprintf(cmd, "xdg-open %s", repo_url);
        system(cmd);
        exit(0);
    }
    wins[0].field = -1;
    ui_refreshwin(0, 0, 0, wins[0].w, wins[0].h);
}

void view_about()
{
    int i, j, p;
    char *s = "Scalable Screen Font Editor";
    ui_win_t *win = &wins[0];
    p = (win->w - 320) / 2;
    ssfn_dst.ptr = (uint8_t*)win->data;
    ssfn_dst.p = win->p*4;
    ssfn_dst.w = win->w;
    ssfn_dst.h = win->h;
    ssfn_dst.x = (win->w - 620) / 2;
    ssfn_dst.y = 160;
    ssfn_dst.fg = theme[THEME_FG];
    ssfn_dst.bg = 0;
    while((i = ssfn_render(&logofnt, &ssfn_dst, s)) > 0) s += i;
    ssfn_dst.fg = 0xFF000000;
    j = wins[0].field == 6 ? THEME_FG : THEME_BG;
    ui_box(win, p - 2, 180, 324, 18, theme[j], theme[wins[0].field == 6 ? THEME_LIGHT: THEME_BG], theme[j]);
    ui_text(win, p + 1, 181, repo_url);
    ssfn_dst.fg = theme[THEME_FG];
    ui_text(win, p, 180, repo_url);
    ssfn_dst.y = win->h - 16 * 16;
    if(ssfn_dst.y < 208) ssfn_dst.y = 208;
    ui_text(win, 8, ssfn_dst.y, "Copyright (C) 2020 bzt (bztsrc@gitlab)");
    ssfn_dst.y += 16;
    for(i = CPYRGT_0; i <= CPYRGT_9 && (int)ssfn_dst.y < win->h - 18; i++) {
        ssfn_dst.y += 16;
        ui_text(win, 8, ssfn_dst.y, lang[i]);
    }
}

void ctrl_about_onmove()
{
    int p = (wins[0].w - 320) / 2;
    if(event.y >= 180 && event.y < 196 && event.x >= p && event.x < p + 320)
        cursor = CURSOR_GRAB;
}

void ctrl_about_onclick()
{
    int p = (wins[0].w - 320) / 2;
    if(event.y >= 180 && event.y < 196 && event.x >= p && event.x < p + 320)
        about_open_repo();
}

void ctrl_about_onenter()
{
    if(wins[0].field == 6)
        about_open_repo();
}
