/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "dimension.hpp"

#include <entity/components/physics.hpp>
#include <entity/entity/entity.hpp>

namespace Components
{

Vec3 DimensionComponent::get_camera_position()
{
    auto physics = GET_COMPONENT_INTERFACE(Physics, this->entity);
    IF_ASSERT(physics == NULL) return NULL_POSITION;
    Vec3 position = physics->get_position();
    position.z += this->camera_height;
    return position;
}

} // Entities
