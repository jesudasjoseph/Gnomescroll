#include "inventory.hpp"

#include <compat_gl.h>
namespace HudInventory
{
    
void Inventory::draw()
{
    if (!this->inited) return;
    const float z = -0.5;

    glColor3ub(255,255,255);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, inventory_background_texture);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   
    //draw background
    glBegin(GL_QUADS);
    {
        const int x_size = 512;
        const int y_size = 512;

        glTexCoord2f(0.0,0.0);
        glVertex3f(x, y, z);  // Top left

        glTexCoord2f(1.0,0.0);
        glVertex3f(x+x_size, y, z);  // Top right

        glTexCoord2f(1.0,1.0);
        glVertex3i(x+x_size, y-y_size, z);  // Bottom right

        glTexCoord2f(0.0,1.0);
        glVertex3i(x, y-y_size, z);  // Bottom left
    }
    glEnd();

    glBindTexture(GL_TEXTURE_2D, item_slot_texture);
    glBegin(GL_QUADS);

    {
        int i, j;
        float x,y;

        const int x_size = 64;
        const int y_size = 64;

        const int x_off = 10;
        const int y_off = 51;
        const int x_inc = 50;
        const int y_inc = 50;

        for(i=0;i<9; i++)
        for(j=0;j<6; j++)
        {
            x = this->x + x_off + i*x_inc;
            y = this->y - y_off - j*y_inc;

            glTexCoord2f(0.0,0.0);
            glVertex3f(x, y, z);  // Top left

            glTexCoord2f(1.0,0.0);
            glVertex3f(x+x_size, y, z);  // Top right

            glTexCoord2f(1.0,1.0);
            glVertex3i(x+x_size, y-y_size, z);  // Bottom right

            glTexCoord2f(0.0,1.0);
            glVertex3i(x, y-y_size, z);  // Bottom left
        }
    }
    glEnd();

    glBindTexture(GL_TEXTURE_2D, item_sheet_texture);

    glBegin(GL_QUADS);
    {
        int i, j;
        float x,y;
        int ti, tj;
        float tx, ty;
        int index;

        const int x_size = 32;
        const int y_size = 32;

        const int x_off = 14;
        const int y_off = 55;
        const int x_inc = 50;
        const int y_inc = 50;

        const float tx_inc = 0.0625;
        const float ty_inc = 0.0625;

        for(i=0;i<9; i++) {
        for(j=0;j<6; j++) {
            
            x = this->x + x_off + i*x_inc;
            y = this->y - y_off - j*y_inc;


            if(i<=j) index = 0;
            if(i>j) index = 1;

            ti = index % 16;
            tj = index / 16;
            
            tx = ti*tx_inc;
            ty = tj*ty_inc;
            
            glTexCoord2f(tx, ty);
            glVertex3f(x, y, z);  // Top left

            glTexCoord2f(tx+tx_inc, ty);
            glVertex3f(x+x_size, y, z);  // Top right

            glTexCoord2f(tx+tx_inc, ty+ty_inc);
            glVertex3i(x+x_size, y-y_size, z);  // Bottom right

            glTexCoord2f(tx, ty+tx_inc);
            glVertex3i(x, y-y_size, z);  // Bottom left

        }}
    }
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}

void Inventory::set_position(float x, float y)
{
    this->x = x;
    this->y = y;
}

void Inventory::init()
{
    if (this->inited) return;

    int a,b,c;
    a = create_texture_from_file((char*) "./media/texture/hud/item_sheet_01.png", &this->item_sheet_texture);
    b = create_texture_from_file((char*)"./media/texture/hud/item_slot_41x41.png", &this->item_slot_texture);
    c = create_texture_from_file((char*)"./media/texture/hud/inventory_background_461x352.png", &this->inventory_background_texture);
    if(a!=0 || b!=0 || c!=0) printf("%d %d %d -Inventory Texture Loader Error (any nonzero is failure)\n", a,b,c);
    if (a||b||c) return;

    this->inited = true;
}

Inventory::Inventory()
:
inited(false),
x(0),y(0),
item_sheet_texture(0),
item_slot_texture(0),
inventory_background_texture(0)
{}


Inventory inventory;

void init()
{
    inventory.init();
    inventory.set_position(250, 500);
}

}
