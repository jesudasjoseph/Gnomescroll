
#include "net_agent.hpp"


#include <net_lib/common/message_handler.h>
#include <c_lib/agent/agent.hpp>

/*
    agent control state message
*/

#define Agent_control_state_message_id 3
#define Agent_control_state_message_size 2*sizeof(uint8_t)+2*sizeof(uint16_t)+3*sizeof(uint32_t)

int handle_agent_control_state_message(unsigned char* buff, int buff_n) {
    Agent_control_state_message cs;
    int r = cs.deserialize(buff, buff_n); //should pass in pointer for number of ints read

    //printf("control state received: id=%i, seq=%i \n", cs.id, cs.seq);
    return r;
}

typedef int (*pt2handler)(unsigned char*, int);
void Agent_control_state_message_register_message() {
    register_message_handler(Agent_control_state_message_id, Agent_control_state_message_size, (pt2handler) &handle_agent_control_state_message);
    printf("Registering handler for agent control state input: message_id= %i, message_size= %i\n", Agent_control_state_message_id, Agent_control_state_message_size);
}

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;

Agent_control_state_message::Agent_control_state_message() {
    id = 3;
    seq = 0;
}

void Agent_control_state_message::send_message() {
    unsigned char* buff= NetClient::get_client_out_buffer();
    int* buff_n = NetClient::get_client_out_buffer_n();

    int bcount = *buff_n;
    if(*buff_n > 800) {
        printf("Cannot send message: output buffer is full! %i bytes\n", *buff_n);
        return;
    }

    PACK_uint8_t(Agent_control_state_message_id, buff, buff_n);  //push message id on stack
    PACK_uint16_t(id, buff, buff_n); //agent id
    ///assume agent id is part of state?
    PACK_uint8_t(seq, buff, buff_n);
    PACK_uint16_t(tick%65536, buff, buff_n);
    PACK_uint32_t(cs, buff, buff_n);
    PACK_float(theta, buff, buff_n);
    PACK_float(phi, buff, buff_n);

    seq++;
    //printf("Agent_control_state_message::send_message: message size= %i bytes\n", *buff_n - bcount);
}

int Agent_control_state_message::deserialize(unsigned char* buff, int buff_n) {
    //PACK_uint8_t(3, buff, buff_n);  //push message id on stack
    int _buff_n = buff_n;
    int msg_id = UPACK_uint8_t(buff, &buff_n); //msg id, not used
    id = UPACK_uint16_t(buff, &buff_n); //agent id
    seq = UPACK_uint8_t(buff, &buff_n);
    tick =UPACK_uint16_t(buff, &buff_n);
    cs = UPACK_uint32_t(buff, &buff_n);
    theta = UPACK_float(buff, &buff_n);
    phi = UPACK_float(buff, &buff_n);

    return buff_n - _buff_n;
}

/*
    agent state message
*/

#define Agent_state_message_id 4
#define Agent_state_message_size 2*sizeof(uint8_t)+2*sizeof(uint16_t)+3*sizeof(uint32_t)

int handle_agent_state_message(unsigned char* buff, int buff_n) {
    Agent_control_state_message cs;
    int r = cs.deserialize(buff, buff_n); //should pass in pointer for number of ints read

    //printf("control state received: id=%i, seq=%i \n", cs.id, cs.seq);
    return r;
}

typedef int (*pt2handler)(unsigned char*, int);
void Agent_state_message_register_message() {
    register_message_handler(Agent_control_state_message_id, Agent_control_state_message_size, (pt2handler) &handle_agent_control_state_message);
    printf("Registering handler for agent control state input: message_id= %i, message_size= %i\n", Agent_control_state_message_id, Agent_control_state_message_size);
}

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;

Agent_state_message::Agent_control_state_message() {
    id = 3;
    seq = 0;
}

void Agent_state_message::send_message() {
    unsigned char* buff= NetClient::get_client_out_buffer();
    int* buff_n = NetClient::get_client_out_buffer_n();

    int bcount = *buff_n;
    if(*buff_n > 800) {
        printf("Cannot send message: output buffer is full! %i bytes\n", *buff_n);
        return;
    }

    PACK_uint8_t(Agent_control_state_message_id, buff, buff_n);  //push message id on stack
    PACK_uint16_t(id, buff, buff_n); //agent id
    ///assume agent id is part of state?
    PACK_uint8_t(seq, buff, buff_n);
    PACK_uint16_t(tick%65536, buff, buff_n);
    PACK_uint32_t(cs, buff, buff_n);
    PACK_float(theta, buff, buff_n);
    PACK_float(phi, buff, buff_n);

    seq++;
    //printf("Agent_control_state_message::send_message: message size= %i bytes\n", *buff_n - bcount);
}

int Agent_state_message::deserialize(unsigned char* buff, int buff_n) {
    //PACK_uint8_t(3, buff, buff_n);  //push message id on stack
    int _buff_n = buff_n;
    int msg_id = UPACK_uint8_t(buff, &buff_n); //msg id, not used
    id = UPACK_uint16_t(buff, &buff_n); //agent id
    seq = UPACK_uint8_t(buff, &buff_n);
    tick =UPACK_uint16_t(buff, &buff_n);
    cs = UPACK_uint32_t(buff, &buff_n);
    theta = UPACK_float(buff, &buff_n);
    phi = UPACK_float(buff, &buff_n);

    return buff_n - _buff_n;
}
