#include "cube_selector.hpp"

#include <common/compat_gl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL/draw_functions.hpp>
#include <agent/agent.hpp>
//#include <agent/agent_weapons.hpp>
//#include <weapons/weapons.hpp>

#include <t_map/glsl/texture.hpp>

namespace HudCubeSelector
{

void CubeSelector::set_block_selector(int pos, CubeID cube_id, int tex_id)
{
    int ct = this->n_x*this->n_y;
    GS_ASSERT(cube_id != NULL_CUBE);
    GS_ASSERT(tex_id != NULL_SPRITE && tex_id >= 0);
    GS_ASSERT(pos >= 0 && pos < ct);
    if (cube_id == NULL_CUBE) return;
    if (tex_id == NULL_SPRITE || tex_id < 0) return;
    if (pos < 0 || pos >= ct) return;

    GS_ASSERT(cubes[pos].cube_id == NULL_CUBE);
    GS_ASSERT(cubes[pos].tex_id == NULL_SPRITE);
    cubes[pos].cube_id = cube_id;

    for (int i=0; i<ct; i++)
        if (cubes[i].tex_id != NULL_SPRITE)
            GS_ASSERT(cubes[i].tex_id != tex_id); 
    
    cubes[pos].tex_id = tex_id;
}

void CubeSelector::set_block_selector(CubeID cube_id, int tex_id)
{
    int pos = 0;
    for (; pos<this->n_x*this->n_y; pos++)
        if (cubes[pos].tex_id == NULL_SPRITE)
            break;
    if (pos >= this->n_x*this->n_y)
    {
        printf("WARNING: Hud cube selector is full\n");
        return;
    }
    this->set_block_selector(pos, cube_id, tex_id);
}

void CubeSelector::set_position(float x, float y)
{
    this->x = _xresf - x;
    this->y = y;
}

CubeID CubeSelector::get_active_id()    //get the cube selected by hud
{
    return cubes[this->get_active_pos()].cube_id;
}

int CubeSelector::get_active_pos()
{
    return this->pos;
}

void CubeSelector::set_active_pos(int pos)
{
    this->pos = pos;
    this->pos_x = pos % 8;
    this->pos_y = pos / 8;
}

void CubeSelector::set_active_id(int id)
{
    if (id == 0xFF) return;
    for (int i=0; i<MAX_CUBES; i++)
        if (cubes[i].cube_id == id)
        {
            this->set_active_pos(i);
            break;
        }
}

void CubeSelector::draw()
{
    GS_ASSERT(this->cubes != NULL);
    if (this->cubes == NULL) return;

    const float _ssize = 16;
    const float sborder = 1;
    const float z_ = -0.5f;
    const float txmargin = 0.001f;

    const float width = this->n_x * (_ssize+sborder);
    const float height = this->n_y * (_ssize+sborder);

    // draw black background
    glColor3ub(0,0,0);
    glBegin(GL_QUADS);
    glVertex3f(this->x, this->y, z_);
    glVertex3f(this->x + width, this->y, z_);
    glVertex3f(this->x + width, this->y - height, z_);
    glVertex3f(this->x, this->y - height, z_);
    glEnd();

    glColor3ub(255,255,255);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, t_map::block_textures_normal);

    glBegin(GL_QUADS);

    // draw sprites
    for(int i=0; i<this->n_x; i++)
    for(int j=0; j<this->n_y; j++)
    {
        CubeID cube_id = cubes[i+8*j].cube_id;
        if (cube_id == NULL_CUBE) continue;
        int tex_id = cubes[i+8*j].tex_id;
        
        float x0 = x + i*(_ssize+sborder) + sborder/2;
        float x1 = x0 + _ssize - sborder;
        float y0 = y - j*(_ssize+sborder) + sborder/2;
        float y1 = y0 - _ssize + sborder;

        int ti = tex_id % 16;
        int tj = tex_id / 16;

        float tx_min = ti*0.0625f + txmargin;
        float ty_min = tj*0.0625f + txmargin;
        float tx_max = ti*0.0625f + 0.0625f - txmargin;
        float ty_max = tj*0.0625f + 0.0625f - txmargin;

        glTexCoord2f(tx_min,ty_max);
        glVertex3f(x0, y0, z_);  // Top left

        glTexCoord2f(tx_max,ty_max);
        glVertex3f(x1,y0, z_);  // Top right

        glTexCoord2f(tx_max,ty_min );
        glVertex3f(x1,y1, z_);  // Bottom right

        glTexCoord2f(tx_min,ty_min );
        glVertex3f(x0,y1, z_);  // Bottom left
    }

    glEnd();
    glDisable(GL_TEXTURE_2D);

    // draw selected cube outline
    int i = this->pos_x;
    int j = this->pos_y;

    float x0 = x + i*(_ssize+sborder) + sborder/2;
    float x1 = x0 + _ssize - sborder;
    float y0 = y - j*(_ssize+sborder) + sborder/2;
    float y1 = y0 - _ssize + sborder;
    
    glLineWidth(1.0f);
    glColor4ub(0,0,255,255);  // blue
    glBegin(GL_LINE_STRIP);

    glVertex3f(x0, y0, z_);  // Top left
    glVertex3f(x1,y0, z_);  // Top right
    glVertex3f(x1,y1, z_);  // Bottom right
    glVertex3f(x0,y1, z_);  // Bottom left
    glVertex3f(x0, y0, z_); // tie up the last line

    glEnd();
    glLineWidth(1.0f);
    glColor4ub(255,255,255,255);
}

/* Controls */
void CubeSelector::up()
{
    this->vertical(true);
}
void CubeSelector::down()
{
    this->vertical(false);
}
void CubeSelector::left()
{
    this->horizontal(true);
}
void CubeSelector::right()
{
    this->horizontal(false);
}

void CubeSelector::horizontal(bool left)
{
    int shift = (left) ? -1 : 1;
    int row = this->pos / this->n_x;
    int col = this->pos % this->n_x;

    int n = (col + shift) % this->n_x;
    if (n < 0) n += this->n_x;
    n += row * this->n_x;

    if (n < 0 || n > (this->n_x*this->n_y-1)) return;

    this->set_active_pos(n);
}

void CubeSelector::vertical(bool up)
{
    int shift = (up) ? -1 : 1;
    int row = this->pos / this->n_x;
    int col = this->pos % this->n_x;

    row = (row + shift) % this->n_y;
    if (row < 0) row += this->n_y;
    int n = (row * this->n_x) + col;

    if (n < 0 || n > (this->n_x*this->n_y-1)) return;

    this->set_active_pos(n);
}

bool CubeSelector::set_block_type(CubeID cube_id)
{
    GS_ASSERT(this->cubes != NULL);
    if (this->cubes == NULL) return false;
    
    ASSERT_VALID_CUBE_ID(cube_id);
    GS_ASSERT(t_map::isInUse(cube_id));
    GS_ASSERT(cube_id != ERROR_CUBE);
    if (!t_map::isInUse(cube_id) || cube_id == ERROR_CUBE) return false;
    IF_INVALID_CUBE_ID(cube_id) return false;

    for (int i=0; i<this->n_x*this->n_y; i++)
    {
        if (this->cubes[i].cube_id == cube_id)
        {
            this->set_active_pos(i);
            return true;
        } 
    }
    return false;
}

CubeSelector::CubeSelector() :
    x(0),y(0),
    n_x(8), n_y(8),
    pos(0),
    pos_x(0), pos_y(0)
{
    this->cubes = (struct CubeSelectElement*)malloc(MAX_CUBES * sizeof(struct CubeSelectElement));
    for(int i=0; i<MAX_CUBES; i++)
    {
        cubes[i].cube_id = NULL_CUBE;
        cubes[i].tex_id = NULL_SPRITE;
    }
}

CubeSelector cube_selector;

void init()
{
    cube_selector.set_position(150,250);
}

void set_cube_hud(int hudx, int hudy, CubeID cube_id, int tex_id)
{
    if(hudx < 1 || hudy < 1 || hudx > 8 || hudy > 8)
    {
        printf("set_cube_hud error! hux= %i hudy= %i \n", hudx,hudy);
        return;
    }
    hudx--;
    hudy--;
    HudCubeSelector::cube_selector.set_block_selector(8*hudy+hudx, cube_id, tex_id);
}

void set_cube_hud(CubeID cube_id, int tex_id)
{
    HudCubeSelector::cube_selector.set_block_selector(cube_id, tex_id);
}

}   // HudCubeSelector
