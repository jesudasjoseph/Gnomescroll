/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "blub.hpp"

#include <entity/entity/entity.hpp>
#include <entity/constants.hpp>
#include <entity/components/health.hpp>
#include <entity/components/dimension.hpp>
#if DC_SERVER
# include <entity/entities/state_machine_actions.hpp>
#endif

namespace Entities
{

void load_mob_blub_data()
{
    EntityType type = ENTITY_MONSTER_BLUE_BLUB;

    entity_data->begin_attaching_to(type);

    (void)ADD_COMPONENT(PositionMomentum);

    auto mob = ADD_COMPONENT(SpriteMob);
    mob->mob.set_type("blue_blub");

    auto dims = ADD_COMPONENT(Dimension);
    dims->set_height(t_mob::get_mob_height(mob->mob.type) * 1.5f);
    dims->set_width(t_mob::get_mob_width(mob->mob.type) * 1.5f);

    auto health = ADD_COMPONENT(HitPoints);
    health->health = 50;
    health->health_max = 50;

    #if DC_SERVER
    auto dest = ADD_COMPONENT(DestinationTargeting);
    dest->sight_range = 15.0f;
    dest->destination_choice_x = 20;
    dest->destination_choice_y = 20;
    dest->speed = 0.1f;
    dest->max_z_diff = 3;

    auto agent = ADD_COMPONENT(AgentTargeting);
    agent->sight_range = 15.0f;
    agent->speed = 0.12f;
    agent->max_z_diff = 3;
    agent->max_lock_ticks = ONE_SECOND * 10;
    agent->proximity_radius = 2.1f;
    agent->jump_force = 0.38f;
    agent->set_jump_cooldowns((3 * ONE_SECOND) / 2, ONE_SECOND * 2);
    agent->attack_rate = (3 * ONE_SECOND) / 2;
    agent->attack_damage = 2;
    agent->jump_near_player = true;

    auto waiting = ADD_COMPONENT(Waiting);
    waiting->wait_time = ONE_SECOND * 3;

    auto limiter = ADD_COMPONENT(RateLimit);
    limiter->limit = MOB_BROADCAST_RATE;

    auto item_drop = ADD_COMPONENT(ItemDrop);
    item_drop->drop->set_max_drop_types(3);
    item_drop->drop->set_max_drop_amounts("synthesizer_coin", 3);
    item_drop->drop->add_drop("synthesizer_coin", 1, 0.3f);
    item_drop->drop->add_drop("synthesizer_coin", 2, 0.1f);
    item_drop->drop->add_drop("synthesizer_coin", 3, 0.05f);
    item_drop->drop->set_max_drop_amounts("plasma_grenade", 10);
    item_drop->drop->add_drop_range("plasma_grenade", 1, 10, 0.8f);
    item_drop->drop->set_max_drop_amounts("led_torch", 5);
    item_drop->drop->add_drop_range("led_torch", 1, 5, 0.2f);

    auto state = ADD_COMPONENT(StateMachine);
    auto conf = state->configuration;
    conf->add_state("waiting", &do_wait);
    conf->add_state("chase_agent", &chase_agent);
    conf->add_state("wander", &in_transit);
    conf->add_transition("waiting", "done_waiting", "wander", &go_to_next_destination);
    conf->add_transition("waiting", "agent_targeted", "chase_agent", NULL);
    conf->add_transition("wander", "agent_targeted", "chase_agent", NULL);
    conf->add_transition("waiting", "agent_attacked", "chase_agent", &target_attacker);
    conf->add_transition("wander", "agent_attacked", "chase_agent", &target_attacker);
    conf->add_transition("wander", "at_destination", "waiting", &begin_wait);
    conf->add_transition("chase_agent", "agent_target_lost", "waiting", &begin_wait);
    conf->set_start_state("waiting");

    auto knockback = ADD_COMPONENT(Knockback);
    knockback->weight = 1.0f;
    #endif

    #if DC_CLIENT
    auto anim = ADD_COMPONENT(Animation);
    anim->color = Color(21, 21, 77);
    anim->count = 25;
    anim->count_max = 40;
    anim->size = 0.2f;
    anim->force = 1.0f;
    #endif
}

} // Entities
