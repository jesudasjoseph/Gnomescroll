#include <SDL.h>
#include <SDL_opengl.h>

#include <SDL_image.h> //temp?

#include <stdio.h>


#ifdef linux
    //do nothing
#elif _WIN32
    #include <windows.h>
    #include <GL/GLee.h>
#else
    //mac
#endif

#ifndef t_map
    #include "t_map.h"
#endif



#ifndef t_vbo
#define t_vbo


int _test3(int x, int y, int z);


    struct Vertex {
        float x,y,z;
        float tx,ty;
        unsigned char r,g,b,a;
    };

    struct VBO {
        int v_num;
        struct Vertex* v_list;
        int VBO_id;
    };

int _init_draw_terrain();

int _create_vbo(struct VBO* q_VBO, struct Vertex* vlist, int v_num);
int _delete_vbo(struct VBO* q_VBO);

int _start_vbo_draw();
int _draw_quad_vbo(struct VBO* q_VBO);
int _end_vbo_draw();

#endif
