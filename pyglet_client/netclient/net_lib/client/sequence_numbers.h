#ifndef net_lib_client_sequence_number
#define net_lib_client_sequence_number

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


//outgoing
struct packet_sequence {
    int seq;
    int ack;
    //int active;
    //int tick; //better than sequence for determining dropped packets
};


struct Pseq {
    struct packet_sequence packet_sequence_buffer[64];
    //int packet_sequence_buffer_n = 0;
    int packet_sequence_number;
};

void init_sequence_numbers(struct Pseq* sq);
uint16_t get_next_sequence_number(struct Pseq* sq);
void check_for_dropped_packets(struct Pseq* sq);
void process_acks(struct Pseq* sq, unsigned short seq, unsigned int flag);

//incoming/response to outgoing
struct packet_sequence2 {
    int seq;
    int received;
};

struct Pseq2 {
    struct packet_sequence2 seqbuff[64];
    int highest_packet_sequence_number;
};

void init_sequence_numbers_out(struct Pseq2* pq2);
uint16_t get_sequence_number(struct Pseq2* pq2);
void set_ack_for_received_packet(struct Pseq2* pq2, int seq);
uint32_t generate_outgoing_ack_flag(struct Pseq2* pq2);
#endif
