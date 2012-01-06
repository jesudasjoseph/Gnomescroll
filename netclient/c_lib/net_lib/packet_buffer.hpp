#pragma once

#include <c_lib/template/object_pool.hpp>

/*
NetMessage Array
*/

static const int NET_MESSAGE_BUFFER_SIZE = 4096; // 69,000 per second, 74-78k per second
static const int NET_MESSAGE_ARRAY_SIZE = 256; //use 256

#define PACKET_BUFFER_DEBUG 1

void NetPeer::flush_reliable_to_buffer(char* buff_, int* _index)
{
    NetMessageArray* nma = ps->nma;
    int nma_index = ps->read_index;
    int num = ps->messages_n;

    class Net_message* nm;

    //for(int i=0; i < num; i++)
    /*        
        Iterate over num packets
        Dont let retire check 1 on last loop
    */
    for(int i=0; i < num; i++)
    {
        nm = nma->net_message_array[nma_index];
        nma->net_message_array[nma_index] = NULL; //!!! DEBUG

        nm->decrement_reliable();
        nma->reference_count--;    //decrement on confirmation
        nma_index++;
        
        if(nma_index == NET_MESSAGE_ARRAY_SIZE)
        {
            NetMessageArray* tmp = nma->next;
            if(nma->reference_count == 0)
            {
                if(i+1 == num) break;   //prevent from running on last loop, to avoid double retiring
                //printf("1 delete nma %i \n", (int)nma);
                nma->retire();
            }
            nma = tmp;
            nma_index=0;
        }
    }
    if(nma->reference_count == 0)
    {
        //printf("2 delete nma %i \n", (int) nma);
        nma->retire();
    }


    pending_bytes = 0;
    pending_messages = 0;

    *_index = index;    //bytes out
}


};

//used by net_peer
class NetMessageArray {
    private:
    public:
    int reference_count;
    class NetMessageArray* next;
    class Net_message* net_message_array[NET_MESSAGE_ARRAY_SIZE];

    OBJECT_POOL_OBJECT_MACRO

    NetMessageArray()
    {
        #if PACKET_BUFFER_DEBUG
            for(int i=0; i < NET_MESSAGE_ARRAY_SIZE; i++) net_message_array[i] = NULL;
        #endif
        reference_count = 0;
        next = NULL;
    }

    inline void retire() __attribute__((always_inline));

    inline static NetMessageArray* acquire() __attribute__((always_inline));
};


class Net_message {
    private:
    public:
        class Net_message_buffer* b;
        char* buff;
        short len;
        short reference_count;
        Net_message* next;

        OBJECT_POOL_OBJECT_MACRO

    static inline class Net_message* acquire();

    void inline decrement_reliable();
    void inline decrement_unreliable();

    static class Net_message* acquire_reliable(int length);
    static class Net_message* acquire_unreliable(int length);

    Net_message() {
        reference_count = 0; 
        //increment reference on pushing onto packet buffer
        //decrement on dispatch or completion
    }
};


class NetMessageManager
{
    public:

    int pending_messages;
    int pending_bytes_out;

    class NetMessageArray* nma_insert; //array for inserting
    int nma_insert_index;              //index for insertions

    class NetMessageArray* nma_read;   //array for reading
    int nma_read_index;                //index for reads

    NetMessageManager()
    {
        nma_insert = NetMessageArray::acquire();

        nma_insert_index = 0;
        nma_read = rnma_insert;
        nma_read_index = 0;

        pending_messages = 0;
        pending_bytes = 0;
    }
        

    void NetMessageManager::push_message(Net_message* nm) 
    {
        pending_bytes_out += nm->len;
        pending_messages++;

        nm->reference_count++;  //increment reference count?

        //insert message
        nma_insert->net_message_array[nma_insert_index] = nm;

        //insert 
        nma_insert_index++;
        if(nma_insert_index == NET_MESSAGE_ARRAY_SIZE)
        {
            printf("NetMessageManager::push_message, rare condition \n");
            //DEBUG?
            nma_insert->next = NetMessageArray::acquire();
            nma_insert->next->next = NULL; //head of list
            nma_insert = nma_insert->next;
            nma_insert_index = 0;
        }
    }

    void NetMessageManager::serialize_messages(char* buff_, int* _index)
    {
        if(pending_messages == 0) return;
        /*
            Create packet and serialize to it
        */

        class Net_message* nm;

        for(int i=0; i < rnma_pending_messages; i++)
        {
            nm = rnma_read->net_message_array[rnma_read_index];

            memcpy(buff_+*_index, nm->buff, nm->len);
            *_index += nm->len;

            nm->decrement_reliable(); //reference count on packet

            rnma_read_index++;
            if(rnma_read_index == NET_MESSAGE_ARRAY_SIZE)
            {
                printf("NetMessageManager::serialize_messages, rare condition \n");
                NetMessageArray* tmp = nma_read;
                nma_read = nma_read->next;
                tmp->retire();
                nma_read_index=0;
            }
        }      

        //reset to virgin state
        nma_insert_index = 0;
        nma_read = rnma_insert;
        nma_read_index = 0;

        pending_messages = 0;
        pending_bytes = 0;
};