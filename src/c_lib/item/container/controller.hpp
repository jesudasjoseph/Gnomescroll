#pragma once

#include <agent/constants.hpp>

#include <item/common/enum.hpp>
#include <item/common/constant.hpp>
#include <item/properties.hpp>

namespace ItemContainer
{

#if DC_CLIENT
// transactions
ContainerActionType alpha_action_decision_tree(int id, int slot);
ContainerActionType beta_action_decision_tree(int id, int slot);

ContainerActionType nanite_alpha_action_decision_tree(int id, int slot);
ContainerActionType nanite_beta_action_decision_tree(int id, int slot);
ContainerActionType nanite_shopping_alpha_action_decision_tree(int id, int slot);
ContainerActionType nanite_shopping_beta_action_decision_tree(int id, int slot);

ContainerActionType craft_input_alpha_action_decision_tree(int id, int slot);
ContainerActionType craft_input_beta_action_decision_tree(int id, int slot);
ContainerActionType craft_output_alpha_action_decision_tree(int id, int slot);
ContainerActionType craft_output_beta_action_decision_tree(int id, int slot);

ContainerActionType no_container_alpha_action_decision_tree();
ContainerActionType no_container_beta_action_decision_tree();

ContainerActionType smelter_alpha_action_decision_tree(int id, int slot);
ContainerActionType smelter_beta_action_decision_tree(int id, int slot);
#endif

#if DC_SERVER
// transactions
ContainerActionType alpha_action_decision_tree(int agent_id, int client_id, int id, int slot);
ContainerActionType beta_action_decision_tree(int agent_id, int client_id, int id, int slot);

ContainerActionType nanite_alpha_action_decision_tree(int agent_id, int client_id, int id, int slot);
ContainerActionType nanite_beta_action_decision_tree(int agent_id, int client_id, int id, int slot);
ContainerActionType nanite_shopping_alpha_action_decision_tree(int agent_id, int client_id, int id, int slot);
ContainerActionType nanite_shopping_beta_action_decision_tree(int agent_id, int client_id, int id, int slot);

ContainerActionType craft_input_alpha_action_decision_tree(int agent_id, int client_id, int id, int slot);
ContainerActionType craft_input_beta_action_decision_tree(int agent_id, int client_id, int id, int slot);
ContainerActionType craft_output_alpha_action_decision_tree(int agent_id, int client_id, int id, int slot);
ContainerActionType craft_output_beta_action_decision_tree(int agent_id, int client_id, int id, int slot);

ContainerActionType no_container_alpha_action_decision_tree(int agent_id, int client_id);
ContainerActionType no_container_beta_action_decision_tree(int agent_id, int client_id);

ContainerActionType smelter_alpha_action_decision_tree(int agent_id, int client_id, int id, int slot);
ContainerActionType smelter_beta_action_decision_tree(int agent_id, int client_id, int id, int slot);
#endif

}