#pragma once

#include <item/data/enum.hpp>
#include <item/data/constant.hpp>

#ifdef DC_SERVER
#include <item/net/StoC.hpp>
#endif

namespace item
{

void init_container(class ItemContainer* container, ItemContainerType type);

typedef int ItemId;

const int EMPTY_SLOT = 0xffff;

class ItemContainer
{
    public:

        int id;

        int xdim;
        int ydim;

        int slot_max;
        int slot_count;
        ItemContainerType container_type;

        ItemId* slot;

        bool is_full()
        {
            assert(this->slot_count <= this->slot_max);
            if (this->slot_count >= this->slot_max) return false;
            return true;
        }

        bool is_valid_grid_position(int x, int y)
        {
            return (x < 0 || x >= xdim || y < 0 || y >= ydim) ? false : true;
        }

        bool is_valid_grid_position(int _slot)
        {
            int x = _slot % xdim;
            int y = _slot / xdim;
            return (x < 0 || x >= xdim || y < 0 || y >= ydim) ? false : true;
        }

        int get_empty_slot()
        {
            for(int i=0; i<slot_max; i++) { if(slot[i] == EMPTY_SLOT) return i; }
            return -1;
        }

        void insert_item(int item_id, int x, int y)
        {
            assert(is_valid_grid_position(x,y));
            slot[y*xdim + x] = item_id;
            printf("inserted item %d at %d,%d in inventory %d \n", item_id, x, y, id);
        }

        //create item, return slot
        int create_item(int item_id)
        {
            int slot = this->get_empty_slot();
            int x = slot % this->xdim;
            int y = slot / this->xdim;
            assert(this->is_valid_grid_position(x,y));
            this->slot[y*this->xdim + x] = item_id;
            printf("inserted item %d at %d,%d in inventory %d \n", item_id, x, y, id);

            return slot;
        }

        void clear_slot(int x, int y)
        {
            assert(is_valid_grid_position(x,y));
            slot[y*xdim + x] = EMPTY_SLOT;
        }

        /* network shit */
        #if DC_SERVER
        void net_create(int client_id)
        {
            class create_item_container_StoC p;
            p.container_id = this->id;
            p.container_type = this->container_type;
            p.agent_id = client_id;
            p.sendToClient(client_id);
        }

        void net_delete(int client_id)
        {
            class delete_item_container_StoC p;
            p.container_id = this->id;
            p.container_type = this->container_type;
            p.agent_id = client_id;
            p.sendToClient(client_id);
        }

        //assign to an agent
        void net_assign(int client_id)
        {
            class assign_item_container_StoC p;
            p.container_id = this->id;
            p.container_type = this->container_type;
            p.agent_id = client_id;
            p.sendToClient(client_id);
        }
        #endif

        /* initializers */

        void init(ItemContainerType type, int _xdim, int _ydim)
        {
            container_type = type;

            xdim = _xdim;
            ydim = _ydim;

            slot_max = xdim*ydim;

            slot = new ItemId[slot_max];
            for(int i=0; i<slot_max; i++) slot[i] = EMPTY_SLOT;
            //printf("EMPTY_SLOT = %i \n", EMPTY_SLOT);
        }

        ~ItemContainer()
        {
           if (slot != NULL) delete[] slot;
        }

        ItemContainer(int id)
        : id(id), xdim(0), ydim(0),
        slot_max(0), slot_count(0), container_type(CONTAINER_TYPE_NONE),
        slot(NULL)
        {}
};

 
}

#include <common/template/object_list.hpp>

namespace item
{

const int ITEM_CONTAINER_MAX = 1024;

class ItemContainerList: public Object_list<ItemContainer, ITEM_CONTAINER_MAX>
{
    private:
        const char* name() { return "ItemContainer"; }
    public:
        ItemContainerList() { print_list((char*)this->name(), this); }
};

}
