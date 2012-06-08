#pragma once 

#include <voxel/common.h>
#include <physics/affine.hpp>
#include <voxel/voxel_hitscan.hpp>

#if DC_CLIENT
#include <voxel/voxel_render.hpp>
#endif

class Voxel_volume
{
    public:

    struct Affine local_matrix;
    struct Affine world_matrix;

    struct Affine* parent_world_matrix;

    #if DC_CLIENT
    Voxel_vertex_list vvl;
    #endif

    int id;
    bool draw;
    bool hitscan;

    float scale;    //size of voxels
    //bounding sphere
    float radius;   //update when changing scale

    int xdim,ydim,zdim;
    //int _xdim,_ydim,_zdim;
    Voxel* voxel;
    int index1;
    int index12;
    unsigned int index_max;

    float hdx,hdy,hdz;  //half of width, height, depth as floats

    bool needs_vbo_update;
    bool damaged;
    
    struct Voxel_hitscan_element vhe;

    int hitscan_test(float x, float y, float z, float vx, float vy, float vz, float r2,  int voxel[3]);

    //inline int voxel_ray_cast(float x0,float y0,float z0, float _dfx,float _dfy,float _dfz, float max_l, float* distance, int* collision) __attribute((always_inline));
    //inline struct Vec3 get_center() __attribute((always_inline));

    inline int voxel_ray_cast(float x0,float y0,float z0, float _dfx,float _dfy,float _dfz, float max_l, float* distance, int* collision);
    
    inline void get_center(float *v)
    {
        v[0] = world_matrix.v[3].f[0];
        v[1] = world_matrix.v[3].f[1];
        v[2] = world_matrix.v[3].f[2];
    }
    inline struct Vec3 get_center(){ return world_matrix.v[3]; }

    #if DC_CLIENT
    class Voxel_render_list* voxel_render_list;
    int voxel_render_list_id;
    void update_vertex_list();
    #endif

    void set_color(int x, int y, int z, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
    void set_color(int x, int y, int z, unsigned char rgba[4]);

    //hitscan registration
    Voxel_hitscan_list* voxel_hitscan_list;

    void set(int x, int y, int z, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
    void set(int x, int y, int z, Voxel* v);

    void set_parameters(int xdim, int ydim, int zdim, float scale);

    void draw_bounding_box();
    int point_collision_test(Vec3 p, int vxl[3]);

    void init(int xdim, int ydim, int zdim, float scale);
    void set_hitscan_properties(short entity_id, short entity_type, short part_id);

    Voxel_volume(int xdim, int ydim, int zdim, float scale);
    Voxel_volume();

    ~Voxel_volume();

    private:
    #if DC_CLIENT
    inline void push_voxel_quad(Voxel_vertex* scratch, int* index, int x, int y, int z, int side, float* vset, float ox,float oy,float oz) __attribute((always_inline));
    #endif

    inline Voxel* get(unsigned int x, unsigned int y, unsigned int z) __attribute((always_inline));

    inline unsigned int get_as_int(unsigned int x, unsigned int y, unsigned int z) __attribute((always_inline));

/*
    Tests whether a voxel is occupied, for AO
*/
    inline unsigned int _test_occludes_safe(int x, int y, int z) __attribute((always_inline));
    inline unsigned int _test_occludes_safe(int x, int y, int z, int vxl[3]) __attribute((always_inline));
    inline void _set(unsigned int x, unsigned int y, unsigned int z, Voxel* v) __attribute((always_inline));
    inline void _set(unsigned int x, unsigned int y, unsigned int z, unsigned char r, unsigned char g, unsigned char b, unsigned char a) __attribute((always_inline));

};

//void destroy_object_voxel(int id, int type, int part, const int voxel[3], int radius);

void init_voxel_volume();
void teardown_voxel_volume();