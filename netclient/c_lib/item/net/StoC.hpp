#pragma once

#include <net_lib/net.hpp>

//#include <net_lib/t_item/net/StoCs.hpp>
//#include <t_item/free_item.hpp>


namespace item
{



/*
    Container creation and subscription
*/
class create_item_container_StoC: public FixedSizeReliableNetPacketToClient<create_item_container_StoC>
{
    public:
        uint8_t agent_id;
        uint16_t container_id;
        uint8_t container_type;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&agent_id, buff, buff_n, pack);
            pack_u16(&container_id, buff, buff_n, pack);
            pack_u8(&container_type, buff, buff_n, pack);

        }
        inline void handle();
};

class delete_item_container_StoC: public FixedSizeReliableNetPacketToClient<delete_item_container_StoC>
{
    public:
        uint8_t agent_id;
        uint16_t container_id;
        uint8_t container_type;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&agent_id, buff, buff_n, pack);
            pack_u16(&container_id, buff, buff_n, pack);
            pack_u8(&container_type, buff, buff_n, pack);

        }
        inline void handle();
};

class assign_item_container_StoC: public FixedSizeReliableNetPacketToClient<assign_item_container_StoC>
{
    public:
        uint8_t agent_id;
        uint16_t container_id;
        uint8_t container_type;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&agent_id, buff, buff_n, pack);
            pack_u16(&container_id, buff, buff_n, pack);
            pack_u8(&container_type, buff, buff_n, pack);

        }
        inline void handle();
};

/*
    Inventory State Packets
*/


/*
    Inventory Item State Packets
*/

class item_create_StoC: public FixedSizeReliableNetPacketToClient<item_create_StoC>
{
    public:
        //uint8_t type;
        uint16_t item_id;
        uint16_t item_type;
        uint16_t inventory_id;
        uint16_t inventory_slot;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u16(&item_id, buff, buff_n, pack);
            pack_u16(&item_type, buff, buff_n, pack);
            pack_u16(&inventory_id, buff, buff_n, pack);
            pack_u16(&inventory_slot, buff, buff_n, pack);
        }
        inline void handle();
};


/* actions */

class move_item_to_hand_StoC: public FixedSizeReliableNetPacketToClient<move_item_to_hand_StoC>
{
    public:
        uint16_t id;
        uint8_t slota;
        uint8_t slotb;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u16(&id, buff, buff_n, pack);
            pack_u8(&slota, buff, buff_n, pack);
            pack_u8(&slotb, buff, buff_n, pack);
        }
        inline void handle();
};

/* actions */
class drop_item_from_hand_StoC: public FixedSizeReliableNetPacketToClient<drop_item_from_hand_StoC>
{
    public:
        uint16_t ida;
        uint8_t slota;
        uint16_t idb;
        uint8_t slotb;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u16(&ida, buff, buff_n, pack);
            pack_u8(&slota, buff, buff_n, pack);
            pack_u16(&idb, buff, buff_n, pack);
            pack_u8(&slotb, buff, buff_n, pack);
        }
        inline void handle();
};

}
