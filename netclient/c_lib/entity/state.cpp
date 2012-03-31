#include "state.hpp"

unsigned int ObjectState::get_kill_reward(int owner, int team)
{
    if (
         (COINS_ANYONE  & this->coin_rule)
     || ((COINS_ENEMIES & this->coin_rule) && (this->team != team))
     || ((COINS_ALLIES  & this->coin_rule) && (this->team == team))
     || ((COINS_OWNER   & this->coin_rule) && (this->owner == owner))
     || ((COINS_ANYONE_WHEN_UNOWNED & this->coin_rule) && (this->owner == NO_AGENT))
    )
        return this->reward;
    return 0;
}

int ObjectState::take_damage(int dmg)
{
    this->died = false;
    if (this->health <= 0) return 0;
    this->health -= dmg;
    this->health = (this->health < 0) ? 0 : this->health;
    if (this->health <= 0)
    {
        this->ttl = this->ttl_max;
        this->died = true;
        this->dead = true;
    }
    return this->health;
}

bool ObjectState::set_position(float x, float y, float z)
{
    bool changed = false;
    if (
       this->position.x != x
    || this->position.y != y
    || this->position.z != z
    ) changed = true;

    if (this->vox != NULL)
    {
        if (this->frozen_vox && changed)
            this->vox->thaw();

        this->position.x = x;
        this->position.y = y;
        this->position.z = z;
        this->vox->update(
            this->position.x, this->position.y, this->position.z,
            this->theta, this->phi
        );

        if (this->frozen_vox)
            this->vox->freeze();
    }

    if (this->vp != NULL)
        this->vp->set_position(x,y,z);
        
    return changed;
}
