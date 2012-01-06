#pragma once

//put stuff that gets exposed to cython at top here
//extern "C" {
    #include "./time/physics_timer.c" //physics timer
//


//#include <net_lib/client/pviz.c>

#include <net_lib/export.cpp>

#include <net_lib/common/net_peer.cpp>
#include <net_lib/common/sequencer.c>
#include <net_lib/message_handler.c>
#include <net_lib/packet_buffer.cpp>
#include <net_lib/common/python_channel.cpp>

#ifdef DC_CLIENT
#include <net_lib/client/client.cpp>
#include <net_lib/client.c>
#endif

#ifdef DC_SERVER
#include <net_lib/server/server.c>
#include <net_lib/server.c>
#endif
