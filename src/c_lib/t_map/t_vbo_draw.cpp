#include "t_vbo_draw.hpp"

#include <t_map/t_vbo_class.hpp>

#include <t_map/glsl/shader.hpp>
#include <t_map/glsl/texture.hpp>

#include <camera/camera.hpp>
#include <camera/fulstrum_test.hpp>

#include <t_map/glsl/settings.hpp>


#include <common/qsort.h>

#include <physics/quadrant.hpp>

namespace t_map
{

static const int MAX_DRAWN_VBO = 1024;  //this should not be hardcoded; will piss someone off

static int draw_vbo_n;
//static struct Map_vbo* draw_vbo_array[MAX_DRAWN_VBO];

int vbo_frustrum[32*32*2];  //start and end chunk index
//int vbo_frustrum_bottom[32*32];
int vbo_vertex_frustrum[32*32*2]; //start vertex and end vertex

struct _VBO_DRAW_STRUCT
{
    class Map_vbo* map_vbo;
    float distance;
    int vertex_start; //start vertex for drawing
    int vertex_end; //end vertex for drawing
};

static struct _VBO_DRAW_STRUCT* draw_vbo_array;

void vbo_draw_init()
{
    draw_vbo_array = (_VBO_DRAW_STRUCT*) malloc(MAX_DRAWN_VBO * sizeof(_VBO_DRAW_STRUCT));
}

void vbo_draw_end()
{
    free(draw_vbo_array);
}


/*
    Do distance check
*/
bool chunk_render_check( float x, float y)
{
    //static const float dist2 = CAMERA_VIEW_DISTANCE*CAMERA_VIEW_DISTANCE;
    static const float dist2 = CAMERA_VIEW_DISTANCE_SQUARED;

    const float cx = current_camera_position.x;
    const float cy = current_camera_position.y;

    x = quadrant_translate_f(cx, x);
    y = quadrant_translate_f(cy, y);
    
    float dx = cx - x;
    float dy = cy - y;

    return (dx*dx + dy*dy > dist2) ? false : true;
}

/*
    Use frustrum test to 8 block resolution because have to render partial chunks
*/

int _culled = 0;
int _drawn = 0;
int _edge = 0;
int _total = 0;
int _cached = 0;

void Vbo_map::set_frustrum_column(int _i, int _j, float x, float y)
{

    int index = _j*MAP_CHUNK_XDIM + _i; // 32*j + i

    //int lowest = 0;
    //int highest = 128/8;

    if(vbo_frustrum[2*index+0] != -1) 
    {
        _cached ++;
        return;
    }

    int i_min;
    for(i_min=0; i_min < 8; i_min++)
    {
        float z = 16*i_min;

        if(point_fulstrum_test_map(x,y,z) == true) break;
    }

    if(i_min == 8)
    {
        if(point_fulstrum_test_map(x,y, 16*8) == true) 
        {
            //only top chunk is visible
            vbo_frustrum[2*index+0] = 0;
            vbo_frustrum[2*index+1] = 0;
            _edge++;
            return;
        }

        //printf("culled column: %f %f \n", x, y);


        vbo_frustrum[2*index+0] = 0;
        vbo_frustrum[2*index+1] = 0;
        _culled++;
        return;
    }

    int i_max;
    for(i_max=8; i_max >= i_min; i_max--)
    {
        float z = 16*i_max;
        if(point_fulstrum_test_map(x,y,z) == true) break;
    }

    if(i_max == 0)
    {
        //i_min equals zero, only lowest chunk is visible
        vbo_frustrum[2*index+0] = 0;
        vbo_frustrum[2*index+1] = 0;
        _edge++;
        return;
    }

    if(i_max == i_min) //colomn is 
    {
        //single chunk to render?  Only passes fulstrum test for 1 point
        vbo_frustrum[2*index+0] = 0;
        vbo_frustrum[2*index+1] = 0;
        _edge++;
        return;
    }

    vbo_frustrum[2*index+0] = i_min;
    vbo_frustrum[2*index+1] = i_max;

    _drawn++;
}

void Vbo_map::set_frustrum_vertex(int i, int j)
{

}

void Vbo_map::prep_frustrum()
{
    //memset(vbo_frustrum, -1, 32*32*2);
    for(int i=0; i <32*32*2; i++) vbo_frustrum[i] = -1;

    _culled = 0;
    _drawn = 0;
    _edge = 0;
    _total = 0;
    _cached = 0;

    for(int i=0; i<MAP_CHUNK_XDIM; i++) {
    for(int j=0; j<MAP_CHUNK_YDIM; j++) {
        struct Map_vbo* col = vbo_array[j*MAP_CHUNK_XDIM + i ];

        if(col == NULL || col->vnum == 0) continue;

        //col->wxoff = quadrant_translate_f(cx, col->xoff);
        //col->wyoff = quadrant_translate_f(cy, col->yoff);

        if( chunk_render_check( col->wxoff+8.0, col->wyoff+8.0) )
        {
            _total++;

            Vbo_map::set_frustrum_column(i,       j,            col->wxoff, col->wyoff);
            Vbo_map::set_frustrum_column((i+1)%32,  j,          col->wxoff+16.0, col->wyoff);
            Vbo_map::set_frustrum_column(i,         (j+1)%32,   col->wxoff, col->wyoff+16.0);
            Vbo_map::set_frustrum_column((i+1)%32,  (j+1)%32,   col->wxoff+16.0, col->wyoff+16.0);
        }   

    }}

    //printf("culled= %i drawn= %i edge= %i cached= %i total= %i \n", _culled, _drawn, _edge, _cached, _total);

}

void Vbo_map::prep_draw()
{
    struct Map_vbo* col;

    const float cx = current_camera_position.x;
    const float cy = current_camera_position.y;
    ASSERT_BOXED_POINT(cx);
    ASSERT_BOXED_POINT(cy);
    
/*
    for(int i=0; i<map->xchunk_dim; i++)
    for(int j=0; j<map->ychunk_dim; j++)
    {
        col = vbo_array[j*xchunk_dim + i ];
        if(col == NULL) continue;

        col->wxoff = quadrant_translate_f(cx, col->xoff);
        col->wyoff = quadrant_translate_f(cy, col->yoff);

    }
*/
    int c_drawn, c_pruned;
    c_drawn=0; c_pruned=0;
    //start_vbo_draw();
    draw_vbo_n = 0;
    //printf("Start Map Draw\n");
    for(int i=0; i<MAP_CHUNK_XDIM; i++) {
    for(int j=0; j<MAP_CHUNK_YDIM; j++) {
        col = vbo_array[j*MAP_CHUNK_XDIM + i ];

        if(col == NULL || col->vnum == 0) continue;

        col->wxoff = quadrant_translate_f(cx, col->xoff);
        col->wyoff = quadrant_translate_f(cy, col->yoff);

        //if( chunk_render_check( col->wxoff+8.0, col->wyoff+8.0) && xy_circle_fulstrum_test( col->wxoff+8.0, col->wyoff+8.0, 32.0) )
        if( chunk_render_check( col->wxoff+8.0, col->wyoff+8.0) )
        {
            c_drawn++; 
            /*
                Fulstrum culling
            */
            draw_vbo_array[draw_vbo_n].map_vbo = col;
            draw_vbo_n++;
            
            if(draw_vbo_n == MAX_DRAWN_VBO)
            {
                printf("Vbo_map::prep_draw(), ERROR, draw_vbo == MAX_DRAWN_VBO \n");
                return;
            }
        }
        else
        {
            c_pruned++;
        }
    }}
    //printf("drawn: %i pruned: %i \n",  c_drawn, c_pruned);
}

void Vbo_map::sort_draw()
{
    const float cx = current_camera_position.x;
    const float cy = current_camera_position.y;
    ASSERT_BOXED_POINT(cx);
    ASSERT_BOXED_POINT(cy);

    for(int i=0; i<draw_vbo_n; i++ )
    {
        class Map_vbo* v = draw_vbo_array[i].map_vbo;

        float dx = (v->wxoff - cx);
        float dy = (v->wyoff - cy);

        dx = quadrant_translate_f(cx, dx);
        dy = quadrant_translate_f(cx, dy);

        draw_vbo_array[i].distance = dx*dx + dy*dy; //set this
    }

  #define _VBO_DRAW_STRUCT_lt(a,b) ((a)->distance < (b)->distance)
  QSORT(struct  _VBO_DRAW_STRUCT, draw_vbo_array, draw_vbo_n, _VBO_DRAW_STRUCT_lt);

  //if(draw_vbo_n > 10) draw_vbo_n = 10;
}

//float _normal_array[3*6];
float _chunk_position[3] = {0};

void Vbo_map::draw_map() 
{
    if(T_MAP_BACKUP_SHADER != 0)
    {
        draw_map_comptability();
        return;
    }

    prep_draw();
    sort_draw();
    prep_frustrum();

    //GL_ASSERT(GL_TEXTURE_2D, true);
    GL_ASSERT(GL_DEPTH_TEST, true);

    //glShadeModel(GL_FLAT);
    glShadeModel(GL_SMOOTH);

    glEnable(GL_CULL_FACE);
    glDisable(GL_TEXTURE_2D);

    glUseProgramObjectARB(map_shader[0]);


    glColor3ub(255,255,255);

    glBindTexture( GL_TEXTURE_2D_ARRAY, terrain_map_glsl );


    glEnableVertexAttribArray(map_Vertex);
    glEnableVertexAttribArray(map_TexCoord);
    glEnableVertexAttribArray(map_RGB);
    glEnableVertexAttribArray(map_LightMatrix);

    struct Map_vbo* vbo;

    //glUniform3fv(map_NormalArray , 6, (GLfloat*) _normal_array );

    float modelview[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, modelview);

    glPushMatrix(); //save matrix
    //glPushMatrix();

    for(int i=0;i<draw_vbo_n;i++)
    {
        vbo = draw_vbo_array[i].map_vbo;

        if(vbo->_v_num[0] == 0)
        {
            //printf("t_vbo_draw.cpp:117 no blocks\n");
            continue; 
        } 


        glLoadMatrixf(modelview);
        glTranslatef(vbo->wxoff, vbo->wyoff, 0.0f);

        glBindBuffer(GL_ARRAY_BUFFER, vbo->vbo_id);

        //glUniform3f(map_ChunkPosition, vbo->wxoff, vbo->wyoff, 0.0f);

        //translation
        //_modelview[3*4+0] = modelview[3*4+0] + vbo->xoff;
        //_modelview[3*4+1] = modelview[3*4+1] + vbo->yoff;

        //float _modelview[16];
        //lGetFloatv(GL_MODELVIEW_MATRIX, _modelview);

        glVertexAttribPointer(map_Vertex, 3, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(struct Vertex), (GLvoid*)0);    
        glVertexAttribPointer(map_TexCoord, 3, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(struct Vertex), (GLvoid*)4);
        glVertexAttribPointer(map_RGB, 3, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(struct Vertex), (GLvoid*)8);
        glVertexAttribPointer(map_LightMatrix, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(struct Vertex), (GLvoid*)12);

        glDrawArrays(GL_QUADS,0, vbo->_v_num[0]);

        //glPopMatrix();
        //glPushMatrix();
    }

    glPopMatrix(); //restore matrix


    glDisableVertexAttribArray(map_Vertex);
    glDisableVertexAttribArray(map_TexCoord);
    glDisableVertexAttribArray(map_RGB);
    glDisableVertexAttribArray(map_LightMatrix);
    
    glUseProgramObjectARB(0);

    //glEnable(GL_TEXTURE_2D);

    glShadeModel(GL_FLAT);
    glDisable(GL_CULL_FACE);
}

void Vbo_map::draw_map_comptability()
{
    //printf("comptability \n");

    prep_draw();
    sort_draw();
    prep_frustrum();

    //GL_ASSERT(GL_TEXTURE_2D, true);
    GL_ASSERT(GL_DEPTH_TEST, true);

    glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_CULL_FACE);


    glColor3ub(255,255,255);

    glUseProgramObjectARB(map_shader[0]);

    glBindTexture( GL_TEXTURE_2D, block_textures_compatibility );

    glEnableVertexAttribArray(map_Vertex);
    glEnableVertexAttribArray(map_TexCoord);
    glEnableVertexAttribArray(map_RGB);
    glEnableVertexAttribArray(map_LightMatrix);

    struct Map_vbo* vbo;

    //glUniform3fv(map_NormalArray , 6, (GLfloat*) _normal_array );

    float modelview[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, modelview);

    glPushMatrix(); //save matrix
    //glPushMatrix();

    for(int i=0;i<draw_vbo_n;i++)
    {
        vbo = draw_vbo_array[i].map_vbo;

        if(vbo->_v_num[0] == 0)
        {
            //printf("t_vbo_draw.cpp:117 no blocks\n");
            continue; 
        } 

        glLoadMatrixf(modelview);
        glTranslatef(vbo->wxoff, vbo->wyoff, 0.0f);

        glBindBuffer(GL_ARRAY_BUFFER, vbo->vbo_id);

        glVertexAttribPointer(map_Vertex, 3, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(struct Vertex), (GLvoid*)0);    
        glVertexAttribPointer(map_TexCoord, 3, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(struct Vertex), (GLvoid*)4);
        glVertexAttribPointer(map_RGB, 3, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(struct Vertex), (GLvoid*)8);
        glVertexAttribPointer(map_LightMatrix, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(struct Vertex), (GLvoid*)12);

        glDrawArrays(GL_QUADS,0, vbo->_v_num[0]);
    }

    glPopMatrix(); //restore matrix

    glDisableVertexAttribArray(map_Vertex);
    glDisableVertexAttribArray(map_TexCoord);
    glDisableVertexAttribArray(map_RGB);
    glDisableVertexAttribArray(map_LightMatrix);


    glUseProgramObjectARB(0);


    //glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_FLAT);
    glDisable(GL_CULL_FACE);

}

}
