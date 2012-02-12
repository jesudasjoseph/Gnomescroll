#pragma once
#ifdef DC_SERVER

#include <c_lib/agent/agent_list.hpp>
#include <c_lib/particles/particle_lib.hpp>
#include <c_lib/game/ctf.hpp>
#include <c_lib/monsters/monsters.hpp>
#include <c_lib/voxel/voxel_hitscan.hpp>
#include <c_lib/items/spawner.hpp>

namespace ServerState
{
    extern Agent_list agent_list;
    extern Cspray_list cspray_list;
    extern Grenade_list grenade_list;
    //extern Neutron_list neutron_list;
    extern Monsters::Slime_list slime_list;
    extern Voxel_hitscan_list voxel_hitscan_list;
    extern Spawner_list spawner_list;
    
    extern CTF ctf;
    
    void init();

    void damage_objects_within_sphere(
        float x, float y, float z, float radius,
        int dmg, int owner, Object_types inflictor_type
    );

    void server_tick();

    void send_game_state_to_client(int client_id);
    void send_id_to_client(int client_id);

    void add_player_to_chat(int client_id);
    void remove_player_from_chat(int client_id);

}
//class NewClient
//{
    //public:
        //int client_id;
        //bool inited;
        //bool loaded;

        //void ready()
        //{
            //if (!this->inited) return;
            //if (this->loaded) return;
            
            //Agent_state* a = agent_list.get(client_id);
            //if (a==NULL) return;
            //if (!a->status.identified) return;

            //this->loaded = true;
            //send_game_state_to_client(this->client_id);
        //}

        //void init(int client_id)
        //{
            //if (this->inited) return;

            //this->inited = true;
            //this->client_id = client_id;

            //Agent_state* a = agent_list.create(client_id);
            //a->send_to_client(client_id);
            //a->add_player_to_chat(client_id);
        //}

        //void teardown()
        //{
            //remove_player_from_chat(this->client_id);
            //ctf.remove_agent_from_team(this->client_id);
            //agent_list.destroy(client_id);
        //}
    

        //NewClient()
        //:
        //client_id(-1),
        //inited(false),
        //loaded(false)
        //{}
        
//};

#endif
