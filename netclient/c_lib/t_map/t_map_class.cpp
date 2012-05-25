#include "t_map_class.hpp"

#include <t_map/t_map.hpp>
#include <t_map/t_properties.hpp>
#include <t_map/_interface.hpp>
#include <t_map/common/constants.hpp>

namespace t_map
{


    static const MAP_ELEMENT NO_MAP_ELEMENT = {{{0}}};
    
    //moved to constants
    //const int TERRAIN_MAP_HEIGHT_BIT_MASK = ~(TERRAIN_MAP_HEIGHT-1);
    //const int TERRAIN_MAP_WIDTH_BIT_MASK = ~(512-1); //assumes map size of 512

    /*
        Constructors
    */
    MAP_CHUNK::MAP_CHUNK(int _xpos, int _ypos)
    {
        //GS_ASSERT( (_xpos % 16) == 0 && (_ypos % 16) == 0 );
        #ifdef DC_CLIENT
            needs_update = false;
        #endif
        xpos = _xpos;
        ypos = _ypos;
        //for(int i=0; i<TERRAIN_CHUNK_WIDTH*TERRAIN_CHUNK_WIDTH*TERRAIN_MAP_HEIGHT;i++) e[i].n = 0;
        for(int i=0; i<TERRAIN_CHUNK_WIDTH*TERRAIN_CHUNK_WIDTH;i++) top_block[i] = 0;

        chunk_index = (ypos / 16)*(MAP_WIDTH/16) + (xpos / 16);
        //printf("xpos,ypos = %i, %i  chunk_index= %i \n", xpos, ypos, chunk_index);
        chunk_item_container.chunk_index = chunk_index;

        memset(e, 0, TERRAIN_CHUNK_WIDTH*TERRAIN_CHUNK_WIDTH*TERRAIN_MAP_HEIGHT*sizeof(struct MAP_ELEMENT) );

    }


    Terrain_map::Terrain_map(int _xdim, int _ydim)
    {
        xdim = _xdim; 
        ydim = _xdim;
        xchunk_dim = xdim/TERRAIN_CHUNK_WIDTH; 
        ychunk_dim = ydim/TERRAIN_CHUNK_WIDTH;

        chunk = new MAP_CHUNK*[xchunk_dim*ychunk_dim];
        for(int i=0; i<xchunk_dim*ychunk_dim; i++) chunk[i] = NULL;

        #if DC_CLIENT
        for (int i=0; i<MAP_WIDTH*MAP_HEIGHT; column_heights[i++] = 0);
        this->reset_heights_read();
        #endif
    }

    Terrain_map::~Terrain_map()
    {
        for(int i=0; i < xchunk_dim*ychunk_dim; i++)
        {
            if(chunk[i] != NULL) delete chunk[i];
        }
        delete[] chunk;
    }


/*
    Get Methods
*/


    int MAP_CHUNK::get_block(int x, int y, int z)
    {
        return e[ (z<<8)+((y&15)<<4)+(x&15) ].block;
    }

    struct MAP_ELEMENT MAP_CHUNK::get_element(int x, int y, int z)
    {
        return e[ (z<<8)+((y&15)<<4)+(x&15) ];
    }

    struct MAP_ELEMENT Terrain_map::get_element(int x, int y, int z)
    {
    #if T_MAP_GET_OPTIMIZED
    /*
        if( ((z & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0) 
            || ((x & TERRAIN_MAP_WIDTH_BIT_MASK) != 0) 
            || ((y & TERRAIN_MAP_WIDTH_BIT_MASK) != 0) 
        ) return NO_MAP_ELEMENT;
    */

        if( ((z & TERRAIN_MAP_HEIGHT_BIT_MASK) | (x & TERRAIN_MAP_WIDTH_BIT_MASK)
            | (y & TERRAIN_MAP_WIDTH_BIT_MASK)) != 0 
        ) return NO_MAP_ELEMENT;

        struct MAP_CHUNK* c;

        c = chunk[ MAP_CHUNK_WIDTH*(y >> 4) + (x >> 4) ];
        //c = chunk[ (y | ~15) + (x >> 4)];
        if(c == NULL) return NO_MAP_ELEMENT;
        //return c->e[(TERRAIN_CHUNK_WIDTH*TERRAIN_CHUNK_WIDTH)*z+ TERRAIN_CHUNK_WIDTH*(y | 15) + (x | 15)];

        return c->e[ (z<<8)+((y&15)<<4)+(x&15) ];
    #else

        if( z >= TERRAIN_MAP_HEIGHT || z < 0 ) return NO_MAP_ELEMENT;
        if( x >= 512 || x < 0 ) return NO_MAP_ELEMENT;
        if( y >= 512 || y < 0 ) return NO_MAP_ELEMENT;

        struct MAP_CHUNK* c;
        {
            int xchunk = (x >> 4);
            int ychunk = (y >> 4);
            c = chunk[ MAP_CHUNK_WIDTH*ychunk + xchunk ];
            if( c == NULL ) return NO_MAP_ELEMENT;
        }

        int xi = x & 15; //bit mask
        int yi = y & 15; //bit mask

        return c->e[TERRAIN_CHUNK_WIDTH*TERRAIN_CHUNK_WIDTH*z+ TERRAIN_CHUNK_WIDTH*yi + xi];
    #endif
    }

#if DC_CLIENT
    void Terrain_map::set_update(int x, int y)
    {
        //printf("set update: %i %i \n", x,y);

        if( ((x & TERRAIN_MAP_WIDTH_BIT_MASK) != 0) 
            ||  ((y & TERRAIN_MAP_WIDTH_BIT_MASK) != 0) 
        ) return;
        struct MAP_CHUNK* c;
        c = chunk[ MAP_CHUNK_WIDTH*(y >> 4) + (x >> 4) ];
        if( c == NULL ) return;
        c->needs_update = true;
    }
#endif

/*
    Set Methods 
*/
    void Terrain_map::set_element(int x, int y, int z, struct MAP_ELEMENT element)
    {
        //printf("set element: %i %i %i \n", x,y,z);
        //printf("set: %i %i %i %i \n", x,y,element.block);
    #if T_MAP_SET_OPTIMIZED
    /*
        if( ((z & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0) 
            || ((x & TERRAIN_MAP_WIDTH_BIT_MASK) != 0) 
            || ((y & TERRAIN_MAP_WIDTH_BIT_MASK) != 0) 
        ) return;
    */

        if( ((z & TERRAIN_MAP_HEIGHT_BIT_MASK) | (x & TERRAIN_MAP_WIDTH_BIT_MASK)
            | (y & TERRAIN_MAP_WIDTH_BIT_MASK)) != 0 
        ) return; // an error

        struct MAP_CHUNK* c;
        c = chunk[ MAP_CHUNK_WIDTH*(y >> 4) + (x >> 4) ];
        //c = chunk[ (y | ~15) + (x >> 4)];
        if( c != NULL )
        {
            c = new MAP_CHUNK( x & ~15, y & ~15);
            chunk[ MAP_CHUNK_WIDTH*(y >> 4) + (x >> 4) ] = c;
        }
        //c->e[(TERRAIN_CHUNK_WIDTH*TERRAIN_CHUNK_WIDTH)*z+ TERRAIN_CHUNK_WIDTH*(y | 15) + (x | 15)] = element;
        c->e[ (z << 8)+ ((y & 15) <<4) + (x & 15)] = element;

        #ifdef DC_CLIENT
            c->needs_update = true; 

            if((x & 15) == 0)  set_update(x-1,y);
            if((x & 15) == 15) set_update(x+1,y);
            if((y & 15) == 0)  set_update(x,y-1);
            if((y & 15) == 15) set_update(x,y+1);
        #endif
    #else
        //printf("set: %i %i %i \n",  x,y,z);

        if( z >= TERRAIN_MAP_HEIGHT || z < 0 ) return;
        if( x >= 512 || x < 0 ) return;
        if( y >= 512 || y < 0 ) return;
        //printf("set %i, %i, %i \n", x,y,z);
        struct MAP_CHUNK* c;
        {
            int xchunk = (x >> 4);
            int ychunk = (y >> 4);
            //printf("chunk= %i, %i \n", xchunk, ychunk);
    
            c = chunk[ MAP_CHUNK_WIDTH*ychunk + xchunk ];
            //printf("chunk index= %i \n", x & ~15, y & ~15, MAP_CHUNK_WIDTH*ychunk + xchunk );

            if( c == NULL )
            {
                //printf("new map chunk= %i %i \n", x & ~15, y & ~15 );
                c = new MAP_CHUNK( x & ~15, y & ~15);
                chunk[ MAP_CHUNK_WIDTH*ychunk + xchunk ] = c;
                
            }
        }

        int xi = x & 15; //bit mask
        int yi = y & 15; //bit mask
        //printf("xi= %i, yi= %i, z= %i \n", xi,yi,z );
        //printf("index2 = %i \n", TERRAIN_CHUNK_WIDTH*TERRAIN_CHUNK_WIDTH*z+ TERRAIN_CHUNK_WIDTH*yi + xi);

        c->e[TERRAIN_CHUNK_WIDTH*TERRAIN_CHUNK_WIDTH*z+ TERRAIN_CHUNK_WIDTH*yi + xi] = element;

        #ifdef DC_CLIENT
            c->needs_update = true; 

            if((x & 15) == 0)  set_update(x-1,y);
            if((x & 15) == 15) set_update(x+1,y);
            if((y & 15) == 0)  set_update(x,y-1);
            if((y & 15) == 15) set_update(x,y+1);
        #endif

    #endif
    }

    int Terrain_map::apply_damage(int x, int y, int z, int dmg)
    {
        //printf("set: %i %i %i %i \n", x,y,element.block);
    #if T_MAP_SET_OPTIMIZED
        if (dmg <= 0) return -4;
        /*
        if( ((z & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0) 
            || ((x & TERRAIN_MAP_WIDTH_BIT_MASK) != 0) 
            ||  ((y & TERRAIN_MAP_WIDTH_BIT_MASK) != 0) 
        ) return -2; //an error
        */

        if( ((z & TERRAIN_MAP_HEIGHT_BIT_MASK) | (x & TERRAIN_MAP_WIDTH_BIT_MASK)
            | (y & TERRAIN_MAP_WIDTH_BIT_MASK)) != 0 
        ) return -2; // an error

        struct MAP_CHUNK* c;
        c = chunk[ MAP_CHUNK_WIDTH*(y >> 4) + (x >> 4) ];
        if( c != NULL ) return -3;

        struct MAP_ELEMENT* e = &c->e[ (z<<8)+((y&15)<<4)+(x&15) ];

        if(e->block == 0) return -1;
        e->damage += dmg;
        if(e->damage >= maxDamage(e->block) ) 
        {
            #if DC_SERVER
            destroy_item_container_block(x,y,z);
            #endif
            // destroy block
            *e = NO_MAP_ELEMENT; 
            
            #ifdef DC_CLIENT
                c->needs_update = true; 

                if((x & 15) == 0)  set_update(x-1,y);
                if((x & 15) == 15) set_update(x+1,y);
                if((y & 15) == 0)  set_update(x,y-1);
                if((y & 15) == 15) set_update(x,y+1);
            #endif

            return 0;
        } 
        else 
        {
            return e->damage;
        }

    #else

        if (dmg <= 0) return -4;

        if( z >= TERRAIN_MAP_HEIGHT || z < 0 ) return -2;
        if( x >= 512 || x < 0 ) return -2 ;
        if( y >= 512 || y < 0 ) return -2;
        //printf("set %i, %i, %i \n", x,y,z);
        struct MAP_CHUNK* c;
        {
            int xchunk = (x >> 4);
            int ychunk = (y >> 4);
            //printf("chunk= %i, %i \n", xchunk, ychunk);
    
            c = chunk[ MAP_CHUNK_WIDTH*ychunk + xchunk ];
            //printf("chunk index= %i \n", x & ~15, y & ~15, MAP_CHUNK_WIDTH*ychunk + xchunk );

            if( c == NULL ) return -3;
        }

        int xi = x & 15; //bit mask
        int yi = y & 15; //bit mask
        //printf("xi= %i, yi= %i, z= %i \n", xi,yi,z );
        //printf("index2 = %i \n", TERRAIN_CHUNK_WIDTH*TERRAIN_CHUNK_WIDTH*z+ TERRAIN_CHUNK_WIDTH*yi + xi);

        struct MAP_ELEMENT* e =  &c->e[TERRAIN_CHUNK_WIDTH*TERRAIN_CHUNK_WIDTH*z+ TERRAIN_CHUNK_WIDTH*yi + xi];
        
        if(e->block == 0) return -1;
        e->damage += dmg;
        if(e->damage >= maxDamage(e->block) ) 
        {
            #if DC_SERVER
            destroy_item_container_block(x,y,z);
            #endif
            // destroy block
            *e = NO_MAP_ELEMENT; 

            #ifdef DC_CLIENT
                c->needs_update = true; 

                if((x & 15) == 0)  set_update(x-1,y);
                if((x & 15) == 15) set_update(x+1,y);
                if((y & 15) == 0)  set_update(x,y-1);
                if((y & 15) == 15) set_update(x,y+1);
            #endif

            return 0;
        } 
        else 
        {
            return e->damage;
        }

    #endif
    }

    int Terrain_map::apply_damage(int x, int y, int z, int dmg, int* block_type)
    {
        //printf("set: %i %i %i %i \n", x,y,element.block);
    #if T_MAP_SET_OPTIMIZED
        if (dmg <= 0) return -4;

        /*
        if( ((z & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0) 
            || ((x & TERRAIN_MAP_WIDTH_BIT_MASK) != 0) 
            ||  ((y & TERRAIN_MAP_WIDTH_BIT_MASK) != 0) 
        ) return -2; //an error
        */

        if( ((z & TERRAIN_MAP_HEIGHT_BIT_MASK) | (x & TERRAIN_MAP_WIDTH_BIT_MASK)
            | (y & TERRAIN_MAP_WIDTH_BIT_MASK)) != 0 
        ) return -2; // an error

        struct MAP_CHUNK* c;
        c = chunk[ MAP_CHUNK_WIDTH*(y >> 4) + (x >> 4) ];
        if( c != NULL ) return -3;

        struct MAP_ELEMENT* e = &c->e[ (z<<8)+((y&15)<<4)+(x&15) ];

        if(e->block == 0) return -1;
        e->damage += dmg;
        if(e->damage >= maxDamage(e->block) ) 
        {
            #if DC_SERVER
            destroy_item_container_block(x,y,z);
            #endif
            // destroy block
            *e = NO_MAP_ELEMENT; 
            
            #ifdef DC_CLIENT
                c->needs_update = true; 

                if((x & 15) == 0)  set_update(x-1,y);
                if((x & 15) == 15) set_update(x+1,y);
                if((y & 15) == 0)  set_update(x,y-1);
                if((y & 15) == 15) set_update(x,y+1);

            #endif

            return 0;
        } 
        else 
        {
            return e->damage;
        }

    #else

        if (dmg <= 0) return -4;

        if( z >= TERRAIN_MAP_HEIGHT || z < 0 ) return -2;
        if( x >= 512 || x < 0 ) return -2 ;
        if( y >= 512 || y < 0 ) return -2;
        //printf("set %i, %i, %i \n", x,y,z);
        struct MAP_CHUNK* c;
        {
            int xchunk = (x >> 4);
            int ychunk = (y >> 4);
            //printf("chunk= %i, %i \n", xchunk, ychunk);
    
            c = chunk[ MAP_CHUNK_WIDTH*ychunk + xchunk ];
            //printf("chunk index= %i \n", x & ~15, y & ~15, MAP_CHUNK_WIDTH*ychunk + xchunk );

            if( c == NULL ) return -3;
        }

        int xi = x & 15; //bit mask
        int yi = y & 15; //bit mask
        //printf("xi= %i, yi= %i, z= %i \n", xi,yi,z );
        //printf("index2 = %i \n", TERRAIN_CHUNK_WIDTH*TERRAIN_CHUNK_WIDTH*z+ TERRAIN_CHUNK_WIDTH*yi + xi);

        struct MAP_ELEMENT* e =  &c->e[TERRAIN_CHUNK_WIDTH*TERRAIN_CHUNK_WIDTH*z+ TERRAIN_CHUNK_WIDTH*yi + xi];
        
        *block_type = e->block;

        if(e->block == 0) return -1;
        e->damage += dmg;
        if(e->damage >= maxDamage(e->block) ) 
        {
            #if DC_SERVER
            destroy_item_container_block(x,y,z);
            #endif
            // destroy block
            *e = NO_MAP_ELEMENT; 

            #ifdef DC_CLIENT
                c->needs_update = true; 

                if((x & 15) == 0)  set_update(x-1,y);
                if((x & 15) == 15) set_update(x+1,y);
                if((y & 15) == 0)  set_update(x,y-1);
                if((y & 15) == 15) set_update(x,y+1);
            #endif

            return 0;
        } 
        else 
        {
            return e->damage;
        }

    #endif
    }


    #if DC_CLIENT
    void Terrain_map::reset_heights_read()
    {   // call when heights ar edone being read
        this->height_changed = false;
        for (int i=0; i<MAP_CHUNK_HEIGHT*MAP_CHUNK_WIDTH; chunk_heights_changed[i++] = false);
    }
    
    void Terrain_map::chunk_received(int cx, int cy)
    {   // update chunk/column heights based on this chunk
        int highest = -1;
        int h;
        int xoff = cx * TERRAIN_CHUNK_WIDTH;
        int yoff = cy * TERRAIN_CHUNK_WIDTH;
        MAP_CHUNK* m = this->chunk[cx + MAP_CHUNK_WIDTH*cy];
        int x,y;

        for (int i=0; i<TERRAIN_CHUNK_WIDTH; i++)
            for (int j=0; j<TERRAIN_CHUNK_WIDTH; j++)
            {
                x = xoff+i;
                y = yoff+j;

                for (h=map_dim.z-1; h>=0; h--)
                    if (isSolid(m->get_block(x,y,h))) break;
                h += 1;
                
                if (h > highest) highest = h;
                this->column_heights[x + y*MAP_WIDTH] = h;
            }
            
        this->chunk_heights_changed[cx + cy*MAP_CHUNK_WIDTH] = true;
        this->height_changed = true;

        #if DC_CLIENT
        if(cx+1 < MAP_CHUNK_WIDTH)
        {
            if(chunk[ MAP_CHUNK_WIDTH*(cy) + cx+1 ] != NULL)
                chunk[ MAP_CHUNK_WIDTH*(cy) + cx+1 ]->needs_update = true;
        }
        if(cx-1 >= 0)
        {
            if(chunk[ MAP_CHUNK_WIDTH*(cy) + cx-1 ] != NULL)
                chunk[ MAP_CHUNK_WIDTH*(cy) + cx-1 ]->needs_update = true;
        }
        if(cy+1 < MAP_CHUNK_WIDTH)
        {
            if(chunk[ MAP_CHUNK_WIDTH*(cy+1) + cx ] != NULL)
                chunk[ MAP_CHUNK_WIDTH*(cy+1) + cx ]->needs_update = true;
        }
        if(cy-1 >= 0)
        {
            if(chunk[ MAP_CHUNK_WIDTH*(cy-1) + cx ] != NULL)
                chunk[ MAP_CHUNK_WIDTH*(cy-1) + cx ]->needs_update = true;
        }
        #endif

    }
    
    inline unsigned char Terrain_map::get_cached_height(int x, int y)
    {
        return this->column_heights[x + y*MAP_WIDTH];
    }

    void Terrain_map::update_heights(int x, int y, int z, int val)
    {
        z += 1; // heights are not 0 indexed;
        int new_h = -1;
        unsigned char h = this->column_heights[x + y*MAP_WIDTH];
        if (val != 0)
        {   // setting higher block
            if (z > h)
                new_h = z;
        }
        else
        {
            if (z >= h) // deleting top block
                new_h = _get_highest_solid_block(x,y,z) + 1;
        }

        if (new_h < 0) return; // no change in height
        this->column_heights[x + y*MAP_WIDTH] = new_h;

        int cx = x / TERRAIN_CHUNK_WIDTH;   // truncate
        int cy = y / TERRAIN_CHUNK_WIDTH;
        this->chunk_heights_changed[cx + cy*MAP_CHUNK_WIDTH] = true;
        this->height_changed = true;
    }
    #endif

/*
    Block Methodss
*/
    int Terrain_map::get_block(int x, int y, int z)
    {
        //return get_element(x,y,z).block;

        if( ((z & TERRAIN_MAP_HEIGHT_BIT_MASK) | (x & TERRAIN_MAP_WIDTH_BIT_MASK)
            | (y & TERRAIN_MAP_WIDTH_BIT_MASK)) != 0 
        ) return 0;

        struct MAP_CHUNK* c = chunk[ MAP_CHUNK_WIDTH*(y >> 4) + (x >> 4) ];
        if(c == NULL) return 0;
        //return c->e[(TERRAIN_CHUNK_WIDTH*TERRAIN_CHUNK_WIDTH)*z+ TERRAIN_CHUNK_WIDTH*(y | 15) + (x | 15)];

        return c->e[ (z<<8)+((y&15)<<4)+(x&15) ].block;
    }

    void Terrain_map::set_block(int x, int y, int z, int value)
    {
        struct MAP_ELEMENT element = {{{value, 0,0,0}}};
        set_element(x,y,z, element);
        #if DC_CLIENT
        update_heights(x,y,z,value);
        #endif
    }


/*
    void Terrain_map::reset_chunk_container_blocks(int chunk_index)
    {
        struct MAP_CHUNK* c=chunk[chunk_index];
        if(c == NULL) GS_ABORT();
        c->chunk_item_container._reset();
    }
*/
}
