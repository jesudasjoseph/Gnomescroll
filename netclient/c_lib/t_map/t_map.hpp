#pragma once


#include <stdio.h>
#include <stdlib.h>

#include <compat.h>
#include <compat_gl.h>

#include <c_lib/common/functional.h>
#include <c_lib/template/net.hpp>

/*
#include "t_vbo.h"
#include "t_properties.h"
*/

/*
#ifndef  t_properties
#include "t_properties.h"
#endif

#ifndef  t_vbo
#include "t_vbo.h"
#endif
*/

#define vm_map_dim 64 //number of map chunks in x/y
#define vm_chunk_size 8
#define vm_column_max 16
#define vm_chunk_voxel_size (vm_chunk_size * vm_chunk_size * vm_chunk_size)

#define xmax (vm_map_dim    * vm_chunk_size)
#define ymax (vm_map_dim    * vm_chunk_size)
#define zmax (vm_column_max * vm_chunk_size)

struct VBO {
        int v_num;
        struct Vertex* v_list;
        int v_list_max_size;

        #ifdef DC_CLIENT
            GLuint VBO_id;
        #else
            int VBO_id;
        #endif
        int _v_num[4];       //parameters for draw pass
        int _v_offset[4];

        //draw pass 0, normal quads
        //draw pass 1, normal quads
        //draw pass 1, normal quads
        //draw pass 1, normal quads
};

//need 3 types of transparency

struct vm_chunk {
    unsigned short voxel[512];
    unsigned char hash2[8*8*8];   //hash result for 2ary infinite texture
    unsigned char hash3[8*8*8];   //hash result for 3ary infinite texture
    int x_off, y_off, z_off; //lower corner of chunk
    unsigned int local_version;
    unsigned int server_version;
    unsigned int vbo_needs_update;
    unsigned int requested;
};

struct vm_column {
    int x_off, y_off;
    struct vm_chunk* chunk[vm_column_max];
    unsigned int local_version;
    unsigned int server_version;
    //vm_column_history history;
    //unsigned int vbo_needs_update;
    //unsigned int vbo_loaded;
    //unsigned int empty; // has blocks
    unsigned int flag;
    struct VBO vbo;
};

struct vm_map {
    struct vm_column column[vm_map_dim*vm_map_dim];
};

//state flags

//flag setting

//int __inline set_flag(vm_column* c, unsigned int flag);
//int __inline get_flag(vm_column* c, unsigned int flag);

//functions
extern struct vm_map map;

int _init_t_map();
int _set(int x, int y, int z, int value);
int _get(int x, int y, int z);
int _clear();

int _set_server_version(int x,int y,int z, int server_version);

struct vm_map* _get_map();
struct vm_chunk* _get_chunk(int xoff, int yoff, int zoff);

int _get_highest_open_block(int x, int y, int n);
int _get_lowest_open_block(int x, int y, int n);

int _get_highest_solid_block(int x, int y);
int _get_lowest_solid_block(int x, int y);

int block_sphere(float x, float y, float z, float radius, int* blocks, int max_blocks);

/*
#endif

//inline functions
#ifndef T_MAP_INLINE
#define T_MAP_INLINE
*/


#define VBO_loaded 1
#define VBO_needs_update 2
#define VBO_has_blocks 4
#define VBO_drawn 8

//#define set_flag(c,flag,value) value ? c->flag | flag : c->flag & ~flag

//#define set_flag(c, f, value)  ({ (c) -> flag = (value) ? (c) -> flag | f : (c) -> flag & ~ (f); })
//#define get_flag(c, f)  ({c -> flag & f})


static inline void set_flag(struct vm_column* c, int flag, int value) {
    if(value) {
        c->flag = c->flag | flag;
    } else {
        c->flag = c->flag & ~flag;
    }
}

static inline int flag_is_true(struct vm_column* c, int flag) {
    if((c->flag & flag) == 0) { return 0; } else {return 1;}
}

static inline int flag_is_false(struct vm_column* c, int flag) {
    if((c->flag & flag) == 0) { return 1; } else {return 0;}
}

//void set_flag(struct vm_column* c, unsigned int flag, int value);
//int get_flag(struct vm_column* c, unsigned int flag);

//extern void set_flag(struct vm_column* c, unsigned int flag, int value);
//extern int get_flag(struct vm_column* c, unsigned int flag);


/* Network */

/* move these, along with the handler */
class block_StoC: public FixedSizeNetPacketToClient<block_StoC>
{
    public:

        int x,y,z;
        int val;
        
        inline void packet(char* buff, int* buff_n, bool pack) 
        {
            pack_u16(&x, buff, buff_n, pack);
            pack_u16(&y, buff, buff_n, pack);
            pack_u16(&z, buff, buff_n, pack);
            pack_u16(&val, buff, buff_n, pack);
        }

        inline void handle();
        
        block_StoC(int x, int y, int z, int val): x(x), y(y), z(z), val(val) {}
        
        block_StoC(): x(0), y(0), z(0), val(0) {}
};

