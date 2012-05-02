#pragma once

// Common
namespace Toolbelt
{

void init();
void teardown();
void tick();

void trigger_agent_selected_item(ItemID item_id);
void remove_agent(int agent_id);

} // Toolbelt

#if DC_CLIENT
namespace Toolbelt
{

void update_selected_item_type();
void tick_agent_selected_item_type(int agent_id, int item_type);
void trigger_agent_selected_item_type(int agent_id, int item_type);
void tick_local_agent_selected_item_type(int item_type);
void trigger_local_agent_selected_item_type(int item_type);

// network events
void assign_toolbelt(int container_id);

// UI events
void toolbelt_item_selected_event(int container_id, int slot);
void left_trigger_down_event();
void left_trigger_up_event();
void right_trigger_down_event();
void right_trigger_up_event();
void reload_event();

} // Toolbelt
#endif

#if DC_SERVER
namespace Toolbelt
{

void tick_agent_selected_item(ItemID item_id);
void trigger_agent_selected_item(ItemID item_id);

void update_toolbelt_items();

ItemID get_agent_selected_item(int agent_id);
bool set_agent_toolbelt_slot(int agent_id, int slot);

} // Toolbelt
#endif
