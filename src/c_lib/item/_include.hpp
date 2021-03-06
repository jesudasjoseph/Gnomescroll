/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#include <item/common/constants.hpp>
#include <item/common/enum.hpp>

#include <item/properties.cpp>
#include <item/item.cpp>

#include <item/net/CtoS.cpp>
#include <item/net/StoC.cpp>

#include <item/_state.cpp>
#include <item/_interface.cpp>

#include <item/config/_interface.hpp>
#include <item/config/item_dat.cpp>
#include <item/config/synthesizer_dat.hpp>
#include <item/config/crafting_dat.hpp>
#include <item/config/smelting_dat.hpp>

#if DC_SERVER
# include <item/config/item_drop.cpp>
# include <item/config/item_drop_alt.cpp>
# include <item/server.cpp>
#endif
