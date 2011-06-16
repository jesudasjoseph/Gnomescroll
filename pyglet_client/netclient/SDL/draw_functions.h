#include <SDL.h>
#include <SDL_opengl.h>

#include <stdio.h>

/*
cdef struct Vertex:
    float x,y,z
    float tx,ty
    unsigned char r,g,b,a #for packing

cdef struct Quad:
    Vertex vertex[4]
*/

struct Vertex {
    float x,y,z;
    float tx,ty;
    unsigned char r,g,b,a;
};

struct Quad {
  struct Vertex vertex[4];
};

int _draw_line(int r, int g,int b, float x0, float y0, float z0, float x1, float y1, float z1);
int _draw_point(int r, int g,int b, float x0, float y0, float z0);
int _blit_sprite(int tex, float x0, float y0, float x1, float y1, float z);

int _bind_VBO(struct Quad* quad_list, int v_num);
