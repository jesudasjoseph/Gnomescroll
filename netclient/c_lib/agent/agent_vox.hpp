#pragma once

#include <c_lib/voxel/voxel_model.hpp>
#include <agent/agent_status.hpp>

extern VoxDat agent_vox_dat;
extern VoxDat agent_vox_dat_crouched;

enum AGENT_BODY_PARTS
{
    AGENT_PART_TORSO,
    AGENT_PART_HEAD,
    AGENT_PART_LARM,
    AGENT_PART_RARM,
    AGENT_PART_LLEG,
    AGENT_PART_RLEG
};

const int AGENT_PART_HEAD_DAMAGE  = AGENT_HEALTH;
const int AGENT_PART_TORSO_DAMAGE = AGENT_HEALTH / 2;
const int AGENT_PART_LARM_DAMAGE  = AGENT_HEALTH / 4;
const int AGENT_PART_RARM_DAMAGE  = AGENT_HEALTH / 4;
const int AGENT_PART_LLEG_DAMAGE  = AGENT_HEALTH / 4;
const int AGENT_PART_RLEG_DAMAGE  = AGENT_HEALTH / 4;
