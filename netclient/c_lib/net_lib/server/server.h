#ifndef net_lib_server_h
#define net_lib_server_h

#include <errno.h>

#include <sys/types.h> //?
#include <sys/socket.h> //?

#include "../common/net_lib_common.h"
#include "../common/sequencer.h"
#include "../common/net_peer.h"

#include "../common/message_handler.h"

/*
struct Socket {
    uint32_t ip;
    uint16_t port;
    int socket;
    struct sockaddr_in address;
};

#define TTL_MAX 150
//5 seconds timeout?

struct NetPeer {
    unsigned short id;
    uint32_t ip;
    uint16_t port;
    uint16_t client_id;
    struct sockaddr_in address;

    unsigned int ttl;

    struct Pseq2 sq2;
};
*/

#define HARD_MAX_CONNECTIONS 1024
struct ConnectionPool {
    int n_connections;
    struct NetPeer* connection[HARD_MAX_CONNECTIONS];
};

void broad_cast_packet(); //every 30 ms
void broad_cast_packet2();

void receive_packets(struct Socket* socket);

void init_server(unsigned short port);
inline int error_check_packet(unsigned char* data, int n);
void process_packets();

void decrement_ttl();
void poll_connection_timeout();

void check_pool_for_dropped_packets();
#endif
