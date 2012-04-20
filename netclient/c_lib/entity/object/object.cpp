#include "object.hpp"

#include <c_lib/entity/constants.hpp>
#include <c_lib/entity/component/component.hpp>
#include <c_lib/entity/network/packets.hpp>
#include <c_lib/entity/components/draw/billboard_sprite.hpp>
#include <c_lib/entity/components/pickup.hpp>
#include <c_lib/entity/components/health/ttl.hpp>

namespace Objects
{

using Components::Component;

void Object::add_component(Component* component)
{
    // retrieve component of type from pool
    // call subscribe
    this->components[this->n_components++] = component;
}

// returns component of type if available
// must cast to desired component manually
Component* Object::get_component(ComponentType type)
{
    for (int i=0; i<this->n_components; i++)
        if (this->components[i]->type == type)
            return this->components[i];
    return NULL;
}

Component* Object::get_component_interface(ComponentInterfaceType interface)
{
    for (int i=0; i<this->n_components; i++)
        if (this->components[i]->interface == interface)
            return this->components[i];
    return NULL;
}

void Object::broadcastDeath()
{
    object_destroy_StoC msg;
    msg.id = this->id;
    msg.type = this->type;
    msg.broadcast();
}

void Object::init(int n_components)
{
    this->components = (Component**)malloc(n_components * sizeof(Component*));
}

Object::~Object()
{
    if (this->components != NULL) free(this->components);
}

} // Objects