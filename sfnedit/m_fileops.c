/*
 * sfnedit/m_fileops.c
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
 * @brief Main window file operation tools
 *
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <time.h>
#include <sys/stat.h>
#include <inttypes.h>
#include <stdlib.h>
#include <limits.h>
#include "libsfn.h"
#include "ui.h"
#include "lang.h"
#ifdef __WIN32__
#include <windows.h>
#include <winnls.h>
#include <shlobj.h>
#else
#ifndef __wur
#define __wur
#endif
extern char *realpath (const char *__restrict __name, char *__restrict __resolved) __THROW __wur;
#endif
#ifndef PRIu64
#if __WORDSIZE == 64
#define PRIu64 "lu"
#define PRId64 "ld"
#else
#define PRIu64 "llu"
#define PRId64 "lld"
#endif
#endif

#ifndef PATH_MAX
# ifdef MAX_PATH
#  define PATH_MAX MAX_PATH
# else
#  define PATH_MAX 65536
# endif
#endif
#ifndef FILENAME_MAX
# define FILENAME_MAX 256
#endif

typedef struct {
    char *name;
    char type;
    uint64_t size;
    time_t time;
} filelist_t;
filelist_t *files = NULL;

int numfiles = 0, scrollfiles = 0, pagefiles = 0, selfiles = -1, ordering = 0, pathX[PATH_MAX/FILENAME_MAX+64] = { 0 }, pathlen;
int lastsave = -1, clkfiles = 0, question_y = 0;
char fn[PATH_MAX+64] = {0}, filename[FILENAME_MAX+64] = {0}, path[PATH_MAX/FILENAME_MAX+64][FILENAME_MAX], fsearch[256] = { 0 };
char strrs[16] = { 0 }, strre[16] = { 0 }, fstatus[256];

/**
 * Sort file names
 */
static int fncmp(const void *a, const void *b)
{
    filelist_t *A = (filelist_t*)a, *B = (filelist_t*)b;
    if(ordering < 4) {
        if(A->type && !B->type) return 1;
        if(!A->type && B->type) return -1;
    }
    switch(ordering) {
        case 0: return strcmp(A->name, B->name);
        case 1: return strcmp(B->name, A->name);
        case 2: return A->size > B->size;
        case 3: return B->size > A->size;
        case 4: return (int)(A->time - B->time);
        case 5: return (int)(B->time - A->time);
    }
    return 0;
}

/**
 * Read directory contents
 */
void fileops_readdir(int save)
{
    char tmp[PATH_MAX+64];
    DIR *dir;
    struct dirent *de;
    struct stat st;
    int i, j, k, l = strlen(fsearch);

    if(files) {
        for(i = 0; i < numfiles; i++)
            if(files[i].name)
                free(files[i].name);
        free(files);
        files = NULL;
    }
    numfiles = scrollfiles = 0; selfiles = clkfiles = -1;
    for(tmp[0] = 0, i = 0; i < pathlen; i++)
        strcat(tmp, path[i]);
    j = strlen(tmp);
    dir = opendir(tmp);
    if(dir) {
        while((de = readdir(dir))) {
            if(de->d_name[0] == '.') continue;
            strcpy(tmp + j, de->d_name);
            if(stat(tmp, &st)) continue;
            if(!S_ISREG(st.st_mode) && !S_ISDIR(st.st_mode)) continue;
            k = strlen(de->d_name);
            if(!S_ISDIR(st.st_mode)) {
                if(k < 3) continue;
                if(!memcmp(de->d_name + k - 3, ".gz", 3)) k -= 3;
                if(k < 5) continue;
                if( memcmp(de->d_name + k - 4, ".sfn", 4) && memcmp(de->d_name + k - 4, ".asc", 4)
#ifndef USE_NOFOREIGN
                    && (save || (memcmp(de->d_name + k - 4, ".pf2", 4) &&
                    memcmp(de->d_name + k - 4, ".pfa", 4) && memcmp(de->d_name + k - 4, ".pfb", 4) &&
                    memcmp(de->d_name + k - 5, ".woff",5) && memcmp(de->d_name + k - 5, "woff2",5) &&
                    memcmp(de->d_name + k - 4, ".ttf", 4) && memcmp(de->d_name + k - 4, ".ttc", 4) &&
                    memcmp(de->d_name + k - 4, ".otf", 4) && memcmp(de->d_name + k - 4, ".pcf", 4) &&
                    memcmp(de->d_name + k - 4, ".sfd", 4) && memcmp(de->d_name + k - 4, ".svg", 4) &&
                    memcmp(de->d_name + k - 4, ".fnt", 4) && memcmp(de->d_name + k - 4, ".fon", 4) &&
                    memcmp(de->d_name + k - 4, ".psf", 4) && memcmp(de->d_name + k - 5, ".psfu",5) &&
                    memcmp(de->d_name + k - 4, ".bdf", 4) && memcmp(de->d_name + k - 4, ".hex", 4) &&
                    memcmp(de->d_name + k - 4, ".tga", 4) && memcmp(de->d_name + k - 4, ".png", 4)))
#endif
                    ) continue;
            }
            if(l) {
                if(k < l) continue;
                k -= l;
                for(i = 0; i <= k && ui_casecmp(de->d_name + i, fsearch, l); i++);
                if(i > k) continue;
            }
            i = numfiles++;
            files = (filelist_t*)realloc(files, numfiles * sizeof(filelist_t));
            if(!files) { numfiles = 0; break; }
            files[i].name = (char*)malloc(strlen(de->d_name)+1);
            if(!files[i].name) { numfiles--; continue; }
            strcpy(files[i].name, de->d_name);
            files[i].type = S_ISDIR(st.st_mode) ? 0 : 1;
            files[i].size = st.st_size;
            files[i].time = st.st_mtime;
        }
        closedir(dir);
    }
    qsort(files, numfiles, sizeof(filelist_t), fncmp);
}

/**
 * File operations windows, load / save
 */
void view_fileops(int save)
{
    struct tm *lt;
    char *s, tmp[32];
    int i, j, k;
    ui_win_t *win = &wins[0];
    time_t now = time(NULL), diff;
#ifdef __WIN32__
    wchar_t home[MAX_PATH];
#endif

    if(!fn[0]) {
        if(ctx.filename)
#ifdef __WIN32__
            strcpy(fn, ctx.filename);
#else
            s = realpath(ctx.filename, fn);
#endif
        else {
#ifdef __WIN32__
            if(SHGetFolderPathW(HWND_DESKTOP, CSIDL_DESKTOPDIRECTORY, NULL, 0, home))
                wsprintfW(home, L".\\");
            for(i = 0, s = fn; home[i]; i++) {
                if(home[i] < 0x80) {
                    *s++ = home[i];
                } else if(home[i] < 0x800) {
                    *s++ = ((home[i]>>6)&0x1F)|0xC0;
                    *s++ = (home[i]&0x3F)|0x80;
                } else {
                    *s++ = ((home[i]>>12)&0x0F)|0xE0;
                    *s++ = ((home[i]>>6)&0x3F)|0x80;
                    *s++ = (home[i]&0x3F)|0x80;
                }
            }
            *s++ = DIRSEP;
            *s = 0;
#else
            if((!getcwd(fn, sizeof(fn) - 3) || !fn[0] || !memcmp(fn, "/usr", 4)) && (s = getenv("HOME")) != NULL) strcpy(fn, s);
            strcat(fn, "/");
#endif
            strcpy(filename, "noname.sfn");
        }
    }
    memset(path, 0, sizeof(path));
    for(i = pathlen = 0, s = fn; *s; s++) {
        if(i < FILENAME_MAX-1) path[pathlen][i++] = *s;
        if(*s == DIRSEP) { i = 0; pathlen++; }
    }
    memset(pathX, 0, sizeof(pathX));
    if(wins[0].field == 6) j = k = THEME_CURSOR; else { j = THEME_LIGHT; k = THEME_DARK; }
    ssfn_dst.w = win->w - 158; if(ssfn_dst.w < 1) ssfn_dst.w = 1;
    for(i = 0, ssfn_dst.x = 10; i < pathlen && ssfn_dst.x < win->w - 158; i++) {
        pathX[i] = ssfn_dst.x;
        ui_text(win, ssfn_dst.x, 30, path[i]);
        ui_rect(win, pathX[i] - 3, 29, ssfn_dst.x - pathX[i] + 6, 20, theme[selfield == i + 4 ? k : j],
            theme[selfield == i + 4 ? j : k]);
        ssfn_dst.x += 8;
    }
    pathX[i] = ssfn_dst.x - 8;
    if(!files || lastsave != save || input_refresh) fileops_readdir(save);
    lastsave = save;
    if(wins[0].field == 8 && selfiles == -1 && numfiles > 0)
        selfiles = 0;
    pagefiles = (win->h - 51 - 72) / 16; if(pagefiles < 2) pagefiles = 2;
    if(selfiles + 1 > scrollfiles + pagefiles - 1) scrollfiles = selfiles - pagefiles + 1;
    if(selfiles >= 0 && selfiles < scrollfiles) scrollfiles = selfiles;
    if(wins[0].tool == MAIN_TOOL_LOAD || wins[0].tool == MAIN_TOOL_SAVE) {
        ssfn_dst.w = win->w - 8; if(ssfn_dst.w < 1) ssfn_dst.w = 1;
        ssfn_dst.h = win->h;
        ui_icon(win, win->w - 134 - 16, 30, ICON_SEARCH, 0);
        ui_input(win, win->w - 132, 29, 120, fsearch, wins[0].field == 7, 255, 0);
        ui_rect(win, 7, 51, win->w - 14, win->h - 102, theme[THEME_DARKER], theme[THEME_LIGHT]);
        ui_box(win, 8, 52, 18, 20, theme[THEME_LIGHT], theme[THEME_BG], theme[THEME_DARKER]);
        ssfn_dst.w = win->w - 284; if(ssfn_dst.w < 1) ssfn_dst.w = 1;
        ui_box(win, 26, 52, win->w - 314, 20, theme[selfield == 1 ? THEME_DARKER : THEME_LIGHT], theme[THEME_BG],
            theme[selfield == 1 ? THEME_LIGHT : THEME_DARKER]);
        ssfn_dst.fg = theme[THEME_LIGHTER];
        ui_text(win, 34, 53, lang[FILEOP_NAME]);
        ssfn_dst.fg = theme[THEME_DARKER];
        ui_text(win, 33, 52, lang[FILEOP_NAME]);
        ssfn_dst.w = win->w - 168; if(ssfn_dst.w < 1) ssfn_dst.w = 1;
        ui_box(win, win->w - 288, 52, 120, 20, theme[selfield == 2 ? THEME_DARKER : THEME_LIGHT], theme[THEME_BG],
            theme[selfield == 2 ? THEME_LIGHT : THEME_DARKER]);
        ssfn_dst.fg = theme[THEME_LIGHTER];
        ui_text(win, win->w - 280, 53, lang[FILEOP_SIZE]);
        ssfn_dst.fg = theme[THEME_DARKER];
        ui_text(win, win->w - 281, 52, lang[FILEOP_SIZE]);
        ssfn_dst.w = win->w - 9; if(ssfn_dst.w < 1) ssfn_dst.w = 1;
        ui_box(win, win->w - 168, 52, 159, 20, theme[selfield == 3 ? THEME_DARKER : THEME_LIGHT], theme[THEME_BG],
            theme[selfield == 3 ? THEME_LIGHT : THEME_DARKER]);
        ssfn_dst.fg = theme[THEME_LIGHTER];
        ui_text(win, win->w - 160, 53, lang[FILEOP_TIME]);
        ssfn_dst.fg = theme[THEME_DARKER];
        ui_text(win, win->w - 161, 52, lang[FILEOP_TIME]);
        ssfn_dst.w = win->w;
        ssfn_dst.fg = theme[THEME_FG];
        j = win->w * 60;
        switch(ordering >> 1) {
            case 0: ui_tri(win, win->w - 300, 60, ordering & 1); break;
            case 1: ui_tri(win, win->w - 180, 60, ordering & 1); break;
            case 2: ui_tri(win, win->w - 20, 60, ordering & 1); break;
        }
        ssfn_dst.y = 72;
        ssfn_dst.h = win->h - 51;
        for(i = scrollfiles; i < numfiles && ssfn_dst.y < ssfn_dst.h; i++, ssfn_dst.y += 16) {
            if(i == selfiles) {
                ui_box(win, 9, ssfn_dst.y, win->w - 18, 16, theme[THEME_SELBG], theme[THEME_SELBG], theme[THEME_SELBG]);
                ssfn_dst.fg = theme[THEME_SELFG];
            } else
                ssfn_dst.fg = theme[THEME_FG];
            ui_icon(win, 9, ssfn_dst.y, files[i].type ? ICON_FILE : ICON_FOLDER, 0);
            ssfn_dst.w = win->w - 284; if(ssfn_dst.w < 1) ssfn_dst.w = 1;
            ui_text(win, 30, ssfn_dst.y, files[i].name);
            sprintf(tmp,"%13" PRIu64,files[i].size);
            ssfn_dst.w = win->w - 168; if(ssfn_dst.w < 1) ssfn_dst.w = 1;
            ui_text(win, win->w - 280, ssfn_dst.y, tmp);
            ssfn_dst.w = win->w - 9; if(ssfn_dst.w < 1) ssfn_dst.w = 1;
            diff = now - files[i].time;
            if(diff < 120) strcpy(tmp, lang[FILEOP_NOW]); else
            if(diff < 3600) sprintf(tmp, lang[FILEOP_MSAGO], (int)(diff/60)); else
            if(diff < 7200) sprintf(tmp, lang[FILEOP_HAGO], (int)(diff/60)); else
            if(diff < 24*3600) sprintf(tmp, lang[FILEOP_HSAGO], (int)(diff/3600)); else
            if(diff < 48*3600) strcpy(tmp, lang[FILEOP_YESTERDAY]); else {
                lt = localtime(&files[i].time);
                if(files[i].time < 7*24*3600) strcpy(tmp, lang[FILEOP_WDAY0 + lt->tm_wday]); else
                    sprintf(tmp, "%04d-%02d-%02d", lt->tm_year+1900, lt->tm_mon+1, lt->tm_mday);
            }
            ui_text(win, win->w - 160, ssfn_dst.y, tmp);
        }
        ssfn_dst.fg = theme[THEME_FG];
        ssfn_dst.w = win->w;
        ssfn_dst.h = win->h;
        j = win->w / 4;
        if(!save) {
            if(!strrs[0]) sprintf(strrs, "U+%X", rs);
            if(!strre[0]) sprintf(strre, "U+%X", re);
            ui_text(win, 8, win->h - 44, lang[FILEOP_RANGE]);
            ui_input(win, 80, win->h - 44, 72, strrs, wins[0].field == 9, 15, 1);
            ui_input(win, 164, win->h - 44, 72, strre, wins[0].field == 10, 15, 2);
            ui_bool(win, 260, win->h - 44, lang[FILEOP_REPLACE], replace, wins[0].field == 11);
            ui_button(win, win->w - 12 - j, win->h - 44, j, lang[FILEOP_IMPORT], selfield == 4 ? 1 : 0, wins[0].field == 12);
        } else {
            ui_bool(win, 8, win->h - 44, "ASC", ascii, wins[0].field == 9);
            ui_bool(win, 64, win->h - 44, "gzip", zip, wins[0].field == 10);
            s = strrchr(filename, '.');
            if(s && !strcmp(s, ".gz")) { *s = 0; s = strrchr(filename, '.'); }
            if(!s) s = filename + strlen(filename);
            if(s != filename) memcpy(s, ascii ? ".asc" : ".sfn", 5);
            ui_input(win, 128, win->h - 44, win->w - j - 12 - 128 - 12, filename, wins[0].field == 11, 255, 0);
            ui_button(win, win->w - 12 - j, win->h - 44, j, lang[FILEOP_SAVE], selfield == 4 ? 3 : 2, wins[0].field == 12);
        }
    }
}

/**
 * On enter handler
 */
void ctrl_fileops_onenter(int save)
{
    int i, j = wins[0].w / 4;
    char *s;

    clkfiles = -1;
    if(wins[0].field == 6) {
        if(pathlen > 0) pathlen--;
        for(fn[0] = 0, i = 0; i < pathlen; i++)
            strcat(fn, path[i]);
        fileops_readdir(save);
        path[pathlen][strlen(path[pathlen]) - 1] = 0;
        for(selfiles = 0; selfiles < numfiles && strcmp(files[selfiles].name, path[pathlen]); selfiles++);
    }
    if(!save) {
        if(wins[0].field == 11) replace ^= 1;
        if(wins[0].field == 8 || wins[0].field == 12) {
            if(selfiles >= 0 && selfiles < numfiles) {
                for(fn[0] = 0, i = 0; i < pathlen; i++)
                    strcat(fn, path[i]);
                strcat(fn, files[selfiles].name);
                if(files[selfiles].type) {
                    ui_cursorwin(&wins[0], CURSOR_LOADING);
                    ui_button(&wins[0], wins[0].w - 12 - j, wins[0].h - 44, j, lang[FILEOP_IMPORT], 0, -1);
                    ui_flushwin(&wins[0], wins[0].w - 12 - j, wins[0].h - 44, j, 32);
                    if(sfn_load(fn, 0)) {
                        sfn_sanitize(-1);
                        strcpy(filename, files[selfiles].name);
                        wins[0].tool = MAIN_TOOL_GLYPHS;
                        wins[0].field = selfield = -1;
                        ui_updatetitle(0);
                    } else {
                        sprintf(fstatus, "libsfn: %s", lang[ERR_LOAD]);
                        errstatus = fstatus;
                    }
                    ui_cursorwin(&wins[0], CURSOR_PTR);
                } else {
                    strcat(path[pathlen++], files[selfiles].name);
                    strcat(fn, DIRSEPS);
                    fileops_readdir(save);
                    selfiles = 0;
                    wins[0].field = 8;
                }
            }
        }
    } else {
        if(wins[0].field == 9) { ascii ^= 1; if(ascii) zip = 0; }
        if(wins[0].field == 10) { zip ^= 1; if(zip) ascii = 0; }
        if(wins[0].field == 8 || wins[0].field == 12) {
            for(fn[0] = 0, i = 0; i < pathlen; i++)
                strcat(fn, path[i]);
            if(wins[0].field == 12 || (selfiles >= 0 && selfiles < numfiles && files[selfiles].type)) {
                if(wins[0].field == 8) strcpy(filename, files[selfiles].name);
                s = strrchr(filename, '.');
                if(s && !strcmp(s, ".gz")) { *s = 0; s = strrchr(filename, '.'); }
                if(!s) s = filename + strlen(filename);
                if(s != filename) memcpy(s, ascii ? ".asc" : ".sfn", 5);
                strcat(fn, filename);
                ui_cursorwin(&wins[0], CURSOR_LOADING);
                if(wins[0].tool == MAIN_TOOL_SAVE) {
                    ui_input(&wins[0], 128, wins[0].h - 44, wins[0].w - j - 12 - 128 - 12, filename, 0, 255, 0);
                    ui_button(&wins[0], wins[0].w - 12 - j, wins[0].h - 44, j, lang[FILEOP_SAVE], 2, -1);
                    ui_flushwin(&wins[0], 0, wins[0].h - 44, wins[0].w, 32);
                }
                sfn_sanitize(-1);
                if(sfn_save(fn, ascii, zip)) ui_updatetitle(0); else
                {
                    sprintf(fstatus, "libsfn: %s", lang[ERR_SAVE]);
                    errstatus = fstatus;
                }
                fileops_readdir(save);
                ui_cursorwin(&wins[0], CURSOR_PTR);
            } else {
                strcat(path[pathlen++], files[selfiles].name);
                strcat(fn, files[selfiles].name);
                strcat(fn, DIRSEPS);
                fileops_readdir(save);
                selfiles = 0;
                wins[0].field = 8;
            }
        }
    }
    ui_resizewin(&wins[0], wins[0].w, wins[0].h);
    ui_refreshwin(0, 0, 0, wins[0].w, wins[0].h);
    selfield = -1;
}

/**
 * On key handler
 */
void ctrl_fileops_onkey()
{
    int i, j;
    if(wins[0].field == 8) {
        switch(event.x) {
            case K_LEFT:
            case K_BACKSPC:
                wins[0].field = 6;
                ctrl_fileops_onenter(wins[0].tool == MTOOL_SAVE);
                wins[0].field = 8;
            break;
            case K_RIGHT: ctrl_fileops_onenter(wins[0].tool == MTOOL_SAVE); break;
            case K_UP:
                if(event.h & 1) { if(ordering > 0) ordering--; else ordering = 5; }
                else { if(selfiles > 0) selfiles--; }
            break;
            case K_DOWN:
                if(event.h & 1) { if(ordering < 5) ordering++; else ordering = 0; }
                else { if(selfiles + 1 < numfiles) selfiles++; }
            break;
            case K_PGUP:
                selfiles -= pagefiles - 1;
                if(selfiles < 0) selfiles = 0;
            break;
            case K_PGDN:
                selfiles += pagefiles - 1;
                if(selfiles >= numfiles) selfiles = numfiles - 1;
            break;
            case K_HOME: selfiles = 0; break;
            case K_END: selfiles = numfiles - 1; break;
            default:
                j = strlen((char*)&event.x);
                for(i = selfiles + 1; i < numfiles && memcmp(files[i].name, (char*)&event.x, j); i++);
                if(i < numfiles) selfiles = i;
                else {
                    for(i = 0; i < selfiles && memcmp(files[i].name, (char*)&event.x, j); i++);
                    if(i < selfiles) selfiles = i;
                }
            break;
        }
        ui_resizewin(&wins[0], wins[0].w, wins[0].h);
        ui_refreshwin(0, 0, 0, wins[0].w, wins[0].h);
    } else if(event.x == K_DOWN) {
        wins[0].field = 8;
        selfiles = 0;
        ui_resizewin(&wins[0], wins[0].w, wins[0].h);
        ui_refreshwin(0, 0, 0, wins[0].w, wins[0].h);
    }
    selfield = 0;
}

/**
 * On button press handler
 */
void ctrl_fileops_onbtnpress(int save)
{
    int i, j = wins[0].w / 4;
    selfield = 0; wins[0].field = -1;
    if(event.y >= 29 && event.y <= 49) {
        if(event.x >= wins[0].w - 132 && event.x <= wins[0].w - 8) wins[0].field = 7;
        else {
            for(i = 0; i < pathlen; i++)
                if(event.x >= pathX[i] - 3 && event.x <= pathX[i + 1] - 5) {
                    selfield = 4 + i; break;
                }
        }
    } else
    if(event.y > 51 && event.y < 73) {
        if(event.x >= 26 && event.x <= wins[0].w - 287) selfield = 1; else
        if(event.x >= wins[0].w - 288 && event.x <= wins[0].w - 167) selfield = 2; else
        if(event.x >= wins[0].w - 168 && event.x <= wins[0].w - 10) selfield = 3;
    } else
    if(event.y > 73 && event.y <= wins[0].h - 51) {
        wins[0].field = 8;
        if(event.w & 1) {
            selfiles = (event.y - 73) / 16 + scrollfiles;
            if(selfiles >= numfiles) selfiles = numfiles - 1;
            if(selfiles != clkfiles) clkfiles = selfiles;
            else ctrl_fileops_onenter(save);
        } else
        if(event.w & (1 << 3)) {
            if(scrollfiles > 0) scrollfiles--;
            if(selfiles > scrollfiles + pagefiles - 1) selfiles = scrollfiles + pagefiles - 1;
        } else
        if(event.w & (1 << 4)) {
            if(scrollfiles + pagefiles < numfiles) scrollfiles++;
            if(selfiles < scrollfiles) selfiles = scrollfiles;
        }
        ui_resizewin(&wins[0], wins[0].w, wins[0].h);
    } else
    if(event.y >= wins[0].h - 44 && event.y <= wins[0].h - 18) {
        if(!save) {
            if(event.x >= 80 && event.x <= 160) wins[0].field = 9; else
            if(event.x >= 164 && event.x <= 240) wins[0].field = 10; else
            if(event.x >= 260 && event.x <= wins[0].w - 16 - j) replace ^= 1;
        } else {
            if(event.x >= 8 && event.x <= 60) { ascii ^= 1; if(ascii) zip = 0; } else
            if(event.x >= 64 && event.x <= 124) { zip ^= 1; if(zip) ascii = 0; } else
            if(event.x >= 128 && event.x <= wins[0].w - 16 - j) wins[0].field = 11;
        }
        if(event.x >= wins[0].w - 12 - j && event.x <= wins[0].w - 12) selfield = 4;
    }
}

/**
 * On click (button release) handler
 */
void ctrl_fileops_onclick(int save)
{
    int i, j = wins[0].w / 4;
    if(event.y >= 29 && event.y <= 49) {
        if(event.x < wins[0].w - 132 && selfield >= 4 && event.x >= pathX[selfield-4] - 3 && event.x <= pathX[selfield-3] - 5) {
            pathlen = selfield - 3;
            for(fn[0] = 0, i = 0; i < pathlen; i++)
                strcat(fn, path[i]);
            fileops_readdir(save);
            path[pathlen][strlen(path[pathlen]) - 1] = 0;
        }
    } else
    if(event.y > 51 && event.y < 73) {
        if(event.x >= 26 && event.x <= wins[0].w - 287 && selfield == 1) {
            if(ordering == 0) ordering = 1; else ordering = 0;
        } else
        if(event.x >= wins[0].w - 288 && event.x <= wins[0].w - 167 && selfield == 2) {
            if(ordering == 2) ordering = 3; else ordering = 2;
        } else
        if(event.x >= wins[0].w - 168 && event.x <= wins[0].w - 10 && selfield == 3) {
            if(ordering == 4) ordering = 5; else ordering = 4;
        }
        fileops_readdir(save);
    } else
    if(event.y >= wins[0].h - 44 && event.y <= wins[0].h - 18) {
        if(event.x >= wins[0].w - 12 - j && event.x <= wins[0].w - 12 && selfield == 4) {
            wins[0].field = 12;
            ctrl_fileops_onenter(save);
            wins[0].field = 8;
        }
    }
    selfield = 0;
}

/**
 * Save modified font question window
 */
void view_dosave()
{
    ui_win_t *win = &wins[0];

    question_y = (win->h / 2) + 16;
    ssfn_dst.bg = 0;
    ui_text(win, (win->w - ui_textwidth(lang[FILEOP_DOSAVE])) / 2, question_y - 48, lang[FILEOP_DOSAVE]);
    ui_button(win, 20, question_y, (win->w - 80) / 2, lang[FILEOP_YES], selfield == 1, win->field == 6);
    ui_button(win, win->w / 2 + 20, question_y, (win->w - 80) / 2, lang[FILEOP_NO], selfield == 2 ? 3 : 2, win->field == 7);
}

/**
 * On enter handler
 */
void ctrl_dosave_onenter()
{
    if(wins[0].field != 7) {
        wins[0].tool = MAIN_TOOL_SAVE;
        wins[0].field = 11; selfield = -1;
        ui_resizewin(&wins[0], wins[0].w, wins[0].h);
        ui_refreshwin(0, 0, 0, wins[0].w, wins[0].h);
        ctrl_fileops_onenter(1);
    }
    modified = 0;
}

/**
 * On button press handler
 */
void ctrl_dosave_onbtnpress()
{
    selfield = 0;
    if(question_y && event.y >= question_y && event.y < question_y + 20)
        selfield = (event.x < wins[0].w / 2) ? 1 : 2;
}

/**
 * On click (button release) handler
 */
void ctrl_dosave_onclick()
{
    int x = wins[0].w / 2;
    if(question_y && event.y >= question_y && event.y < question_y + 20) {
        if(event.x < x && selfield == 1) { wins[0].field = 6; ctrl_dosave_onenter(); }
        if(event.x > x && selfield == 2) { wins[0].field = 7; ctrl_dosave_onenter(); }
    }
}

/**
 * Start new font question window
 */
void view_new()
{
    ui_win_t *win = &wins[0];

    question_y = (win->h / 2) + 16;
    ssfn_dst.bg = 0;
    ui_text(win, (win->w - ui_textwidth(lang[FILEOP_NEW])) / 2, question_y - 48, lang[FILEOP_NEW]);
    ui_button(win, 20, question_y, (win->w - 80) / 2, lang[FILEOP_YES], selfield == 1 ? 3 : 2, win->field == 6);
    ui_button(win, win->w / 2 + 20, question_y, (win->w - 80) / 2, lang[FILEOP_NO], selfield == 2, win->field == 7);
}

/**
 * On enter handler
 */
void ctrl_new_onenter()
{
    int i;
    if(wins[0].field != 7) {
        for(i=1; i < numwin; i++)
            if(wins[i].winid)
                ui_closewin(i);
        numwin = 1;
        sfn_free(); sfn_init(ui_pb);
    }
    wins[0].tool = MAIN_TOOL_GLYPHS;
    wins[0].field = selfield = -1;
}

/**
 * On key handler
 */
void ctrl_new_onkey()
{
    if(event.x == K_BACKSPC) {
        wins[0].tool = MAIN_TOOL_GLYPHS;
        wins[0].field = selfield = -1;
    }
}

/**
 * On button press handler
 */
void ctrl_new_onbtnpress()
{
    selfield = 0;
    if(question_y && event.y >= question_y && event.y < question_y + 20)
        selfield = (event.x < wins[0].w / 2) ? 1 : 2;
}

/**
 * On click (button release) handler
 */
void ctrl_new_onclick()
{
    int x = wins[0].w / 2;
    if(question_y && event.y >= question_y && event.y < question_y + 20) {
        if(event.x < x && selfield == 1) { wins[0].field = 6; ctrl_new_onenter(); }
        if(event.x > x && selfield == 2) { wins[0].field = 7; ctrl_new_onenter(); }
    }
}
