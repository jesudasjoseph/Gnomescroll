#include "billboard_sprite.hpp"

#if DC_CLIENT

#include <common/draw/draw.hpp>
#include <entity/components/physics.hpp>

namespace Components
{

void TexturedVoxelComponent::set_texture()
{
    const int pix = this->pixel_width;  // NxN random texture sample within cube identified by texture_index
    const int cube_w = 32;
    const int cube_surface_w = 512;
    const int cubes_per_surface_axis = cube_surface_w / cube_w;
    const float scale = 2.0f/((float)cube_w);

    const float pixel_w = 1.0f/((float)cube_surface_w);

    this->pixel_margin = pixel_w*pix;

    int itx,ity;
    float ftx,fty;

    // integer index of cube in spritesheet
    itx = this->texture_index % cubes_per_surface_axis;
    ity = this->texture_index / cubes_per_surface_axis;

    // ...scaled to texture
    ftx = itx * scale;
    fty = ity * scale;

    int margin = cube_w - pix;
    GS_ASSERT(margin >= 0);

    //0 0 randrange(1,1) - 1;
    //1 0, 1,-1 randrange(1,3) - 2;
    //2 0, 1, 2, -1, -2 randrange(1, 5) - 3;
    //3 0, 1, 2, 3, -1, -2, -3 randrange(1, 7) - 4;

    // random offset in x,y within texture quad to sample
    int mx = randrange(1, (margin*2)+1) - (margin+1);
    int my = randrange(1, (margin*2)+1) - (margin+1);
    
    // ...scaled to texture
    float fx = ((float)mx) * pixel_w;
    float fy = ((float)my) * pixel_w;

    ftx += fx;
    fty += fy;

    this->tx = ftx;
    this->ty = fty;
}

inline void TexturedVoxelComponent::draw(Vec3 position)
{
    drawTexturedMinivox(position, this->forward, this->right, this->normal,
        this->texture_index, this->tx, this->ty, this->pixel_margin);
}

void TexturedVoxelComponent::call()
{
    PhysicsComponent* state = (PhysicsComponent*)this->object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    if (state == NULL) return;
    this->draw(state->get_position());
}

} // Components

#endif