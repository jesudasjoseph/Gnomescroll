/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#include <t_map/glsl/structs.hpp>
#include <t_map/glsl/settings.hpp>
#include <t_map/common/map_element.hpp>
#include <t_map/t_map_class.hpp>
#include <physics/quadrant.hpp>

namespace t_map
{


/*
    Delete VBOs for map chunks outside of a given radius
    Do not update VBOs for map chunks outside of radius
    Put VBO refreshes on a que and sort by distance, recency and viewing fulstrum
    Thead VBO generation
    Double buffer VBOs
*/

/*
    Improve viewing fulstrum test
*/

class MapVBO
{
    private:
        static const int MAP_VBO_STARTING_SIZE = 128;
    public:
        float xpos;
        float ypos;
        float xoff;
        float yoff;
        float wxoff;
        float wyoff;
        int vnum;
        int vnum_max;
        struct Vertex* v_list;
        struct VertexBackup* v_list_backup; //backup list
        int _v_num[4];
        int _v_offset[4];
        int vertex_num[6];
        int vertex_offset[6];
        //int vertex_num_array[7][16];   //for each column, every 8 z
        int voff_array[6][8+1]; //levels
        GLuint vbo_id;

    MapVBO(class MapChunk* m) :
        vnum(0), vnum_max(0), v_list(NULL), v_list_backup(NULL), vbo_id(0)
    {
        this->xpos = m->xpos + 8.0f;
        this->ypos = m->ypos + 8.0f;
        this->xoff = m->xpos;
        this->yoff = m->ypos;
        this->wxoff = 0.0f;
        this->wyoff = 0.0f;
        memset(this->_v_num, 0, sizeof(this->_v_num));
        memset(this->_v_offset, 0, sizeof(this->_v_offset));
        memset(this->vertex_num, 0, sizeof(this->vertex_num));
        memset(this->vertex_offset, 0, sizeof(this->vertex_offset));
        memset(this->voff_array, 0, sizeof(this->voff_array));
    }

    ~MapVBO()
    {
        if (this->vbo_id != 0) glDeleteBuffers(1, &this->vbo_id);
        delete[] v_list;
        delete[] v_list_backup;
    }
};

const int VBO_LIST_SIZE = 1024; //max number of VBOS that can have loaded VBOs


class VBOMap
{
    private:
        const static int CHUNK_START_DISTANCE2 = (128+16)*(128+16);
        const static int CHUNK_IGNORE_DISTANCE2 = (128+64)*(128+64);
    public:
        class MapVBO** vbo_array;
        class TerrainMap* map;

    VBOMap(class TerrainMap* _map)
    {
        this->map = _map;
        this->vbo_array = new MapVBO*[ MAP_CHUNK_XDIM*MAP_CHUNK_YDIM ];
        for (int i=0; i<MAP_CHUNK_XDIM*MAP_CHUNK_YDIM; i++) vbo_array[i] = NULL;
    }

    ~VBOMap()
    {
        for (int i=0; i<MAP_CHUNK_XDIM*MAP_CHUNK_YDIM; i++) if (vbo_array[i] != NULL) delete vbo_array[i];
        delete[] vbo_array;
    }

    #define MAP_VBO_CULLING 1
    #define MAP_VBO_CULLING_MAP_CHUNK_NULL 1 //delete vbo if map chunk is null

    //update all VBOs that need updating

/*
    Dont load chunk until its neightbors are loaded!
*/
    void update_map()
    {
    #if MAP_VBO_CULLING
        int cx = current_camera_position.x;
        int cy = current_camera_position.y;
    #endif

        class MapChunk* m;
        int min_distance2 = 0x7FFFFFF;
        int _i = -1;
        int _j = -1;

        //find closest chunk to update
        for (int i=0; i<MAP_CHUNK_XDIM; i++)
        for (int j=0; j<MAP_CHUNK_YDIM; j++)
        {
            const int index = j*MAP_CHUNK_XDIM + i;
            m = map->chunk[index];

        #if MAP_VBO_CULLING_MAP_CHUNK_NULL

            if (m == NULL)
            {
                if (vbo_array[index] != NULL)
                {
                    delete vbo_array[index];
                    vbo_array[index] = NULL;
                }
                continue;
            }
        #else
            if (m == NULL) continue; //can speed up by maintain list of chunks
        #endif

            int x = cx - quadrant_translate_i(cx, 16*i+8);
            int y = cy - quadrant_translate_i(cy, 16*j+8);
            int distance2 = x*x+y*y;

        #if MAP_VBO_CULLING
            if (distance2 >= CHUNK_IGNORE_DISTANCE2)
            {
                if (vbo_array[index] != NULL)
                {
                    delete vbo_array[index];
                    vbo_array[index] = NULL;
                    m->needs_update = false;    //so it gets recreated if in range
                    continue;
                }
            }
        #else
            if (distance2 >= CHUNK_IGNORE_DISTANCE2)
                continue;
        #endif

            if (distance2 > CHUNK_START_DISTANCE2)
                continue;
            //if map is loaded but VBO is not loaded, set update to true
            if (m != NULL && vbo_array[index] == NULL)
            {
                m->needs_update = true;
                vbo_array[index] = new MapVBO(m);
                //printf("Warning: map chunk is loaded and VBO does not exist, but needs_update is false\n");
            }

            if (!m->needs_update)
                continue;

            if (distance2 < min_distance2)
            {
                min_distance2 = distance2;
                _i = i;
                _j = j;
            }

        }
        //update if any chunks in range need updating
        if (_i != -1)
        {
            const int index = _j*MAP_CHUNK_XDIM + _i;
            //printf("updating index: %i \n", index);

            m = map->chunk[index];

            m->needs_update = false; //reset flag

            GS_ASSERT(vbo_array[index] != NULL);
            //if (vbo_array[index] == NULL) vbo_array[index] = new MapVBO(m);
            //printf("updating vbo: %i %i \n", i, j);

            if (T_MAP_BACKUP_SHADER == 0)
            {
                update_vbo(_i, _j);
            }
            else
            {
                update_vbo_compatibility(_i, _j);
            }
            return;
        }

    }

    //for VBO projection
    void load_all()
    {
        for (int i=0; i<MAP_CHUNK_XDIM; i++)
        for (int j=0; j<MAP_CHUNK_YDIM; j++)
        {
            const int index = j*MAP_CHUNK_XDIM + i;
            class MapChunk* m = map->chunk[index];
            if (m == NULL) continue; //can speed up by maintain list of chunks
            if (!m->needs_update) continue;
            m->needs_update = false; //reset flag
            if (vbo_array[index] == NULL) vbo_array[index] = new MapVBO(m);
            update_vbo(i, j);
        }

    }

    void update_vbo(int i, int j);
    void update_vbo_compatibility(int i, int j);

    void prep_draw();
    void sort_draw();

    void draw_map();
    void draw_map_compatibility();

    void prep_frustrum_vertices();
};

}   // t_map
