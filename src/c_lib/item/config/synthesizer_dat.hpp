#pragma once

#include <item/common/struct.hpp>

namespace Item
{

//void synthesizer_item_def(const char* item_name, int cost);
//void synthesizer_item_set(int level, int slotx, int sloty);

void load_synthesizer()
{
    synthesizer_item_def("mining_laser", 1);
    synthesizer_item_set(0,0);

    synthesizer_item_def("laser_rifle", 5);
    synthesizer_item_set(1,0);

    synthesizer_item_def("small_crafting_bench", 1);
    synthesizer_item_set(0,1);

    synthesizer_item_def("smelter_1", 5);
    synthesizer_item_set(1,1);

    synthesizer_item_def("cryofreezer_1", 3);
    synthesizer_item_set(1,2);

    synthesizer_item_def("repair_kit", 3);
    synthesizer_item_set(0,2);

    synthesizer_item_def("block_placer", 20);
    synthesizer_item_set(0,3);
}


}