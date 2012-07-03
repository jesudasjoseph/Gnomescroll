#pragma once

#include <voxel/voxel_hitscan.hpp>
#include <entity/constants.hpp>
#include <physics/vec3.hpp>

namespace Hitscan {
    
typedef enum {
    HITSCAN_TARGET_NONE,
    HITSCAN_TARGET_VOXEL,
    HITSCAN_TARGET_BLOCK
} HitscanTargetTypes;

struct HitscanBlock
{
    int hit;
    int x,y,z;
    float distance;
    int side[3];
    int tile;
};

HitscanBlock* ray_intersect_block(float x, float y, float z, float vx, float vy, float vz);

//int terrain(float x, float y, float z, float vx, float vy, float vz, int pos[3], float *distance);
HitscanTargetTypes terrain(float x, float y, float z, float vx, float vy, float vz, int pos[3], float *distance, int side[3], int *tile);

// for agents hitscanning strictly agents:
int against_agents(Vec3 position, Vec3 direction, float max_distance, int firing_agent_id);
// for mobs hitscanning strictly agents:
int against_agents(Vec3 position, Vec3 direction, float max_distance);

HitscanTargetTypes hitscan_against_world(
    Vec3 p, Vec3 v,
    int ignore_id, ObjectType ignore_type,    // inputs
    struct Voxel_hitscan_target* target, float* vox_distance, float collision_point[3],
    int block_pos[3], int side[3], int* tile, float* block_distance // outputs
);

}
