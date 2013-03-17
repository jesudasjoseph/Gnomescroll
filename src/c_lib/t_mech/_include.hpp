#pragma once

#include <t_mech/common/common.hpp>
#include <t_mech/properties.cpp>
#include <t_mech/net/CtoS.cpp>
#include <t_mech/net/StoC.cpp>

#include <t_mech/mech_raycast.hpp>
#include <t_mech/mech_state.cpp>
#include <t_mech/config/_interface.hpp>
#include <t_mech/config/mech_dat.hpp>

#ifdef DC_CLIENT
# include <t_mech/mech_draw.hpp>
#endif

#if DC_SERVER
# include <t_mech/config/drop_dat.cpp>
#endif

#include <t_mech/_interface.cpp>
