#include "spawner.hpp"

#include <physics/motion.hpp>
#include <entity/object/object.hpp>
#include <entity/object/helpers.hpp>
#include <entity/constants.hpp>
#include <entity/objects/fabs/constants.hpp>
#include <entity/components/physics/position.hpp>
#include <entity/components/voxel_model.hpp>
#include <voxel/vox_dat_init.hpp>
#include <agent/_interface.hpp>

namespace Entities
{

void load_agent_spawner_data()
{
    EntityType type = ENTITY_AGENT_SPAWNER;

    entity_data->begin_attaching_to(type);

    entity_data->attach_component(type, COMPONENT_Position);
    entity_data->attach_component(type, COMPONENT_Dimension);
    entity_data->attach_component(type, COMPONENT_VoxelModel);
    entity_data->attach_component(type, COMPONENT_HitPoints);

    #if DC_SERVER
    entity_data->attach_component(type, COMPONENT_AgentSpawner);
    entity_data->attach_component(type, COMPONENT_ItemDrop);
    entity_data->attach_component(type, COMPONENT_RateLimit);
    #endif

    #if DC_CLIENT
    entity_data->attach_component(type, COMPONENT_Animation);
    #endif
}

static void set_agent_spawner_properties(Entity* object)
{
    ADD_COMPONENT(Position, object);

    auto dims = ADD_COMPONENT(Dimension, object);
    dims->height = AGENT_SPAWNER_HEIGHT;

    auto vox = ADD_COMPONENT(VoxelModel, object);
    vox->vox_dat = &VoxDats::agent_spawner;
    vox->init_hitscan = AGENT_SPAWNER_INIT_WITH_HITSCAN;
    vox->init_draw = AGENT_SPAWNER_INIT_WITH_DRAW;

    auto health = ADD_COMPONENT(HitPoints, object);
    health->health = AGENT_SPAWNER_MAX_HEALTH;
    health->health_max = AGENT_SPAWNER_MAX_HEALTH;

    #if DC_SERVER
    auto spawner = ADD_COMPONENT(AgentSpawner, object);
    spawner->radius = AGENT_SPAWNER_SPAWN_RADIUS;

    auto limiter = ADD_COMPONENT(RateLimit, object);
    limiter->limit = MOB_BROADCAST_RATE;

    auto item_drop = ADD_COMPONENT(ItemDrop, object);
    item_drop->drop.set_max_drop_types(1);
    item_drop->drop.set_max_drop_amounts("agent_spawner", 1);
    item_drop->drop.add_drop("agent_spawner", 1, 1.0f);
    #endif

    #if DC_CLIENT
    auto anim = ADD_COMPONENT(Animation, object);
    anim->count = AGENT_SPAWNER_ANIMATION_COUNT;
    anim->count_max = AGENT_SPAWNER_ANIMATION_COUNT_MAX;
    anim->size = AGENT_SPAWNER_ANIMATION_SIZE;
    anim->force = AGENT_SPAWNER_ANIMATION_FORCE;
    anim->color = AGENT_SPAWNER_ANIMATION_COLOR;
    #endif

    object->tick = &tick_agent_spawner;
    object->update = &update_agent_spawner;

    object->create = create_packet;
    object->state = state_packet;
}

Entity* create_agent_spawner()
{
    EntityType type = ENTITY_AGENT_SPAWNER;
    Entity* obj = entity_list->create(type);
    GS_ASSERT(obj != NULL);
    if (obj == NULL) return NULL;
    set_agent_spawner_properties(obj);
    return obj;
}

void ready_agent_spawner(Entity* object)
{

    auto vox = GET_COMPONENT_INTERFACE(VoxelModel, object);
    auto physics = GET_COMPONENT_INTERFACE(Physics, object);

    Vec3 position = physics->get_position();
    Vec3 angles = physics->get_angles();

    vox->ready(position, angles.x, angles.y);
    vox->freeze();

    #if DC_SERVER
    object->broadcastCreate();
    #endif
}

void die_agent_spawner(Entity* object)
{
    #if DC_SERVER
    auto item_drop = GET_COMPONENT_INTERFACE(ItemDrop, object);
    GS_ASSERT(item_drop != NULL);
    item_drop->drop_item();

    // remove self from any agents using us
    using Agents::agent_list;
    for (size_t i=0; i<agent_list->max; i++)
        if (agent_list->objects[i].id != agent_list->null_id)
            if (agent_list->objects[i].status.spawner == object->id)
                agent_list->objects[i].status.set_spawner(BASE_SPAWN_ID);

    object->broadcastDeath();
    #endif

    #if DC_CLIENT
    // explosion animation
    auto vox = GET_COMPONENT_INTERFACE(VoxelModel, object);
    if (vox != NULL && vox->vox != NULL)
    {
        auto anim = GET_COMPONENT_INTERFACE(Animation, object);
        if (anim != NULL)
            anim->explode_random(vox->get_center());
    }

    //dieChatMessage(object);
    #endif
}

void tick_agent_spawner(Entity* object)
{
    #if DC_SERVER
    auto physics = GET_COMPONENT_INTERFACE(Physics, object);
    Vec3 position = physics->get_position();
    position.z = stick_to_terrain_surface(position);
    physics->set_position(position);
    auto limiter = GET_COMPONENT_INTERFACE(RateLimit, object);
    if (limiter->allowed()) object->broadcastState();
    #endif
}

void update_agent_spawner(Entity* object)
{
    auto physics = GET_COMPONENT_INTERFACE(Physics, object);
    auto vox = GET_COMPONENT_INTERFACE(VoxelModel, object);

    Vec3 angles = physics->get_angles();
    Vec3 pos = physics->get_position();
    vox->force_update(pos, angles.x, angles.y, physics->get_changed());
    physics->set_changed(false);  // reset changed state
}

} // Entities
