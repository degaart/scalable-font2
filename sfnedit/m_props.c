/*
 * sfnedit/m_props.c
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
 * @brief Main window font properties tool
 *
 */

#include <stdint.h>
#include <stdio.h>
#include "libsfn.h"
#include "ui.h"
#include "lang.h"

int fieldtexts = 0, typetexts = 0;

/**
 * Font properties window
 */
void view_props()
{
    int i, j, k;
    ui_win_t *win = &wins[0];
    if(!fieldtexts) {
        for(i = PROP_TYPE; i <= PROP_LICENSE; i++) {
            j = ui_textwidth(lang[i]) + 28;
            if(j > fieldtexts) fieldtexts = j;
        }
        for(i = PROP_SERIF; i <= PROP_HAND; i++) {
            j = ui_textwidth(lang[i]) + 8;
            if(j > typetexts) typetexts = j;
        }
    }
    if(wins[0].field != 6) {
        ui_box(win, fieldtexts, 40, typetexts + 19, (PROP_HAND - PROP_SERIF + 1) * 18 + 4, theme[THEME_BG],
            theme[THEME_BG], theme[THEME_BG]);
    }
    j = win->w - fieldtexts - 20;
    ui_text(win, 20, 40, lang[PROP_TYPE]);
    ui_input(win, fieldtexts, 40, typetexts + 15, lang[PROP_SERIF + ctx.family], 0, 16, 0);
    ui_box(win, fieldtexts + typetexts + 3, 41, 15, 18, theme[selfield == 6 ? THEME_DARKER : THEME_LIGHT], theme[THEME_BG],
        theme[selfield == 6 ? THEME_LIGHT : THEME_DARKER]);
    ui_tri(win, fieldtexts + typetexts + 7, 48, 0);
    k = fieldtexts + typetexts + 32;
    i = (win->w - k - 20) / 4;
    ui_bool(win, k, 40, lang[PROP_BOLD], ctx.style & SSFN_STYLE_BOLD, wins[0].field == 7);
    ui_bool(win, k+i, 40, lang[PROP_ITALIC], ctx.style & SSFN_STYLE_ITALIC, wins[0].field == 8);
    ui_bool(win, k+2*i, 40, lang[PROP_USRDEF1], ctx.style & SSFN_STYLE_USRDEF1, wins[0].field == 9);
    ui_bool(win, k+3*i, 40, lang[PROP_USRDEF2], ctx.style & SSFN_STYLE_USRDEF2, wins[0].field == 10);
    ui_text(win, 20, 64, lang[PROP_NAME]);
    ui_input(win, fieldtexts, 64, j, ctx.name, wins[0].field == 11, 255, 3);
    ui_text(win, 20, 88, lang[PROP_FAMILY]);
    ui_input(win, fieldtexts, 88, j, ctx.familyname, wins[0].field == 12, 255, 4);
    ui_text(win, 20,112, lang[PROP_SUBFAM]);
    ui_input(win, fieldtexts,112, j, ctx.subname, wins[0].field == 13, 255, 5);
    ui_text(win, 20,136, lang[PROP_REVISION]);
    ui_input(win, fieldtexts,136, j, ctx.revision, wins[0].field == 14, 255, 6);
    ui_text(win, 20,160, lang[PROP_MANUFACTURER]);
    ui_input(win, fieldtexts,160, j, ctx.manufacturer, wins[0].field == 15, 255, 7);
    ui_text(win, 20,184, lang[PROP_LICENSE]);
    ui_input(win, fieldtexts,184, j, ctx.license, wins[0].field == 16, 255, 8);

    if(wins[0].field == 6) {
        ui_box(win, fieldtexts, 40, typetexts + 19, (PROP_HAND - PROP_SERIF + 1) * 18 + 4, theme[THEME_DARKER],
            theme[THEME_INPBG], theme[THEME_LIGHT]);
        for(i = 0; i <= PROP_HAND - PROP_SERIF; i++) {
            if(i == ctx.family)
                ui_box(win, fieldtexts + 1, 41 + i * 18, typetexts + 17, 20, theme[THEME_SELBG],
                    theme[THEME_SELBG], theme[THEME_SELBG]);
            ui_text(win, fieldtexts + 2, 41 + i * 18, lang[i + PROP_SERIF]);
        }
        ui_box(win, fieldtexts + typetexts + 3, 41, 15, 18, theme[THEME_DARKER], theme[THEME_BG], theme[THEME_LIGHT]);
        ui_tri(win, fieldtexts + typetexts + 7, 48, 0);
    }
}

/**
 * On enter handler
 */
void ctrl_props_onenter()
{
    if(wins[0].field == 7) ctx.style ^= SSFN_STYLE_BOLD; else
    if(wins[0].field == 8) ctx.style ^= SSFN_STYLE_ITALIC; else
    if(wins[0].field == 9) ctx.style ^= SSFN_STYLE_USRDEF1; else
    if(wins[0].field ==10) ctx.style ^= SSFN_STYLE_USRDEF2; else
    if(wins[0].field == 6) wins[0].field = 7;
    ui_refreshwin(0, 0, 0, wins[0].w, wins[0].h);
}

/**
 * On key handler
 */
void ctrl_props_onkey()
{
    if(wins[0].field == 6) {
        if(event.x == K_UP) {
            if(ctx.family > 0) {
                ctx.family--;
                ui_refreshwin(0, 0, 0, wins[0].w, wins[0].h);
            }
        } else
        if(event.x == K_DOWN) {
            if(ctx.family < PROP_HAND - PROP_SERIF + 1) {
                ctx.family++;
                ui_refreshwin(0, 0, 0, wins[0].w, wins[0].h);
            }
        }
    }
}

/**
 * On button press handler
 */
void ctrl_props_onbtnpress()
{
    int i, k;
    ui_win_t *win = &wins[0];

    selfield = -1;
    if(wins[0].field == 6) {
        if(event.y > 40 && event.y < 41 + (PROP_HAND - PROP_SERIF + 1) * 18 && event.x > fieldtexts &&
            event.x < fieldtexts + typetexts + 2) ctx.family = (event.y - 41) / 18;
    } else
    if(event.y > 40 && event.y < 60) {
        k = fieldtexts + typetexts + 32;
        i = (win->w - k - 20) / 4;
        if(event.x >= fieldtexts && event.x < fieldtexts + typetexts + 18) selfield = 6; else
        if(event.x >= k && event.x <= k + i - 1) ctx.style ^= SSFN_STYLE_BOLD; else
        if(event.x >= k + i && event.x <= k + 2*i - 1) ctx.style ^= SSFN_STYLE_ITALIC; else
        if(event.x >= k + 2*i && event.x <= k + 3*i - 1) ctx.style ^= SSFN_STYLE_USRDEF1; else
        if(event.x >= k + 3*i && event.x <= k + 4*i - 1) ctx.style ^= SSFN_STYLE_USRDEF2;
    } else
    if(event.x > fieldtexts && event.y >= 64 && event.y < 208) wins[0].field = (event.y - 64) / 24 + 11;
    else wins[0].field = -1;
}

/**
 * On click (button release) handler
 */
void ctrl_props_onclick()
{
    if(wins[0].field == 6) {
        wins[0].field = -1;
    } else {
        if(event.y > 40 && event.y < 60 && event.x >= fieldtexts && event.x < fieldtexts + typetexts + 18 && selfield == 6)
            wins[0].field = 6;
    }
    selfield = -1;
}
