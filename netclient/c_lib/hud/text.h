#pragma once

#include <compat_gl.h>
#include <stdio.h>

int init_text();

SDL_Surface* _create_text_surface(char* text,int r,int g,int b,int a);
int _draw_text_surface(SDL_Surface* surface, int x, int y);
int _free_text_surface(SDL_Surface* surface);

int _draw_text(char* text, float x, float y, float height, float width, float depth, int r, int g, int b, int a);
