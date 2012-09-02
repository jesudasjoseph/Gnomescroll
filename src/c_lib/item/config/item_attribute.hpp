#pragma once

#include <t_map/common/constants.hpp>

namespace Item
{
    class ItemAttribute
    {
        public:

        ItemGroup group;

        int item_type;
    
        char* pretty_name;

        //IG_PLACER
        int block_type_id;   //id of block type that it creates

        //IG_MECH_PLACER_PLAYER
        int mech_type_id;

        //IG_HITSCAN_WEAPON
        int hitscan_fire_cooldown;      // ms per bullet
        int hitscan_damage;             // damage
        int hitscan_bullet_effect_enum; // bullet effect

        //IG_MELEE_WEAPON
        int melee_fire_cooldown;
        int melee_damage;

        // weapons
        float firing_range;
        int firing_rate;
        int block_damage[t_map::MAX_CUBES];

        int object_damage;       // if this is nonzero it will be used, otherwise randrange will be used
        int object_damage_min;  // for randrange(min,max);
        int object_damage_max;
    
    
        // All items
        int max_energy;
        int max_durability;
        int max_stack_size;

        // particle rendering
        bool particle_voxel;
        int particle_voxel_texture;
    
        // health
        unsigned int repair_agent_amount;

        bool gas;
        int gas_lifetime;
        
        bool fuel;
        int fuel_burn_rate; // in ticks
    
        ItemContainerType container_type;

        int cube_height;

        // animation
        int animation_id;

        ItemAttribute()
        {
            this->item_type = NULL_ITEM_TYPE;
            this->group = IG_NONE;
        }

        void load_defaults(int item_type, ItemGroup group)
        {
            // PUT ALL DEFAULTS HERE
            this->group = group;
            this->item_type = item_type;

            block_type_id = -1; // error value
            mech_type_id = -1;  // error value

            pretty_name = NULL;
            max_energy = NULL_ENERGY;
            max_durability = NULL_DURABILITY;
            max_stack_size = 1;
            particle_voxel = false;
            particle_voxel_texture = 0;
            firing_range = DEFAULT_FIRING_RANGE;
            firing_rate = 6;
            // match defaults with fist
            for (int i=0; i<t_map::MAX_CUBES; i++)
            {
                if (t_map::get_cube_material(i) == CUBE_MATERIAL_DIRT)
                    this->block_damage[i] = 2;
                else
                if (t_map::get_cube_material(i) == CUBE_MATERIAL_DECORATION)
                    this->block_damage[i] = 4;
                else
                    this->block_damage[i] = 1;
            }
            
            object_damage = 0;
            object_damage_min = 5;
            object_damage_max = 10;
            repair_agent_amount = 0;
            container_type = CONTAINER_TYPE_NONE;
            gas = false;
            gas_lifetime = NULL_GAS_LIFETIME;
            fuel = false;
            fuel_burn_rate = 30;
            cube_height = 0;
            animation_id = 0;
        }
    };

}
