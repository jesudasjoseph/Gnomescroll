#include "client.hpp"

#if DC_SERVER
dont_include_this_file_in_server
#endif

namespace Item
{

static uint16_t container_event_id = 0;
int container_event[CONTAINER_EVENT_MAX];

static void send_container_alpha_action(ContainerActionType action, int container_id, int slot)
{
    assert(action != CONTAINER_ACTION_NONE);
    
    container_event_id += 1;
    container_event_id %= CONTAINER_EVENT_MAX;
    container_event[container_event_id] = container_id;
    
    container_action_alpha_CtoS msg;
    msg.event_id = container_event_id;
    msg.action = action;

    msg.container_id = container_id;
    msg.slot = slot;

    msg.hand_type = player_hand_type_ui;
    msg.hand_stack = player_hand_stack_ui;

    if (action == FULL_HAND_TO_WORLD)
    {
        msg.slot_type = NULL_ITEM_TYPE;
        msg.slot_stack = 1;
    }
    else
    {
        ItemContainerUIInterface* container = get_container_ui(container_id);
        assert(container != NULL);

        msg.slot_type = container->get_slot_type(slot);
        msg.slot_stack = container->get_slot_stack(slot);    
    }

    msg.send();
}

static void send_container_beta_action(ContainerActionType action, int container_id, int slot)
{
    assert(container_id != NULL_CONTAINER);
    assert(action != CONTAINER_ACTION_NONE);
    
    container_event_id += 1;
    container_event_id %= CONTAINER_EVENT_MAX;
    container_event[container_event_id] = container_id;

    container_action_beta_CtoS msg;
    msg.event_id = container_event_id;
    msg.action = action;

    msg.container_id = container_id;
    msg.slot = slot;
    
    msg.hand_type = player_hand_type_ui;
    msg.hand_stack = player_hand_stack_ui;

    if (action == FULL_HAND_TO_WORLD)
    {
        msg.slot_type = NULL_ITEM_TYPE;
        msg.slot_stack = 1;
    }
    else
    {
        ItemContainerUIInterface* container = get_container_ui(container_id);
        assert(container != NULL);

        msg.slot_type = container->get_slot_type(slot);
        msg.slot_stack = container->get_slot_stack(slot);    
    }
    
    msg.send();
}

void mouse_left_click_handler(int container_id, int slot, bool nanite, bool craft_output)
{
    ContainerActionType action = alpha_action_decision_tree(container_id, slot);
    if (nanite) action = nanite_alpha_action_decision_tree(container_id, slot);
    else action = alpha_action_decision_tree(container_id, slot);
    if (action != CONTAINER_ACTION_NONE) send_container_alpha_action(action, container_id, slot);
}

void mouse_right_click_handler(int container_id, int slot, bool nanite, bool craft_output)
{
    ContainerActionType action;
    if (nanite) action = nanite_beta_action_decision_tree(container_id, slot);
    else action = beta_action_decision_tree(container_id, slot);
    if (action != CONTAINER_ACTION_NONE) send_container_beta_action(action, container_id, slot);
}

}
