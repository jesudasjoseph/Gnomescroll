#pragma once

#include <c_lib/entity/constants.hpp>
#include <c_lib/entity/component/component.hpp>
#include <c_lib/entity/component/component_list.hpp>
#include <c_lib/entity/components/include.hpp>

namespace Components
{

/*
 * Adding a component:
 *
 * Add type. Create component, either inheriting exisiting interface or creating one
 * If creating new interface, add interface type
 * typedef list from template, declare extern, intern and alloc/free
 * Add subscribe/unsubscribe to switches
 * 
 */

/* ComponentList typedefs */

// TODO - move
 
/* Physics */
const int MAX_PHYSICS_COMPONENTS = 4096;
typedef ComponentList<PositionPhysicsComponent, COMPONENT_POSITION, MAX_PHYSICS_COMPONENTS>
    PositionPhysicsComponentList;
typedef ComponentList<PositionMomentumPhysicsComponent, COMPONENT_POSITION_MOMENTUM, MAX_PHYSICS_COMPONENTS>
    PositionMomentumPhysicsComponentList;
typedef ComponentList<PositionChangedPhysicsComponent, COMPONENT_POSITION_CHANGED, MAX_PHYSICS_COMPONENTS>
    PositionChangedPhysicsComponentList;
typedef ComponentList<PositionMomentumChangedPhysicsComponent, COMPONENT_POSITION_MOMENTUM_CHANGED, MAX_PHYSICS_COMPONENTS>
    PositionMomentumChangedPhysicsComponentList;
typedef ComponentList<VerletPhysicsComponent, COMPONENT_VERLET, MAX_PHYSICS_COMPONENTS>
    VerletPhysicsComponentList;

/* Stackables */
const int MAX_STACKABLE_COMPONENTS = 4096;
typedef ComponentList<StackableComponent, COMPONENT_STACKABLE, MAX_STACKABLE_COMPONENTS>
    StackableComponentList;

/* Drawing */
const int MAX_RENDERABLE_COMPONENTS = 4096;
//typedef ComponentList<BillboardSpriteComponent, COMPONENT_BILLBOARD_SPRITE, MAX_RENDERABLE_COMPONENTS>
    //BillboardSpriteComponentList;
class BillboardSpriteComponentList:
    public CallableComponentList<BillboardSpriteComponent, COMPONENT_BILLBOARD_SPRITE, MAX_RENDERABLE_COMPONENTS>
{};
class ColoredVoxelComponentList:
    public CallableComponentList<ColoredVoxelComponent, COMPONENT_COLORED_VOXEL, MAX_RENDERABLE_COMPONENTS>
{};
class TexturedVoxelComponentList:
    public CallableComponentList<TexturedVoxelComponent, COMPONENT_TEXTURED_VOXEL, MAX_RENDERABLE_COMPONENTS>
{};

/* Pickup */
const int MAX_PICKUP_COMPONENTS = 2048;
typedef ComponentList<PickupComponent, COMPONENT_PICKUP, MAX_PICKUP_COMPONENTS>
    PickupComponentList;

/* Health */
const int MAX_HEALTH_COMPONENTS = 4096;
typedef ComponentList<TTLHealthComponent, COMPONENT_TTL, MAX_HEALTH_COMPONENTS>
    TTLHealthComponentList;
typedef ComponentList<HitPointsHealthComponent, COMPONENT_HIT_POINTS, MAX_HEALTH_COMPONENTS>
    HitPointsHealthComponentList;

/* Team */
const int MAX_TEAM_COMPONENTS = 1024;
typedef ComponentList<TeamComponent, COMPONENT_TEAM, MAX_TEAM_COMPONENTS>
    TeamComponentList;
typedef ComponentList<IndexedTeamComponent, COMPONENT_INDEXED_TEAM, MAX_TEAM_COMPONENTS>
    IndexedTeamComponentList;

/* Owner */
const int MAX_OWNER_COMPONENTS = 4096;
typedef ComponentList<OwnerComponent, COMPONENT_OWNER, MAX_OWNER_COMPONENTS>
    OwnerComponentList;

/* Voxel Model */
const int MAX_VOXEL_MODEL_COMPONENTS = 1024;
typedef ComponentList<VoxelModelCompoent, COMPONENT_VOXEL_MODEL, MAX_VOXEL_MODEL_COMPONENTS>
    VoxelModelComponentList;

/* ComponentList declarations */

extern PositionPhysicsComponentList* position_physics_component_list;
extern PositionMomentumPhysicsComponentList* position_momentum_physics_component_list;
extern PositionChangedPhysicsComponentList* position_changed_physics_component_list;
extern PositionMomentumChangedPhysicsComponentList* position_momentum_changed_physics_component_list;
extern VerletPhysicsComponentList* verlet_physics_component_list;

extern StackableComponentList* stackable_component_list;

extern BillboardSpriteComponentList* billboard_sprite_component_list;
extern ColoredVoxelComponentList* colored_voxel_component_list;
extern TexturedVoxelComponentList* textured_voxel_component_list;

extern PickupComponentList* pickup_component_list;

extern TTLHealthComponentList* ttl_health_component_list;
extern HitPointsHealthComponentList* hit_points_health_component_list;

extern TeamComponentList* team_component_list;
extern IndexedTeamComponentList* indexed_team_component_list;

extern OwnerComponentList* owner_component_list;

extern VoxelModelComponentList* voxel_model_component_list;

/* ComponentList handler switches */

Component* get_switch(ComponentType type);
void release_switch(Component* component);

} // Components
