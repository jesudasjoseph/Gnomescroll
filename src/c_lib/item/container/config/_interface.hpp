/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#include <item/container/config/types.hpp>
#include <item/container/config/names.hpp>
#include <item/container/config/_state.hpp>

namespace ItemContainer
{

/* Public Attribute Accessors */

ItemContainerType get_type(const char* name);
const char* get_compatible_name(const char* name);
class ContainerAttribute* get_attr(const char* name);
class ContainerAttribute* get_attr(ItemContainerType type);
unsigned int get_container_max_slots(ItemContainerType type);
int get_container_xdim(ItemContainerType type);
int get_container_ydim(ItemContainerType type);
int get_container_alt_max_slots(ItemContainerType type);
int get_container_alt_xdim(ItemContainerType type);
int get_container_alt_ydim(ItemContainerType type);
bool container_type_is_attached_to_agent(ItemContainerType type);
bool container_type_is_block(ItemContainerType type);
const char* get_container_name(ItemContainerType type);
const char* get_container_display_name(ItemContainerType type);
containerCreate get_container_create_function(ItemContainerType type);

/* Configuration Loader */

void init_config();
void teardown_config();
void load_config();
void end_config();

bool is_valid_container_name(const char* name);

}   // ItemContainer
