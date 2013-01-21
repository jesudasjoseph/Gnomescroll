#pragma once

#include <t_map/_interface.hpp>
#include <t_map/common/map_element.hpp>

namespace t_map
{

//15 is now highest light level

/*
    Update skylights seperately
*/

//e[ (z<<8)+((y&15)<<4)+(x&15) ] = e;
void update_skylight(int chunk_i, int chunk_j)
{
    class MAP_CHUNK* mc = main_map->chunk[32*chunk_j + chunk_i];
    struct MAP_ELEMENT e;

    //struct MAP_e e;

    for(int i=0; i<16; i++)
    for(int j=0; j<16; j++)
    {
        int k = map_dim.z-1;

        // get highest block
        for (; k>=0; k--)
        {
            e = mc->get_element(i,j,k);
            if(e.block != 0)    //iterate until we hit top block
                break;
            //e.light = 0x0f; //clear bottom bits, set to 15
            //e.light = 0x0f; //clear bottom bits, set to 15
            e.light = 15;

            mc->set_element(i,j,k,e);
        }
        if (k < 0) return;

        // render gradient down from top block
        
/*
        for (int _k=0; k>=0 && _k<16; k--, _k++)
        {
            e = mc->get_element(i,j,k);
            if(e.block != 0)
                continue;
            e.light  |= 16*(15-_k)+ 0x0f; //clear upper bits
            mc->set_element(i,j,k,e);
        }
*/
        // black out everything below
        for (; k>=0; k--)
        {
            e = mc->get_element(i,j,k);
            if(e.block != 0)
                continue;
            //e.light |= 0xf0;  //clear bottom bits, set to zero
            //e.light = 0x00;     //clear bottom bits, set to zero
            e.light = 0;
            mc->set_element(i,j,k,e);
        }
    }

}

//for comparision purpose
int get_skylight(int x, int y, int z)
{

    if( (z & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0)
        return 0;

    x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    y &= TERRAIN_MAP_WIDTH_BIT_MASK2;

    class MAP_CHUNK* mc = main_map->chunk[ 32*(y >> 4) + (x >> 4) ];
    if(mc == NULL)
        return 16;  //so it does not try to update

    //return mc->e[ (z<<8)+((y&15)<<4)+(x&15) ].light;

    return mc->e[ (z<<8)+((y&15)<<4)+(x&15) ].light & 0x0f;  //bottom half
}

void set_skylight(int x, int y, int z, int value)
{
    GS_ASSERT((z & TERRAIN_MAP_HEIGHT_BIT_MASK) == 0);

    x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    y &= TERRAIN_MAP_WIDTH_BIT_MASK2;

    class MAP_CHUNK* mc = main_map->chunk[ 32*(y >> 4) + (x >> 4) ];
    if(mc == NULL)
        return;
    GS_ASSERT(mc != NULL);
    GS_ASSERT( (y >> 4) < 32);
    GS_ASSERT( (x >> 4) < 32);
    GS_ASSERT(value < 16 && value > 0);
    //printf("%i\n", (z<<8)+((y&15)<<4)+(x&15) );

    int light = mc->e[ (z<<8)+((y&15)<<4)+(x&15) ].light;
    //light = value;
    //light &= 0xf0;          //clear lower byte
    //light |= (value & 0x0f); //set lower byte
    light = (light & 0xf0) | (value & 0x0f);
    //light = (light & 0xf0)
    mc->e[ (z<<8)+((y&15)<<4)+(x&15) ].light = light;
    //mc->e[ (z<<8)+((y&15)<<4)+(x&15) ].light &= (value & 0x0f);  //bottom half
}

//proprogate out
void update_skylight_out(int x, int y, int z)
{
    int li = get_skylight(x,y,z);

    //GS_ASSERT(! isSolid(x,y,z));
    if(li-1 <= 0) return;

/*
    !isSolid(_x,_y,_z) && get_skylight(_x,_y,_z) < li-1 ) {
        set_skylight(_x,_y,_z, li-1);
    These can be combined into a single function
*/

/*
    Recursion can be replaced by poping onto a circular buffer
*/

    int _x,_y,_z;

    _x = (x+1) & TERRAIN_MAP_WIDTH_BIT_MASK2;
    _y = y;
    _z = z;
    //x
    if(!isSolid(_x,_y,_z) && get_skylight(_x,_y,_z) < li-1 )   //do a single get block for this!!
    {
        set_skylight(_x,_y,_z, li-1);
        update_skylight_out(_x,_y,_z);
    }

    //GS_ASSERT( ((x-1) & TERRAIN_MAP_WIDTH_BIT_MASK2) == (x+512-1) % 512);

    _x = (x-1) & TERRAIN_MAP_WIDTH_BIT_MASK2;
    _y = y;
    _z = z;

    if(!isSolid(_x,_y,_z) && get_skylight(_x,_y,_z) < li-1 )
    {
        set_skylight(_x,_y,_z, li-1);
        update_skylight_out(_x,_y,_z);
    }

    //y
    _x = x;
    _y = (y+1) & TERRAIN_MAP_WIDTH_BIT_MASK2;
    _z = z;
    //x
    if(!isSolid(_x,_y,_z) && get_skylight(_x,_y,_z) < li-1 )
    {
        set_skylight(_x,_y,_z, li-1);
        update_skylight_out(_x,_y,_z);
    }

    _x = x;
    _y = (y-1) & TERRAIN_MAP_WIDTH_BIT_MASK2;
    _z = z;

    if(!isSolid(_x,_y,_z) && get_skylight(_x,_y,_z) < li-1 )
    {
        set_skylight(_x,_y,_z, li-1);
        update_skylight_out(_x,_y,_z);
    }

    //y
    _x = x;
    _y = y;
    _z = (z+1) % map_dim.z;

    if(!isSolid(_x,_y,_z) && get_skylight(_x,_y,_z) < li-1 )
    {
        set_skylight(_x,_y,_z, li-1);
        update_skylight_out(_x,_y,_z);
    }

    _x = x;
    _y = y;
    _z = (z-1+map_dim.z)%map_dim.z; //z -1
    if(!isSolid(_x,_y,_z) && get_skylight(_x,_y,_z) < li-1 )
    {
        set_skylight(_x,_y,_z, li-1);
        update_skylight_out(_x,_y,_z);
    }

}

//call this when removing a block
void update_skylight_in(int x, int y, int z)
{
    //int li = get_skylight(x,y,z);

    if(isSolid(x,y,z))
    {
        GS_ASSERT(false);
        return;
    }
    //technically, if top block of height map
    if( get_skylight(x,y,z+1) == 15 )
    {
        GS_ASSERT( z+1 == main_map->get_height(x,y) );
        
        /*
            BUG:
            Fails on removing block capping a column?
        */

        int _z = z;
        //assume first element is not solid
        while(1)
        {
            set_skylight(x,y,_z,15);
            if(isSolid(x,y,_z-1) || _z==0)
                break;
            _z--;
        }

        for(int tz=_z; tz <= z; tz++)
            update_skylight_out(x,y,tz);

        return;
    }
    int li = 0;

    int _x,_y,_z, tli;

    //x
    _x = (x+1) & TERRAIN_MAP_WIDTH_BIT_MASK2;
    _y = y;
    _z = z;

    tli = get_skylight(_x,_y,_z);
    if(tli > li )
        li = tli;

    _x = (x-1) & TERRAIN_MAP_WIDTH_BIT_MASK2;
    _y = y;
    _z = z;

    tli = get_skylight(_x,_y,_z);
    if(tli > li )
        li = tli;

    //y
    _x = x;
    _y = (y+1) & TERRAIN_MAP_WIDTH_BIT_MASK2;
    _z = z;

    tli = get_skylight(_x,_y,_z);
    if(tli > li )
        li = tli;

    _x = x;
    _y = (y-1) & TERRAIN_MAP_WIDTH_BIT_MASK2;
    _z = z;

    tli = get_skylight(_x,_y,_z);
    if(tli > li )
        li = tli;

    //z
    _x = x;
    _y = y;
    _z = (z+1) % map_dim.z;

    tli = get_skylight(_x,_y,_z);
    if(tli > li )
        li = tli;

    _x = x;
    _y = y;
    _z = (z+(map_dim.z-1))%map_dim.z;

    tli = get_skylight(_x,_y,_z);
    if(tli > li )
        li = tli;


    if(li != 0)
    {
        set_skylight(x,y,z, li);
        update_skylight_out(x,y,z);
    }
}

void update_skylight_boundary(int _ci, int _cj)
{
    class MAP_CHUNK* mc;
    int ci, cj;

    //north?
    ci = (_ci + 1 +32 ) % 32;
    cj = (_cj + 0 +32 ) % 32;
    mc = main_map->chunk[32*cj + ci];

    if(mc != NULL)
    {
        const int i = 0;
        for(int j=0; j<16; j++)
        for(int k=0; k<map_dim.z; k++)
        {
            if(isSolid(16*ci+i,16*cj+j,k) ) //|| get_skylight(16*ci+i,16*cj+j,k) != 15) // || get_skylight(i,j,k) < 16)
                continue;
            update_skylight_out(16*ci+i,16*cj+j,k);
        }
    }


    //south?
    ci = (_ci + -1 +32 ) % 32;
    cj = (_cj + 0 +32 ) % 32;
    mc = main_map->chunk[32*cj + ci];

    if(mc != NULL)
    {
        const int i = 15;
        for(int j=0; j<16; j++)
        for(int k=0; k<map_dim.z; k++)
        {
            if(isSolid(16*ci+i,16*cj+j,k) ) //|| get_skylight(16*ci+i,16*cj+j,k) != 15) // || get_skylight(i,j,k) < 16)
                continue;
            update_skylight_out(16*ci+i,16*cj+j,k);
        }
    }

    //west?
    ci = (_ci + 0 +32 ) % 32;
    cj = (_cj + 1 +32 ) % 32;
    mc = main_map->chunk[32*cj + ci];

    if(mc != NULL)
    {
        const int j = 0;
        for(int i=0; i<16; i++)
        for(int k=0; k<map_dim.z; k++)
        {
            if(isSolid(16*ci+i,16*cj+j,k) ) //|| get_skylight(16*ci+i,16*cj+j,k) != 15) // || get_skylight(i,j,k) < 16)
                continue;
            update_skylight_out(16*ci+i,16*cj+j,k);
        }
    }


    //east?
    ci = (_ci + 0 +32 ) % 32;
    cj = (_cj + -1 +32 ) % 32;
    mc = main_map->chunk[32*cj + ci];

    if(mc != NULL)
    {
        const int j = 15;
        for(int i=0; i<16; i++)
        for(int k=0; k<map_dim.z; k++)
        {
            if(isSolid(16*ci+i,16*cj+j,k) ) //|| get_skylight(16*ci+i,16*cj+j,k) != 15) // || get_skylight(i,j,k) < 16)
                continue;
            update_skylight_out(16*ci+i,16*cj+j,k);
        }
    }

}

void update_skylight2(int ci, int cj)
{
    update_skylight_boundary(ci, cj);

    class MAP_CHUNK* mc = main_map->chunk[32*cj + ci];

    mc->refresh_height_cache();

    for(int i=0; i<16; i++)
    for(int j=0; j<16; j++)
    for(int k=0; k<map_dim.z; k++)
    {
        if(isSolid(16*ci+i,16*cj+j,k) ) //|| get_skylight(16*ci+i,16*cj+j,k) != 15) // || get_skylight(i,j,k) < 16)
            continue;

        update_skylight_out(16*ci+i,16*cj+j,k);

    }

}

/*
    Env Light
*/


//for comparision purpose
int get_envlight(int x, int y, int z)
{
    if( (z & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0)
        return 0;

    x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    y &= TERRAIN_MAP_WIDTH_BIT_MASK2;

    class MAP_CHUNK* mc = main_map->chunk[ 32*(y >> 4) + (x >> 4) ];
    if(mc == NULL)
        return 0;  //so it does not try to update

    return (mc->e[ (z<<8)+((y&15)<<4)+(x&15) ].light >> 4);  //upper half
}

void set_envlight(int x, int y, int z, int value)
{
    GS_ASSERT((z & TERRAIN_MAP_HEIGHT_BIT_MASK) == 0);

    x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    y &= TERRAIN_MAP_WIDTH_BIT_MASK2;

    class MAP_CHUNK* mc = main_map->chunk[ 32*(y >> 4) + (x >> 4) ];
    if(mc == NULL)
        return;
    GS_ASSERT(mc != NULL);
    GS_ASSERT( (y >> 4) < 32);
    GS_ASSERT( (x >> 4) < 32);
    GS_ASSERT(value < 16 && value > 0);
    //printf("%i\n", (z<<8)+((y&15)<<4)+(x&15) );

    int light = mc->e[ (z<<8)+((y&15)<<4)+(x&15) ].light;

    //light &= 0x0f;          //clear upper nibble
    //light |= (value << 4);  //set upper nibble

    light = (light & 0x0f) | (value << 4); // clear upper nib, set upper nib

    mc->e[ (z<<8)+((y&15)<<4)+(x&15) ].light = light;
    //mc->e[ (z<<8)+((y&15)<<4)+(x&15) ].light &= (value << 4);  //upper half
}

static inline
int ENV_LIGHT_MIN(int li[6])
{
    int min = li[0];

    for(int i=1; i<6; i++)
    {
        if(li[i] < min)
        {
            min = li[i];
        }
    }

    return min;
}

static inline
int ENV_LIGHT_MAX(int li[6])
{
    int max = li[0];

    for(int i=1; i<6; i++)
    {
        if(li[i] > max)
        {
            max = li[i];
        }
    }
    return max;
}

/*
    Each block should only set its own values
*/
void _envlight_update(int x, int y, int z)
{

    //x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    //y &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    //z &= 127;
    static const int va[3*6] =
    {
        0,0,1,
        0,0,-1,
        1,0,0,
        -1,0,0,
        0,1,0,
        0,-1,0
    };


    struct MAP_ELEMENT e = get_element(x,y,z);
    //fast_cube_properties[e.block].light_source == false

    //int li = get_envlight(x,y,z); //light value at current position
    int li = e.light;


    if(fast_cube_properties[e.block].light_source == true)
    {
            //light source block
        #if 0
            for(int i=0; i<6; i++)
            {
                if(get_envlight(x+va[3*i+0] ,y+va[3*i+1] , z+va[3*i+2]) < li -1)
                    _envlight_update(x+va[3*i+0] ,y+va[3*i+1] , z+va[3*i+2]);
            }
            return;
        #else
            for(int i=0; i<6; i++)
            {
                struct MAP_ELEMENT _e = get_element(x+va[3*i+0] ,y+va[3*i+1] , z+va[3*i+2]);
                if(_e.light < li -1 && fast_cube_properties[_e.block].solid == false)
                    _envlight_update(x+va[3*i+0] ,y+va[3*i+1] , z+va[3*i+2]);
            }
            return;
        #endif
        }
        else
        {
            //solid, not light source
            return;
        }

    }

    else
    {

        //solid non-light source
        if(fast_cube_properties[e.block].solid == true)
        {
            return;
        }
       //non-solid non-light source

    }



    int lia[6];
    for(int i=0; i<6; i++)
    {
        lia[i] = get_envlight(x+va[3*i+0] ,y+va[3*i+1] , z+va[3*i+2]);
    }

    int min = ENV_LIGHT_MIN(lia);
    int max = ENV_LIGHT_MAX(lia);

    if(li == max -1)
    {
        //done, lighting is correct
        return;
    }


    //check for removal condition

    //proprogate outward
    if(li < max -1)
    {
        GS_ASSERT(max - 1 >= 0); //cannot have negative values
        if(fast_cube_properties[e.block].light_source == true)
            return;
        
        //set light of current block to max -1 of sourounding blocks
        set_envlight(x,y,z, max-1);
        for(int i=0; i<6; i++)
        {
            //conditions?
            _envlight_update(x+va[3*i+0] ,y+va[3*i+1], z+va[3*i+2]);
        }
        return;
    }


    struct MAP_ELEMENT ea[6];

    for(int i=0; i<6; i++)
    {
        ea[i] = get_element(x+va[3*i+0] ,y+va[3*i+1] , z+va[3*i+2]);
    }

    for(int i=0; i<6; i++)
    {
        //proprogate out to all blocks that are not solid
        if(fast_cube_properties[ea[i].block].solid == false && 
            ea[i].light < li-1)
        {
        
        }

    }

    for(int i=0; i<6; i++)
    {
        if(fast_cube_properties[ea[i].block].solid == false && 
            ea[i].light < li-1)
        {
            set_envlight(x+va[3*i+0] ,y+va[3*i+1] , z+va[3*i+0], li-1);
        }
    }
/*
    lia[0] = get_envlight(x ,y , z+1);
    lia[1] = get_envlight(x ,y , z-1);
    lia[2] = get_envlight(x+1,y, z);
    lia[3] = get_envlight(x-1,y, z);
    lia[4] = get_envlight(x, y+1, z);
    lia[5] = get_envlight(x, y-1, z);
*/

/*
    int min = ENV_LIGHT_MIN(lia);
    int max = ENV_LIGHT_MAX(lia);

    if(min < 

    if(lia[0] < li-1) _envlight_update(x ,y , z+1);
    if(lia[1] < li-1) _envlight_update(x ,y , z-1);
    if(lia[2] < li-1) _envlight_update(x+1,y, z);
    if(lia[3] < li-1) _envlight_update(x-1,y, z);
    if(lia[4] < li-1) _envlight_update(x, y+1, z);
    if(lia[5] < li-1) _envlight_update(x, y-1, z);

    if(li != max -1)
    {
        //cannot be solid block
        GS_ASSERT(fast_cube_properties[e.block].solid == false)

        //check if its source block
        if(fast_cube_properties[e.block].light_source == true)
        {
            //source blocks must be at light value
            GS_ASSERT(li == fast_cube_attributes[e.block].light_value)
            //return;
        }
        else
        {
            //non source blocks must be one less than max of blocks around them
            //GS_ASSERT(li > max -1);
        }

        

        //update blocks if they are less than current light value
        //if(lia[0] < max-1) _envlight_update

        //proprogate outwards
    }
    //int min = ENV_LIGHT_MIN(li_t, li_b, li_n, li_s, li_s, li_w, li_e);

    */
}

/*
    Add block operations
*/

//proprogates light out from a block
void _envlight_helper_out(int x, int y, int z, int li);

//optimize the hell out of this
void _envlight_helper_out2(int _x, int _y, int _z, int li)
{
    //this is place to handle chunk not loaded stuff
    if(!isSolid(_x,_y,_z) && get_envlight(_x,_y,_z) < li )   //do a single get block for this!!
    {
        set_envlight(_x,_y,_z, li);
        _envlight_helper_out(_x,_y,_z, li);
    }
}

void _envlight_helper_out(int x, int y, int z, int li)
{  
    //set light value?
    //int li = get_envlight(x,y,z);


    int _x,_y,_z;

    //x
    _x = (x+1) & TERRAIN_MAP_WIDTH_BIT_MASK2;
    _y = y;
    _z = z;
    _envlight_helper_out2(_x,_y,_z, li-1);

    _x = (x-1) & TERRAIN_MAP_WIDTH_BIT_MASK2;
    _y = y;
    _z = z;
    _envlight_helper_out2(_x,_y,_z, li-1);

    //y
    _x = x;
    _y = (y+1) & TERRAIN_MAP_WIDTH_BIT_MASK2;
    _z = z;
    _envlight_helper_out2(_x,_y,_z, li-1);

    _x = x;
    _y = (y-1) & TERRAIN_MAP_WIDTH_BIT_MASK2;
    _z = z;
    _envlight_helper_out2(_x,_y,_z, li-1);

    //z
    _x = x;
    _y = y;
    _z = (z+1) % map_dim.z;
    _envlight_helper_out2(_x,_y,_z, li-1);

    _x = x;
    _y = y;
    _z = (z-1+map_dim.z)%map_dim.z;
    _envlight_helper_out2(_x,_y,_z, li-1);
}

/*
void envlight_add_block(int x, int y, int z)
{  
    //set light value?
    int li = get_envlight(x,y,z);

    _envlight_helper_out(x,y,z,li);
}
*/

void update_envlight_boundary(int _ci, int _cj);

void update_envlight(int chunk_i, int chunk_j)
{
    //return;

    class MAP_CHUNK* mc = main_map->chunk[32*chunk_j + chunk_i];
    struct MAP_ELEMENT e;

    //struct MAP_e e;

    for(int k=0; k<128; k++)
    for(int i=0; i<16; i++)
    for(int j=0; j<16; j++)
    {

        int x = 16*chunk_i + i;
        int y = 16*chunk_j + j;

        e = mc->get_element(i,j,k);
        //if(e.block != 0)    //iterate until we hit top block
        //    break;

        //skip if its not a light source

        if(fast_cube_properties[e.block].light_source == false && 
            fast_cube_attributes[e.block].light_value != 0)
        {
            GS_ASSERT(false);
        }

        //if(fast_cube_properties[e.block].light_source == false)
        //    continue;

        int lv = fast_cube_attributes[e.block].light_value;
        if(lv == 0)
            continue;

        _envlight_helper_out(x,y,k,lv);
        //e.light = 15;
        //mc->set_element(i,j,k,e);

    }


    update_envlight_boundary(chunk_i, chunk_j);

}


//proprogate out
void update_envlight_out(int x, int y, int z)
{
    int li = get_envlight(x,y,z);

    //GS_ASSERT(! isSolid(x,y,z));
    if(li-1 <= 0) return;

/*
    !isSolid(_x,_y,_z) && get_envlight(_x,_y,_z) < li-1 ) {
        set_envlight(_x,_y,_z, li-1);
    These can be combined into a single function
*/

/*
    Recursion can be replaced by poping onto a circular buffer
*/

    int _x,_y,_z;

    _x = (x+1) & TERRAIN_MAP_WIDTH_BIT_MASK2;
    _y = y;
    _z = z;
    //x
    if(!isSolid(_x,_y,_z) && get_envlight(_x,_y,_z) < li-1 )   //do a single get block for this!!
    {
        set_envlight(_x,_y,_z, li-1);
        update_envlight_out(_x,_y,_z);
    }

    //GS_ASSERT( ((x-1) & TERRAIN_MAP_WIDTH_BIT_MASK2) == (x+512-1) % 512);

    _x = (x-1) & TERRAIN_MAP_WIDTH_BIT_MASK2;
    _y = y;
    _z = z;

    if(!isSolid(_x,_y,_z) && get_envlight(_x,_y,_z) < li-1 )
    {
        set_envlight(_x,_y,_z, li-1);
        update_envlight_out(_x,_y,_z);
    }

    //y
    _x = x;
    _y = (y+1) & TERRAIN_MAP_WIDTH_BIT_MASK2;
    _z = z;
    //x
    if(!isSolid(_x,_y,_z) && get_envlight(_x,_y,_z) < li-1 )
    {
        set_envlight(_x,_y,_z, li-1);
        update_envlight_out(_x,_y,_z);
    }

    _x = x;
    _y = (y-1) & TERRAIN_MAP_WIDTH_BIT_MASK2;
    _z = z;

    if(!isSolid(_x,_y,_z) && get_envlight(_x,_y,_z) < li-1 )
    {
        set_envlight(_x,_y,_z, li-1);
        update_envlight_out(_x,_y,_z);
    }

    //y
    _x = x;
    _y = y;
    _z = (z+1) % map_dim.z;

    if(!isSolid(_x,_y,_z) && get_envlight(_x,_y,_z) < li-1 )
    {
        set_envlight(_x,_y,_z, li-1);
        update_envlight_out(_x,_y,_z);
    }

    _x = x;
    _y = y;
    _z = (z-1+map_dim.z)%map_dim.z; //z -1
    if(!isSolid(_x,_y,_z) && get_envlight(_x,_y,_z) < li-1 )
    {
        set_envlight(_x,_y,_z, li-1);
        update_envlight_out(_x,_y,_z);
    }

}

//call this when removing a block
void update_envlight_in(int x, int y, int z)
{
    //int li = get_envlight(x,y,z);

    if(isSolid(x,y,z))
    {
        GS_ASSERT(false);
        return;
    }
    //technically, if top block of height map
    if( get_envlight(x,y,z+1) == 15 )
    {
        GS_ASSERT( z+1 == main_map->get_height(x,y) );
        
        /*
            BUG:
            Fails on removing block capping a column?
        */

        int _z = z;
        //assume first element is not solid
        while(1)
        {
            set_envlight(x,y,_z,15);
            if(isSolid(x,y,_z-1) || _z==0)
                break;
            _z--;
        }

        for(int tz=_z; tz <= z; tz++)
            update_envlight_out(x,y,tz);

        return;
    }
    int li = 0;

    int _x,_y,_z, tli;

    //x
    _x = (x+1) & TERRAIN_MAP_WIDTH_BIT_MASK2;
    _y = y;
    _z = z;

    tli = get_envlight(_x,_y,_z);
    if(tli > li )
        li = tli;

    _x = (x-1) & TERRAIN_MAP_WIDTH_BIT_MASK2;
    _y = y;
    _z = z;

    tli = get_envlight(_x,_y,_z);
    if(tli > li )
        li = tli;

    //y
    _x = x;
    _y = (y+1) & TERRAIN_MAP_WIDTH_BIT_MASK2;
    _z = z;

    tli = get_envlight(_x,_y,_z);
    if(tli > li )
        li = tli;

    _x = x;
    _y = (y-1) & TERRAIN_MAP_WIDTH_BIT_MASK2;
    _z = z;

    tli = get_envlight(_x,_y,_z);
    if(tli > li )
        li = tli;

    //z
    _x = x;
    _y = y;
    _z = (z+1) % map_dim.z;

    tli = get_envlight(_x,_y,_z);
    if(tli > li )
        li = tli;

    _x = x;
    _y = y;
    _z = (z+(map_dim.z-1))%map_dim.z;

    tli = get_envlight(_x,_y,_z);
    if(tli > li )
        li = tli;


    if(li != 0)
    {
        set_envlight(x,y,z, li);
        update_envlight_out(x,y,z);
    }
}

void update_envlight_boundary(int _ci, int _cj)
{
    class MAP_CHUNK* mc;
    int ci, cj;

    //north?
    ci = (_ci + 1 +32 ) % 32;
    cj = (_cj + 0 +32 ) % 32;
    mc = main_map->chunk[32*cj + ci];

    if(mc != NULL)
    {
        const int i = 0;
        for(int j=0; j<16; j++)
        for(int k=0; k<map_dim.z; k++)
        {
            if(isSolid(16*ci+i,16*cj+j,k) ) //|| get_envlight(16*ci+i,16*cj+j,k) != 15) // || get_envlight(i,j,k) < 16)
                continue;
            update_envlight_out(16*ci+i,16*cj+j,k);
        }
    }


    //south?
    ci = (_ci + -1 +32 ) % 32;
    cj = (_cj + 0 +32 ) % 32;
    mc = main_map->chunk[32*cj + ci];

    if(mc != NULL)
    {
        const int i = 15;
        for(int j=0; j<16; j++)
        for(int k=0; k<map_dim.z; k++)
        {
            if(isSolid(16*ci+i,16*cj+j,k) ) //|| get_envlight(16*ci+i,16*cj+j,k) != 15) // || get_envlight(i,j,k) < 16)
                continue;
            update_envlight_out(16*ci+i,16*cj+j,k);
        }
    }

    //west?
    ci = (_ci + 0 +32 ) % 32;
    cj = (_cj + 1 +32 ) % 32;
    mc = main_map->chunk[32*cj + ci];

    if(mc != NULL)
    {
        const int j = 0;
        for(int i=0; i<16; i++)
        for(int k=0; k<map_dim.z; k++)
        {
            if(isSolid(16*ci+i,16*cj+j,k) ) //|| get_envlight(16*ci+i,16*cj+j,k) != 15) // || get_envlight(i,j,k) < 16)
                continue;
            update_envlight_out(16*ci+i,16*cj+j,k);
        }
    }


    //east?
    ci = (_ci + 0 +32 ) % 32;
    cj = (_cj + -1 +32 ) % 32;
    mc = main_map->chunk[32*cj + ci];

    if(mc != NULL)
    {
        const int j = 15;
        for(int i=0; i<16; i++)
        for(int k=0; k<map_dim.z; k++)
        {
            if(isSolid(16*ci+i,16*cj+j,k) ) //|| get_envlight(16*ci+i,16*cj+j,k) != 15) // || get_envlight(i,j,k) < 16)
                continue;
            update_envlight_out(16*ci+i,16*cj+j,k);
        }
    }

}


}   // t_map
