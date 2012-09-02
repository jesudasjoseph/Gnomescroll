#pragma once

#include <item/config/_interface.hpp>

#include <t_map/t_properties.hpp>
#include <t_map/common/types.hpp>

namespace Item
{

void verify_item_dat();

void load_item_dat()
{
    start_item_dat();
    int i0 = texture_alias("media/sprites/item/i00.png");
    int i1 = texture_alias("media/sprites/item/i01.png");
    //int i2 = texture_alias("media/sprites/item/i02.png");

    int p0 = texture_alias("media/sprites/item/p00.png");

    item_def(0, IG_ERROR, "error_item");
    s.pretty_name = (char*) "Error";
    sprite_def(i0, 4,1);

    item_def(1, IG_PLACER, "regolith");
    iso_block_sprite_def("regolith");
    s.block_type_id = t_map::dat_get_cube_id("regolith");
    s.pretty_name = (char*) "Regolith";
    s.max_stack_size = 64;
    s.particle_voxel = true;
    s.particle_voxel_texture = t_map::get_cube_primary_texture_index("regolith");
    s.cube_height = 1;

    item_def(2, IG_PLACER, "rock");
    iso_block_sprite_def("rock");
    s.block_type_id = t_map::dat_get_cube_id("rock");
    s.pretty_name = (char*) "Rock";
    s.max_stack_size = 64;
    s.particle_voxel = true;
    s.particle_voxel_texture = t_map::get_cube_primary_texture_index("rock");
    s.cube_height = 1;
    
    item_def(4, IG_RESOURCE, "quartz_crystal");
    sprite_def(i0, 1,5);
    s.pretty_name = (char*) "Quartz Crystal";


    item_def(5, IG_HITSCAN_WEAPON, "laser_rifle");
    sprite_def(i1, 5,8);
    s.pretty_name = (char*) "Laser Rifle";
    s.hitscan_fire_cooldown = 30;
    s.hitscan_damage = 5;
    s.hitscan_bullet_effect_enum = 0;
    s.max_durability = 100;
    s.max_stack_size = 1;
    s.object_damage_min = 15;
    s.object_damage_max = 25;
    block_damage_def(2);
    block_damage_def(CUBE_MATERIAL_DIRT, 4);
    block_damage_def(CUBE_MATERIAL_STONE, 0);

    item_def(7, IG_MINING_LASER, "blue_mining_laser");
    sprite_def(i0, 3,5);
    s.pretty_name = (char*) "Mining Laser";
    s.max_durability = 1200;
    s.max_stack_size = 1;
    s.firing_range = 4.0f;
    s.firing_rate = 6;
    block_damage_def(6);
    block_damage_def(CUBE_MATERIAL_DIRT, 5);
    block_damage_def(CUBE_MATERIAL_STONE, 6);
    block_damage_def(CUBE_MATERIAL_DECORATION, 8);
    s.object_damage_min = 15;
    s.object_damage_max = 20;
    s.animation_id = 0;

    item_def(8, IG_MINING_LASER, "red_mining_laser");
    sprite_def(i0, 3,7);
    s.pretty_name = (char*) "Mining Laser";
    s.max_durability = 1200;
    s.max_stack_size = 1;
    s.firing_range = 4.0f;
    s.firing_rate = 6;
    block_damage_def(6);
    block_damage_def(CUBE_MATERIAL_DIRT, 6);
    block_damage_def(CUBE_MATERIAL_STONE, 8);
    block_damage_def(CUBE_MATERIAL_DECORATION, 12);
    s.object_damage_min = 20;
    s.object_damage_max = 25;
    s.animation_id = 1;

    item_def(9, IG_MINING_LASER, "green_mining_laser");
    sprite_def(i0, 4,7);
    s.pretty_name = (char*) "Mining Laser";
    s.max_durability = 1200;
    s.max_stack_size = 1;
    s.firing_range = 4.0f;
    s.firing_rate = 6;
    block_damage_def(6);
    block_damage_def(CUBE_MATERIAL_DIRT, 7);
    block_damage_def(CUBE_MATERIAL_STONE, 10);
    block_damage_def(CUBE_MATERIAL_DECORATION, 16);
    s.object_damage_min = 25;
    s.object_damage_max = 30;
    s.animation_id = 2;

    item_def(10, IG_GRENADE_LAUNCHER, "plasma_grenade");
    sprite_def(i0, 1,7);
    s.pretty_name = (char*) "Plasma Grenade";
    s.max_stack_size = 100;

    item_def(11, IG_SYNTHESIZER_COIN, "synthesizer_coin");
    sprite_def(i1, 2, 7);
    s.pretty_name = (char*) "Synthesizer Chip";
    s.max_stack_size = 999;

    item_def(12, IG_DEBUG, "location_pointer");
    sprite_def(i0, 4,2);
    s.pretty_name = (char*) "Location Pointer";

    item_def(13, IG_DEBUG, "block_placer");
    sprite_def(i0, 4,5);
    s.pretty_name = (char*) "Block Placer";
    s.cube_height = 1;
    
    item_def(14, IG_UNKNOWN, "unknown");
    sprite_def(i0, 5,1);
    s.pretty_name = (char*) "Unknown";

    //item_def(15, IG_PLACER, "crate_1");
    //iso_block_sprite_def("crate_1");
    //s.block_type_id = t_map::dat_get_cube_id("crate_1");
    //s.max_stack_size = 1;

    // small storage container
    item_def(16, IG_PLACER, "small_storage");
    iso_block_sprite_def("small_storage");
    s.block_type_id = t_map::dat_get_cube_id("small_storage");
    s.pretty_name = (char*) "Small Storage Block";
    s.max_stack_size = 1;
    s.particle_voxel = true;
    s.particle_voxel_texture = t_map::get_cube_primary_texture_index("small_storage");
    s.container_type = CONTAINER_TYPE_STORAGE_BLOCK_SMALL;
    container_block_def("small_storage", CONTAINER_TYPE_STORAGE_BLOCK_SMALL);
    s.cube_height = 1;

    // level 1 utility crafting block
    item_def(17, IG_PLACER, "small_crafting_bench");
    iso_block_sprite_def("small_crafting_bench");
    s.pretty_name = (char*) "Small Crafting Bench";
    s.block_type_id = t_map::dat_get_cube_id("small_crafting_bench");
    s.max_stack_size = 1;
    s.particle_voxel = true;
    s.particle_voxel_texture = t_map::get_cube_primary_texture_index("small_crafting_bench");
    s.container_type = CONTAINER_TYPE_CRAFTING_BENCH_UTILITY;
    container_block_def("small_crafting_bench", CONTAINER_TYPE_CRAFTING_BENCH_UTILITY);
    s.cube_height = 1;

    // Copper


    item_def(32, IG_RESOURCE, "copper_ore");
    sprite_def(i1, 3,3);
    s.pretty_name = (char*) "Copper Ore";
    s.max_stack_size = 64;


    item_def(33, IG_RESOURCE, "copper_bar");
    sprite_def(i1, 3,2);
    s.pretty_name = (char*) "Copper Bar";
    s.max_stack_size = 64;

    item_def(34, IG_SHOVEL, "copper_shovel");
    sprite_def(i1, 3,1);
    s.pretty_name = (char*) "Copper Shovel";
    s.melee_fire_cooldown = 250;
    s.melee_damage = 2;
    s.max_durability = 400;
    s.max_stack_size = 1;
    s.firing_range = 4.0f;
    s.firing_rate = 6;
    block_damage_def(1);
    block_damage_def(CUBE_MATERIAL_DIRT, 4);
    block_damage_def(CUBE_MATERIAL_STONE, 1);
    block_damage_def(CUBE_MATERIAL_DECORATION, 4);
    s.object_damage_min = 15;
    s.object_damage_max = 25;

    // Iron
    item_def(38, IG_RESOURCE, "iron_ore");
    sprite_def(i0, 5,7);
    s.pretty_name = (char*) "Iron Ore";
    s.max_stack_size = 64;

    item_def(39, IG_RESOURCE, "iron_bar");
    sprite_def(i1, 1,2);
    s.pretty_name = (char*) "Iron Bar";
    s.max_stack_size = 64;

    item_def(40, IG_SHOVEL, "iron_shovel");
    sprite_def(i1, 1,1);
    s.pretty_name = (char*) "Iron Shovel";
    s.melee_fire_cooldown = 250;
    s.melee_damage = 6;
    s.max_durability = 400;
    s.max_stack_size = 1;
    s.firing_range = 4.0f;
    s.firing_rate = 6;
    block_damage_def(2);
    block_damage_def(CUBE_MATERIAL_DIRT, 5);
    block_damage_def(CUBE_MATERIAL_STONE, 1);
    block_damage_def(CUBE_MATERIAL_DECORATION, 6);
    s.object_damage_min = 25;
    s.object_damage_max = 35;

    // Gallium
    item_def(35, IG_RESOURCE, "gallium_ore");
    sprite_def(i1, 2,3);
    s.pretty_name = (char*) "Gallium Ore";
    s.max_stack_size = 64;

    item_def(36, IG_RESOURCE, "gallium_bar");
    sprite_def(i1, 2,2);
    s.pretty_name = (char*) "Gallium Bar";
    s.max_stack_size = 64;

    item_def(37, IG_SHOVEL, "gallium_shovel");
    sprite_def(i1, 2,1);
    s.pretty_name = (char*) "Gallium Shovel";
    s.melee_fire_cooldown = 250;
    s.melee_damage = 4;
    s.max_durability = 400;
    s.max_stack_size = 1;
    s.firing_range = 4.0f;
    s.firing_rate = 6;
    block_damage_def(3);
    block_damage_def(CUBE_MATERIAL_DIRT, 6);
    block_damage_def(CUBE_MATERIAL_STONE, 1);
    block_damage_def(CUBE_MATERIAL_DECORATION, 8);
    s.object_damage_min = 20;
    s.object_damage_max = 30;

    // Iridium
    item_def(41, IG_RESOURCE, "iridium_ore");
    sprite_def(i1, 4,3);
    s.pretty_name = (char*) "Iridium Ore";
    s.max_stack_size = 64;

    item_def(42, IG_RESOURCE, "iridium_bar");
    sprite_def(i1, 4,2);
    s.pretty_name = (char*) "Iridium Bar";
    s.max_stack_size = 64;

    item_def(43, IG_SHOVEL, "iridium_shovel");
    sprite_def(i1, 4,1);
    s.pretty_name = (char*) "Iridium Shovel";
    s.melee_fire_cooldown = 250;
    s.melee_damage = 30;
    s.max_durability = 400;
    s.max_stack_size = 1;
    s.firing_range = 4.0f;
    s.firing_rate = 6;
    block_damage_def(3);
    block_damage_def(CUBE_MATERIAL_DIRT, 8);
    block_damage_def(CUBE_MATERIAL_STONE, 2);
    block_damage_def(CUBE_MATERIAL_DECORATION, 8);
    s.object_damage_min = 30;
    s.object_damage_max = 40;

    item_def(44, IG_FIST, "fist");
    sprite_def(i1, 6, 2);
    s.pretty_name = (char*) "Fist";
    s.firing_range = 4.0f;
    s.firing_rate = 6;
    block_damage_def(1);
    block_damage_def(CUBE_MATERIAL_DIRT, 2);
    block_damage_def(CUBE_MATERIAL_DECORATION, 4);
    s.object_damage_min = 5;
    s.object_damage_max = 10;

    item_def(45, IG_RESOURCE, "coal");
    sprite_def(i0, 5,2);
    s.pretty_name = (char*) "Coal";
    s.max_stack_size = 64;
    s.fuel = true;
    s.fuel_burn_rate = 30 * 30; // 30 seconds
    
    item_def(46, IG_PLACER, "steel_block_1");
    iso_block_sprite_def("steel_block_1");
    s.pretty_name = (char*) "Steel Block #1";
    s.block_type_id = t_map::dat_get_cube_id("steel_block_1");
    s.max_stack_size = 16;
    s.particle_voxel = true;
    s.particle_voxel_texture = t_map::get_cube_primary_texture_index("steel_block_1");
    s.cube_height = 1;

    item_def(47, IG_PLACER, "steel_block_2");
    s.pretty_name = (char*) "Steel block #2";
    iso_block_sprite_def("steel_block_2");
    s.block_type_id = t_map::dat_get_cube_id("steel_block_2");
    s.max_stack_size = 16;
    s.particle_voxel = true;
    s.particle_voxel_texture = t_map::get_cube_primary_texture_index("steel_block_2");
    s.cube_height = 1;

    item_def(48, IG_PLACER, "steel_block_3");
    s.pretty_name = (char*) "Steel Block #3";
    iso_block_sprite_def("steel_block_3");
    s.block_type_id = t_map::dat_get_cube_id("steel_block_3");
    s.max_stack_size = 16;
    s.particle_voxel = true;
    s.particle_voxel_texture = t_map::get_cube_primary_texture_index("steel_block_3");
    s.cube_height = 1;

    item_def(49, IG_CONSUMABLE, "small_charge_pack");
    sprite_def(i1, 6,1);
    s.pretty_name = (char*) "Small Charge Pack";
    s.max_stack_size = 16;
    s.repair_agent_amount = 25;

    item_def(50, IG_PLACER, "cryofreezer_1");
    iso_block_sprite_def("cryofreezer_1");
    s.pretty_name = (char*) "Small Cryofreezer";
    s.block_type_id = t_map::dat_get_cube_id("cryofreezer_1");
    s.max_stack_size = 1;
    s.particle_voxel = true;
    s.particle_voxel_texture = t_map::get_cube_primary_texture_index("cryofreezer_1");
    s.container_type = CONTAINER_TYPE_CRYOFREEZER_SMALL;
    container_block_def("cryofreezer_1", CONTAINER_TYPE_CRYOFREEZER_SMALL);
    s.cube_height = 1;

    item_def(51, IG_RESOURCE, "methane_ice");
    sprite_def(i0, 1,4);
    s.pretty_name = (char*) "Methane Ice";
    s.max_stack_size = 16;
    s.gas = true;
    s.gas_lifetime = 30 * 60 * 5;   // 5 minutes
    s.fuel = true;
    s.fuel_burn_rate = 30 * 30; // 30 seconds

    item_def(52, IG_PLACER, "smelter_1");
    iso_block_sprite_def("smelter_1");
    s.pretty_name = (char*) "Simple Smelter";
    s.block_type_id = t_map::dat_get_cube_id("smelter_1");
    s.max_stack_size = 1;
    s.particle_voxel = true;
    s.particle_voxel_texture = t_map::get_cube_primary_texture_index("smelter_1");
    s.container_type = CONTAINER_TYPE_SMELTER_ONE;
    container_block_def("smelter_1", CONTAINER_TYPE_SMELTER_ONE);
    s.cube_height = 1;

    item_def(53, IG_PLACER, "control_node");
    iso_block_sprite_def("control_node");
    s.pretty_name = (char*) "Control Node";
    s.block_type_id = t_map::dat_get_cube_id("control_node");
    s.max_stack_size = 8;
    s.particle_voxel = true;
    s.particle_voxel_texture = t_map::get_cube_primary_texture_index("control_node");
    s.cube_height = 1;

    item_def(54, IG_ENERGY_TANK, "energy_tank");
    sprite_def(i0, 2,8);
    s.pretty_name = (char*) "Energy Tank";
    s.max_stack_size = 1;

    item_def(55, IG_AGENT_SPAWNER, "agent_spawner");
    sprite_def(i1, 2,6);
    s.pretty_name = (char*) "Spawner";
    s.max_stack_size = 1;
    s.cube_height = 2;

    item_def(56, IG_ENERGY_CORE, "energy_core");
    sprite_def(i1, 3,6);
    s.pretty_name = (char*) "Energy Core";
    s.max_stack_size = 1;
    s.cube_height = 1;

    item_def(57, IG_MECH_PLACER, "crystal_seed");
    sprite_def(i1, 5,1);
    s.mech_type_id = t_mech::get_mech_type_id("red_crystal");
    s.pretty_name = (char*) "Crystal Seed";
    s.max_stack_size = 8;

    item_def(58, IG_MECH_PLACER, "crystal_seed2");
    sprite_def(i1, 5,2);
    s.mech_type_id = t_mech::get_mech_type_id("blue_crystal");
    s.pretty_name = (char*) "Crystal Seed";
    s.max_stack_size = 8;

    item_def(59, IG_PLACER, "crusher");
    iso_block_sprite_def("crusher");
    s.pretty_name = (char*) "Crusher";
    s.block_type_id = t_map::dat_get_cube_id("crusher");
    s.max_stack_size = 1;
    s.particle_voxel = true;
    s.particle_voxel_texture = t_map::get_cube_primary_texture_index("crusher");
    s.container_type = CONTAINER_TYPE_CRUSHER;
    container_block_def("crusher", CONTAINER_TYPE_CRUSHER);
    s.cube_height = 1;

    item_def(60, IG_RESOURCE, "powdered_regolith");
    sprite_def(i0, 4,6);
    s.pretty_name = (char*) "Powdered Regolith";
    s.max_stack_size = 64;


    item_def(61, IG_RESOURCE, "iron_rod");
    sprite_def(i1, 1,7);
    s.pretty_name = (char*) "Iron Rod";
    s.max_stack_size = 1;

    item_def(62, IG_RESOURCE, "iron_star");
    sprite_def(i1, 4,7);
    s.pretty_name = (char*) "Iron Star";
    s.max_stack_size = 1;

    item_def(63, IG_RESOURCE, "iron_blade");
    sprite_def(i1, 4,6);
    s.pretty_name = (char*) "Iron Blade";
    s.max_stack_size = 1;

    item_def(64, IG_RESOURCE, "copper_wire");
    sprite_def(i1, 1,6);
    s.pretty_name = (char*) "Copper Wire";
    s.max_stack_size = 1;


    item_def(65, IG_RESOURCE, "iron_helmet");
    sprite_def(i1, 5,3);
    s.pretty_name = (char*) "Iron Helmet";
    s.max_stack_size = 1;

    item_def(66, IG_RESOURCE, "gallium_helmet");
    sprite_def(i1, 5,4);
    s.pretty_name = (char*) "Gallium Helmet";
    s.max_stack_size = 1;

    item_def(67, IG_RESOURCE, "copper_helmet");
    sprite_def(i1, 5,5);
    s.pretty_name = (char*) "Copper Helmet";
    s.max_stack_size = 1;

    item_def(68, IG_RESOURCE, "iridium_helmet");
    sprite_def(i1, 5,6);
    s.pretty_name = (char*) "Iridium Helmet";
    s.max_stack_size = 1;

    item_def(69, IG_RESOURCE, "blue_crystal");
    sprite_def(i1, 3,4);
    s.pretty_name = (char*) "Crystal";
    s.max_stack_size = 16;

    item_def(70, IG_RESOURCE, "red_crystal");
    sprite_def(i0, 1,3);
    s.pretty_name = (char*) "Crystal";
    s.max_stack_size = 16;

    item_def(71, IG_RESOURCE, "green_crystal");
    sprite_def(i0, 2,4);
    s.pretty_name = (char*) "Crystal";
    s.max_stack_size = 16;

	item_def(72, IG_MECH_PLACER, "acadia_seed");
    sprite_def(p0, 1,2);
    s.mech_type_id = t_mech::get_mech_type_id("acadia_flower");
    s.pretty_name = (char*) "Acadia Seed";
    s.max_stack_size = 64;

    item_def(73, IG_RESOURCE, "acadia_fruit");
    sprite_def(p0, 1,9);
    s.max_stack_size = 16;
    s.pretty_name = (char*) "Acadia Fruit";

    item_def(74, IG_MECH_PLACER, "blob_seed");
    sprite_def(p0, 3,2);
    s.mech_type_id = t_mech::get_mech_type_id("blob_flower");
    s.pretty_name = (char*) "Blob Seed";
    s.max_stack_size = 64;

	end_item_dat(); // finalize
    
    verify_item_dat();
}

void verify_item_dat()
{
    for (int i=0; i<MAX_ITEMS; i++)
    {
        if (item_attribute_array[i].item_type == NULL_ITEM_TYPE) continue;

        // make sure group is set
        GS_ASSERT(item_attribute_array[i].group != IG_NONE);
    
        GS_ASSERT(item_attribute_array[i].pretty_name != NULL);
        if (item_attribute_array[i].pretty_name != NULL)
            GS_ASSERT(item_attribute_array[i].pretty_name[0] != '\0');

        // make sure all data types are within bounds
        GS_ASSERT(item_attribute_array[i].max_energy > 0
                && item_attribute_array[i].max_energy <= 0xffff);
                
        GS_ASSERT(item_attribute_array[i].max_durability > 0
                && item_attribute_array[i].max_durability <= 0xffff);
                
        GS_ASSERT(item_attribute_array[i].max_stack_size > 0
                && item_attribute_array[i].max_stack_size <= 0xffff);

        // Energy should not be in use yet.
        GS_ASSERT(item_attribute_array[i].max_energy == NULL_ENERGY);

        // particle voxel texture shouldn't be set if its not a particle voxel -- likely mistake
        GS_ASSERT(item_attribute_array[i].particle_voxel || (!item_attribute_array[i].particle_voxel && item_attribute_array[i].particle_voxel_texture == 0));
        // particle voxel should have a cube height of 1
        GS_ASSERT(!item_attribute_array[i].particle_voxel || item_attribute_array[i].cube_height == 1);

        // gas lifetime should be set, if it is a gas
        GS_ASSERT(!item_attribute_array[i].gas || item_attribute_array[i].gas_lifetime != NULL_GAS_LIFETIME);
    }
}


int _item_cube_iso_spritesheet_id = -1;

void start_item_dat()
{
    #if DC_CLIENT
    _item_cube_iso_spritesheet_id = LUA_load_item_texture(t_map::block_item_16_surface);
    #endif
}

void end_item_dat()
{
    _set_attribute();
    
    #if DC_CLIENT
    LUA_save_item_texture();
    #endif
}

void iso_block_sprite_def(const char* block_name)
{
    #if DC_CLIENT
    if (_item_cube_iso_spritesheet_id == -1)
    {
        printf("Error: iso_block_sprite_def, must call start_item_dat!!!\n");
        GS_ABORT();
    }

    int id = t_map::dat_get_cube_id(block_name);

    int xpos = id % 16;
    int ypos = id / 16;

    int index = LUA_blit_item_texture(_item_cube_iso_spritesheet_id, xpos+1, ypos+1);
    sprite_array[_current_item_id] = index; //check
    #endif
}

}   // Item
