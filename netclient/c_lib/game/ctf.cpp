#include "ctf.hpp"


#include <c_lib/agent/player_agent.hpp>
#include <c_lib/state/client_state.hpp>
#include <c_lib/game/packets.hpp>
#include <common/random.h>

void CTF::init() {
    none.init(0);
    one.init(1);
    two.init(2);

    one.set_color(10,10,10);    //black-ish
    two.set_color(10,210,10);   // green-ish

    char team_name_one[] = "Raiders";
    one.set_name(team_name_one);

    char team_name_two[] = "Ex-military";
    two.set_name(team_name_two);
}

void CTF::start() {
    float x,y,z;

    // TODO use real map widths
    int x_max = 128;
    int y_max = 128;

    x = randrange(0, x_max-1);
    y = randrange(0, (y_max-1)/2);
    z = _get_highest_open_block(x,y);
    x += randf();y += randf();
    this->set_base_position(1, x,y,z);
    x = randrange(0, x_max-1);
    y = randrange(0, (y_max-1)/2);
    z = _get_highest_open_block(x,y);
    x += randf();y += randf();
    this->set_flag_position(1, x,y,z);

    x = randrange(0, x_max-1);
    y = randrange((y_max-1)/2, y_max-1);
    z = _get_highest_open_block(x,y);
    x += randf();y += randf();
    this->set_base_position(2,x,y,z);
    x = randrange(0, x_max-1);
    y = randrange((y_max-1)/2, y_max-1);
    z = _get_highest_open_block(x,y);
    x += randf();y += randf();
    this->set_flag_position(2,x,y,z);
}

void CTF::set_score(int team, int score) {
    if (team != 1 && team != 2) {
        printf("CTF::set_score, team %d invalid\n", team);
    }

    switch (team) {
        case 1:
            one.score = score;
            break;
        case 2:
            two.score = score;
            break;
        default:
            break;
    }
    printf("Set team %d score to %d\n", team, score);
}

void CTF::set_flag_position(int team, float x, float y, float z)
{
    switch (team) {
        case 1:
            one.flag->set_position(x,y,z);
            break;
        case 2:
            two.flag->set_position(x,y,z);
            break;
        default:
            break;
    }

    #ifdef DC_SERVER
    FlagState_StoC msg;
    msg.team = team;
    msg.x = x;
    msg.y = y;
    msg.z = z;
    msg.broadcast();
    #endif
}

void CTF::set_base_position(int team, float x, float y, float z)
{
    switch (team) {
        case 1:
            one.base->set_position(x,y,z);
            break;
        case 2:
            two.base->set_position(x,y,z);
            break;
        default:
            break;
    }

    #ifdef DC_SERVER
    BaseState_StoC msg;
    msg.team = team;
    msg.x = x;
    msg.y = y;
    msg.z = z;
    msg.broadcast();
    #endif
}

#ifdef DC_CLIENT

void CTF::join_team(int team) {
    AgentJoinTeam_CtoS msg;
    msg.team = team;
    msg.agent = ClientState::playerAgent_state.agent_id;
    msg.send();
}

void CTF::on_ready() {
    static int once = 0;
    if (once) return;
    if (!auto_assign) return;
    AgentAutoAssignTeam_CtoS msg;
    msg.agent = ClientState::playerAgent_state.agent_id;
    msg.send();
    once = 1;
}

#endif

#ifdef DC_SERVER

void CTF::on_client_connect(int client_id) {
    one.update_client(client_id);
    two.update_client(client_id);
}

void CTF::auto_assign_agent(int agent_id) {
    // choose the less populated team
    printf("Attempt to auto assign agent %d to team\n", agent_id);
    CTFTeam* team = (one.n > two.n) ? &two : &one;
    bool added = false;
    if (!team->full()) {
        added = add_agent_to_team(team->id, agent_id);
    }
    if (added) {
        AgentJoinTeam_StoC msg;
        msg.team = team->id;
        msg.agent = agent_id;
        msg.broadcast();
    }
    printf("Team one: %d, team two: %d\n", one.n, two.n);
}

void CTF::send_to_client(int client_id)
{
    // send scores TODO

    // send flag/base states
    Flag* flag;
    Base* base;
    int i;
    for (i=1; i<=2; i++) {
        switch (i) {
            case 1:
                flag = one.flag;
                base = one.base;
                break;
            case 2:
                flag = two.flag;
                base = two.base;
                break;
            default:
                break;
        }
        FlagState_StoC flag_msg;
        flag_msg.team = i;
        flag_msg.x = flag->x;
        flag_msg.y = flag->y;
        flag_msg.z = flag->z;
        flag_msg.sendToClient(client_id);
        
        BaseState_StoC base_msg;
        base_msg.team = i;
        base_msg.x = base->x;
        base_msg.y = base->y;
        base_msg.z = base->z;
        base_msg.sendToClient(client_id);
    }
}

#endif

bool CTF::add_agent_to_team(int team, int agent) {
    bool success = false;

    if (team < 0 || team > 2) {
        printf("CTF::add_agent_to_team -- team id %d out of range\n", team);
        return success;
    }

    Agent_state* a = STATE::agent_list.get(agent);
    if (a==NULL) return success;

    if (a->status.team == team) {
        return success;    // already in team
    }


    switch(a->status.team) {
        case 0:
            none.remove_agent(agent);
            break;
        case 1:
            one.remove_agent(agent);
            break;
        case 2:
            two.remove_agent(agent);
            break;
        default:
            break;
    }
    
    switch(team) {
        case 0:
            success = none.add_agent(agent);
            break;
        case 1:
            success = one.add_agent(agent);
            break;
        case 2:
            success = two.add_agent(agent);
            break;
        default:
            break;
    }

    a->status.team = team;
    printf("Added agent %d to team %d\n", agent, team);
    return success;
}

void CTF::set_team_color(int team,
    unsigned char r, unsigned char g, unsigned char b) {
    if (team < 1 || team > 2) {
        printf("CTF::set_team_color -- team id %d out of range\n", team);
        return;
    }

    switch (team) {
        case 1:
            one.set_color(r,g,b);
            break;
        case 2:
            two.set_color(r,g,b);
            break;
        default:
            break;
    }
}
