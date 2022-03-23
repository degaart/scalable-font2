/*
 * sfntest7.cpp
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
 * @brief testing Scalable Screen Font normal renderer C++ wrapper
 *
 */

#include <iostream>
#include <stdio.h>
#include <stdint.h>
#define SSFN_IMPLEMENTATION
#define SSFN_DEBUGGLYPH
#include "../ssfn.h"

#include <SDL.h>

/**
 * Load a font
 */
unsigned char *load_file(char *filename, int *size)
{
    unsigned char *fontdata = NULL;
    FILE *f;

    f = fopen(filename, "rb");
    if(!f) { fprintf(stderr,"unable to load %s\n", filename); exit(3); }
    *size = 0;
    fseek(f, 0, SEEK_END);
    *size = (int)ftell(f);
    fseek(f, 0, SEEK_SET);
    if(!*size) { fprintf(stderr,"unable to load %s\n", filename); exit(3); }
    fontdata = (unsigned char*)malloc(*size);
    if(!fontdata) { fprintf(stderr,"memory allocation error\n"); exit(2); }
    fread(fontdata, *size, 1, f);
    fclose(f);
    return fontdata;
}

/**
 * testing the SSFN library (normal renderer)
 */
void do_test(SDL_Surface *screen, char *fontfn)
{
    std::string str = "a";
    int err, size;
    unsigned char *sfn;
    SSFN::Font *font = new SSFN::Font();
    ssfn_buf_t buf;

    int x,y;

    /* initialize the normal renderer */
    buf.ptr = (uint8_t*)screen->pixels;
    buf.p = screen->pitch;
    buf.w = screen->w;
    buf.h = screen->h;
    buf.fg = 0xFF202020;
    buf.bg = 0;
    buf.x = 4; buf.y = 48;

    /* load and select a font */
    sfn = load_file(fontfn ? fontfn : (char*)"../fonts/VeraR.sfn", &size);
    err = font->Load(sfn);
    if(err != SSFN_OK) { fprintf(stderr, "ssfn load error: err=%d ", err); std::cerr << font->ErrorStr(err) << "\n"; exit(2); }

    err = font->Select(SSFN_FAMILY_ANY, nullptr, SSFN_STYLE_REGULAR | SSFN_STYLE_UNDERLINE, 20);
    if(err != SSFN_OK) { fprintf(stderr, "ssfn select error: err=%d ", err); std::cerr << font->ErrorStr(err) << "\n"; exit(2); }

    /* test the const char* variant */
    err = font->Render(&buf, "i");
    if(err < 0) { fprintf(stderr, "ssfn render char* error: err=%d ", err); std::cerr << font->ErrorStr(err) << "\n"; exit(2); }

    /* test the std::string variant */
    err = font->Render(&buf, str);
    if(err < 0) { fprintf(stderr, "ssfn render std::string error: err=%d ", err); std::cerr << font->ErrorStr(err) << "\n"; exit(2); }

    /* magnify */
    for(y = 0; y < 64; y++) {
        for(x = 0; x < 128; x++) {
            ((uint32_t*)(screen->pixels))[(128+4*y+0)*screen->pitch/4+(4*x+0)] =
            ((uint32_t*)(screen->pixels))[(128+4*y+0)*screen->pitch/4+(4*x+1)] =
            ((uint32_t*)(screen->pixels))[(128+4*y+0)*screen->pitch/4+(4*x+2)] =
            ((uint32_t*)(screen->pixels))[(128+4*y+0)*screen->pitch/4+(4*x+3)] =
            ((uint32_t*)(screen->pixels))[(128+4*y+1)*screen->pitch/4+(4*x+0)] =
            ((uint32_t*)(screen->pixels))[(128+4*y+1)*screen->pitch/4+(4*x+1)] =
            ((uint32_t*)(screen->pixels))[(128+4*y+1)*screen->pitch/4+(4*x+2)] =
            ((uint32_t*)(screen->pixels))[(128+4*y+1)*screen->pitch/4+(4*x+3)] =
            ((uint32_t*)(screen->pixels))[(128+4*y+2)*screen->pitch/4+(4*x+0)] =
            ((uint32_t*)(screen->pixels))[(128+4*y+2)*screen->pitch/4+(4*x+1)] =
            ((uint32_t*)(screen->pixels))[(128+4*y+2)*screen->pitch/4+(4*x+2)] =
            ((uint32_t*)(screen->pixels))[(128+4*y+2)*screen->pitch/4+(4*x+3)] =
            ((uint32_t*)(screen->pixels))[(128+4*y+3)*screen->pitch/4+(4*x+0)] =
            ((uint32_t*)(screen->pixels))[(128+4*y+3)*screen->pitch/4+(4*x+1)] =
            ((uint32_t*)(screen->pixels))[(128+4*y+3)*screen->pitch/4+(4*x+2)] =
            ((uint32_t*)(screen->pixels))[(128+4*y+3)*screen->pitch/4+(4*x+3)] =
                ((uint32_t*)(screen->pixels))[y*screen->pitch/4+(x)];
        }
    }

    printf("Memory allocated: %d %d\n", font->Mem(), (int)sizeof(ssfn_t));
    delete font;
    free(sfn);
}

/**
 * Main procedure
 */
int main(int argc __attribute__((unused)), char **argv)
{
    SDL_Window *window;
    SDL_Surface *screen;
    SDL_Event event;

    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS)) {
        fprintf(stderr,"SDL error %s\n", SDL_GetError());
        return 2;
    }

    window = SDL_CreateWindow("SSFN normal renderer bitmap font test",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,800,600,0);
    screen = SDL_GetWindowSurface(window);
    memset(screen->pixels, 0xF8, screen->pitch*screen->h);

    do_test(screen, argv[1]);

    do{ SDL_UpdateWindowSurface(window); SDL_Delay(10); } while(SDL_WaitEvent(&event) && event.type != SDL_QUIT &&
        event.type != SDL_MOUSEBUTTONDOWN && event.type != SDL_KEYDOWN);

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
