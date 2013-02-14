#include "drop_dat.hpp"

#if DC_CLIENT
dont_include_this_file_in_client
#endif

#include <item/properties.hpp>
#include <item/particle/_interface.hpp>

namespace t_mech
{

static void add_drop_callback(int id)
{
    IF_ASSERT(!isValid((MechType)id)) return;
    IF_ASSERT(mech_attributes == NULL) return;
    mech_attributes[id].item_drop = true;
}

class Item::ItemDropConfig* drop_dat = NULL;

static int get_mech_type_ptr(const char* name)
{   // hides the MechType from the drop dat
    return (int)get_mech_type(name);
}

static const char* get_mech_name_ptr(int mech_type)
{   // hides the MechType from the drop dat
    return get_mech_name((MechType)mech_type);
}

void init_drop_dat()
{
    GS_ASSERT(drop_dat == NULL);
    drop_dat = new Item::ItemDropConfig;
    drop_dat->init("mech", MAX_MECHS);  // TODO -- accept NULL_ID parameter
    drop_dat->get_id_from_name = &get_mech_type_ptr;
    drop_dat->get_name_from_id = &get_mech_name_ptr;
    drop_dat->add_drop_callback = &add_drop_callback;
}

void teardown_drop_dat()
{
    delete drop_dat;
}

void load_drop_dat()
{
    IF_ASSERT(drop_dat == NULL) return;
    class Item::ItemDropConfig* b = drop_dat;

    ////////////////////
    // look at t_map/config/drop_dat.cpp for more configuration examples

    b->def_drop("blue_crystal");
        b->add_drop("blue_crystal", 3);
        b->set_drop(0.70f, 1);
        b->set_drop(0.15f, 2);
        b->set_drop(0.075f, 3);

    b->def_drop("red_crystal");
        b->add_drop("red_crystal", 3);
        b->set_drop(0.70f, 1);
        b->set_drop(0.15f, 2);
        b->set_drop(0.075f, 3);

    b->def_drop("green_crystal");
        b->add_drop("green_crystal", 3);
        b->set_drop(0.70f, 1);
        b->set_drop(0.15f, 2);
        b->set_drop(0.075f, 3);

    //acadia flower stages
    b->def_drop("acadia_flower_stage_0");
        b->add_drop("acadia_seed", 1);
        b->set_drop(0.99f, 1);

    b->def_drop("acadia_flower_stage_1");
        b->add_drop("acadia_seed", 2);
        b->set_drop(0.80f, 1);
        b->set_drop(0.19f, 2);

        b->add_drop("acadia_fruit", 1);
        b->set_drop(0.50f, 1);

    b->def_drop("acadia_flower_stage_2");
        b->add_drop("acadia_seed", 3);
        b->set_drop(0.50f, 1);
        b->set_drop(0.25f, 2);
        b->set_drop(0.24f, 3);

        b->add_drop("acadia_fruit", 3);
        b->set_drop(0.50f, 1);
        b->set_drop(0.25f, 2);
        b->set_drop(0.24f, 3);


    b->def_drop("cryptid_larvae_stage_0");
        b->add_drop("cryptid_larvae", 1);
        b->set_drop(0.99f, 1);

    b->def_drop("cryptid_larvae_stage_1");
        b->add_drop("cryptid_larvae", 2);
        b->set_drop(0.50f, 1);
        b->set_drop(0.24f, 2);

    b->def_drop("cryptid_larvae_stage_2");
        b->add_drop("cryptid_larvae", 3);
        b->set_drop(0.25f, 1);
        b->set_drop(0.25f, 2);
        b->set_drop(0.24f, 3);

    b->def_drop("cryptid_larvae_stage_3");
        b->add_drop("cryptid_larvae", 3);
        b->set_drop(0.33f, 1);
        b->set_drop(0.33f, 2);
        b->set_drop(0.33f, 3);

    b->end();

    b->save_to_file();
}

void handle_drop(int x, int y, int z, int type)
{
    IF_ASSERT(drop_dat == NULL) return;

    for (int i=0; i < drop_dat->meta_drop_table[type].num_drop; i++)
    {
        struct Item::ItemDropTable* cidt = &drop_dat->item_drop_table[i+drop_dat->meta_drop_table[type].index];
        float p = randf();

        if (p <= cidt->drop_cumulative_probabilities[0]) continue;

        for (int j=1; j < cidt->drop_entries; j++)
        {
            //check each drop until commulative probability table hit
            if (p <= cidt->drop_cumulative_probabilities[j])
            {
                for (int k=0; k<cidt->item_drop_num[j]; k++)
                {
                    const float mom = 2.0f;
                    x = (float)((float)x + 0.5f + randf()*0.33f);
                    y = (float)((float)y + 0.5f + randf()*0.33f);
                    z = (float)((float)z + 0.05f);
                    ItemParticle::create_item_particle(cidt->item_type, x, y, z,
                        (randf()-0.5f)*mom, (randf()-0.5f)*mom, mom);
                }
                break;
            }
        }
    }
}

}   // t_mech
