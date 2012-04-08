#pragma once

#include <c_lib/common/enum_types.hpp>

const int EMPTY_SLOT = 65535;
class BaseInventoryProperties
{
    public:
        int item_id; // id would be the id of the main object it is referring to. this lets us get cache info
                // otherwise inconsequential
        Object_types item_type;
        int slot;

    BaseInventoryProperties()
    :
    item_id(EMPTY_SLOT), item_type(OBJ_TYPE_NONE),
    slot(-1)    // slot is set after allocation
    {
    }
};

template <class Inventory, class InventoryProperties>
class BaseInventoryContents // dont use behaviour list unless doing the registration model
{
    public:
        InventoryProperties* objects;
        int x,y;

        int max;
        int ct;

        Inventory* inventory;

    bool full()
    {
        if (this->ct >= this->max)
            return true;
        return false;
    }

    int get_slot(int x, int y)
    {
        return this->x*y + x;
    }

    int get_empty_slot()
    {
        if (this->full())
            return -1;
        for (int i=0; i<this->max; i++)
            if (this->objects[i].item_id == EMPTY_SLOT)
                return i;
        return -1;
    }

    bool is_valid_grid_position(int x, int y)
    {
        if (x < 0 || x >= this->x || y < 0 || y >= this->y)
            return false;
        return true;
    }

    void init(Inventory* inventory, int x, int y)
    {
        if (objects != NULL)
        {
            printf("WARNING: Inventory::init() -- objects is not NULL\n");
            return;
        }
        this->inventory = inventory;
        this->x = x;
        this->y = y;
        this->max = x*y;
        if (this->max <= 0)
        {
            printf("ERROR: Inventory::init() -- dimension %d is <=0: x,y = %d,%d\n", this->max, x,y);
            return;
        }
        this->objects = new InventoryProperties[this->max];
        for (int i=0; i<this->max; i++)
        {
            this->objects[i].slot = i;
            this->objects[i].inventory = inventory;
        }
    }

    bool can_add()
    {
        if (this->full())
            return false;
        return true;
    }
    
    bool can_add(int slot)
    {
        if (slot < 0 || slot >= this->max)
            return false;
        if (this->objects[slot].item_id != EMPTY_SLOT)
            return false;
        return true;
    }

    bool add(int id, Object_types type, int slot)
    {
        if (!this->can_add(slot))
            return false;
        if (this->objects[slot].item_id == EMPTY_SLOT && id != EMPTY_SLOT)
            this->ct++;
        this->objects[slot].load(id, type);
        printf("added %d,%d to %d\n", id, type, slot);
        return true;
    }

    bool remove(int x, int y)
    {
        int slot = this->get_slot(x,y);
        return this->remove(slot);
    }
    
    bool remove(int slot)
    {
        if (slot < 0 || slot >= this->max)
            return false;
        if (this->objects[slot].item_id != EMPTY_SLOT)
            this->ct--;
        this->objects[slot].load(EMPTY_SLOT, OBJ_TYPE_NONE);
        return true;
    }

    bool can_remove(int slot)
    {
        if (slot < 0 || slot >= this->max)
            return false;
        if (this->objects[slot].item_id == EMPTY_SLOT)
            return false;
        return true;
    }

    InventoryProperties* item_at_slot(int x, int y)
    {
        if (!this->is_valid_grid_position(x,y))
            return NULL;
        int slot = this->get_slot(x,y);
        return &this->objects[slot];
    }

    ~BaseInventoryContents<Inventory, InventoryProperties>()
    {
        if (this->objects != NULL)
            delete[] this->objects;
    }

    BaseInventoryContents<Inventory, InventoryProperties>()
    :
    objects(NULL),
    x(0), y(0), max(0), ct(0)
    {
    }
};
