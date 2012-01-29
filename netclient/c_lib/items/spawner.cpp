#include "spawner.hpp"

#include <c_lib/common/random.h>
#include <math.h>

#include <c_lib/agent/net_agent.hpp>

VoxDat spawner_vox_dat;

void Spawner::get_spawn_point(int spawn[3], int agent_height)
{
    int x,y;
    x = (int)this->x;
    y = (int)this->y;

    int sx,sy;
    sx = randrange(x-spawn_radius, x+spawn_radius);
    sy = randrange(y-spawn_radius, y+spawn_radius);
    spawn[0] = (sx < map_dim.x) ? sx : map_dim.x;
    spawn[0] = (spawn[0] < 0) ? 0 : spawn[0];
    spawn[1] = (sy < map_dim.y) ? sy : map_dim.y;
    spawn[1] = (spawn[1] < 0) ? 0 : spawn[1];
    spawn[2] = _get_highest_open_block(spawn[0], spawn[1], agent_height);
}

void Spawner::set_owner(int owner)
{
    this->owner = owner;
}

void Spawner::set_team(int team)
{
    this->team = team;
    if (this->vox != NULL) this->vox->update_team_color(&spawner_vox_dat, this->team);
}

void Spawner::init_vox()
{
    if (this->team == 0) printf("WARNING Spawner::init_vox() -- team not set\n");
    this->vox = new Voxel_model(&spawner_vox_dat, this->id, this->type, this->team);
    this->vox->set_hitscan(true);
    #ifdef DC_CLIENT
    this->vox->set_draw(true);
    #endif
    this->vox->update(&spawner_vox_dat, this->x, this->y, this->z, this->theta, this->phi);
}

Spawner::Spawner(int id)
:
id(id),
team(0),
owner(0),
type(OBJ_TYPE_SPAWNER),
x(0), y(0), z(0),
theta(0), phi(0),
spawn_radius(SPAWNER_RADIUS),
vox(NULL)
{}
Spawner::Spawner(int id, float x, float y, float z)
:
id(id),
team(0),
owner(0),
type(OBJ_TYPE_SPAWNER),
x(x), y(y), z(z),
theta(0), phi(0),
spawn_radius(SPAWNER_RADIUS),
vox(NULL)
{}

void Spawner::create_message(Spawner_create_StoC* msg)
{
    #ifdef DC_SERVER
    msg->id = this->id;
    msg->team = this->team;
    msg->owner = this->owner;
    msg->x = this->x;
    msg->y = this->y;
    msg->z = this->z;
    #endif
}

Spawner::~Spawner()
{
    if (this->vox != NULL) delete this->vox;
}

//#ifdef DC_CLIENT
//inline void Spawner_create_StoC::handle()
//{
    //ClientState::spawner_list.create(id, x,y,z);
//}
//inline void Spawner_destroy_StoC::handle()
//{
    //ClientState::spawner_list.destroy(id);
//}
//#endif

//#ifdef DC_SERVER
//inline void Spawner_create_StoC::handle() {}
//inline void Spawner_destroy_StoC::handle() {}
//#endif
