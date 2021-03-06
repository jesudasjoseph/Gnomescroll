/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "_interface.hpp"

#include <physics/vec3i.hpp>
#include <t_map/common/constants.hpp>
#include <item/common/constants.hpp>
#include <item/common/enum.hpp>
#if DC_CLIENT
# include <t_map/t_vbo_class.hpp>
# include <t_map/t_vbo_update.hpp>
# include <t_map/t_vbo_draw.hpp>
#endif

namespace t_map
{

void init_packets()
{
    t_map::map_chunk_compressed_StoC::register_client_packet();
    t_map::map_chunk_uncompressed_StoC::register_client_packet();

    t_map::set_map_alias_StoC::register_client_packet();
    t_map::clear_alias_StoC::register_client_packet();

    // block value change
    t_map::block_set_StoC::register_client_packet();
    t_map::block_set_palette_StoC::register_client_packet();

    t_map::block_action_StoC::register_client_packet();
    t_map::map_metadata_StoC::register_client_packet();

    // block damage
    t_map::request_block_damage_CtoS::register_server_packet();
    t_map::block_damage_StoC::register_client_packet();

    t_map::container_block_chunk_reset_StoC::register_client_packet();
    t_map::container_block_create_StoC::register_client_packet();
    t_map::container_block_delete_StoC::register_client_packet();

    t_map::control_node_create_StoC::register_client_packet();
    t_map::control_node_delete_StoC::register_client_packet();
}


ItemContainerID get_block_item_container(Vec3i position)
{
    if (!is_valid_z(position)) return NULL_CONTAINER;
    position = translate_position(position);
    class MapChunk* c = main_map->get_chunk(position);
    if (c == NULL)
        return NULL_CONTAINER;
    else
        return c->chunk_item_container.get(position);
}

bool get_container_location(ItemContainerID container_id, Vec3i& position)
{
    IF_ASSERT(container_id == NULL_CONTAINER) return false;

    ItemContainer::ItemContainerInterface* container = ItemContainer::get_container(container_id);
    IF_ASSERT(container == NULL) return false;
    IF_ASSERT(container->chunk < 0) return false;
    IF_ASSERT(container->chunk >= main_map->xchunk_dim*main_map->ychunk_dim) return false;

    class MapChunk* c = main_map->chunk[container->chunk];
    IF_ASSERT(c == NULL) return false;

    c->chunk_item_container.get_container_location(container_id, position);
    return true;
}

#if DC_CLIENT
class VBOMap* vbo_map;

void init_t_vbo()
{
    GS_ASSERT(vbo_map == NULL);
    vbo_map = new VBOMap(main_map);
    t_vbo_update_init();
    vbo_draw_init();
}

void teardown_t_vbo()
{
    delete vbo_map;
    t_vbo_update_end();
    vbo_draw_end();
}

void draw_map()
{
    vbo_map->draw_map();
}

void draw_map_compatibility()
{
    vbo_map->draw_map_compatibility();
}

void update_map()
{
    vbo_map->update_map();
}

static const unsigned int REQUEST_DMG_ID_MAX = 0xFF;
static unsigned int request_id = 0;

static struct Vec3i last_requested_block = vec3i_init(-1);
static unsigned int last_request_id = 0;
static int requested_block_health = 0;

CubeType requested_cube_type = NULL_CUBE;
unsigned int requested_block_damage = 0;

bool is_last_requested_block(const Vec3i& position)
{
    return is_equal(last_requested_block, position);
}

void request_block_damage(const Vec3i& position)
{
    GS_ASSERT(request_id < REQUEST_DMG_ID_MAX);

    // ignore if we already requested this block
    if (is_last_requested_block(position)) return;

    // set last request id & block
    last_request_id = request_id;
    last_requested_block = position;

    requested_cube_type = get(position);
    requested_block_health = maxDamage(requested_cube_type);

    // send packet
    request_block_damage_CtoS msg;
    msg.position = position;
    msg.request_id = request_id;
    msg.send();

    // increment index
    request_id++;
    request_id %= REQUEST_DMG_ID_MAX;

    // reset dmg request
    requested_block_damage = 0;
}

int get_requested_block_remaining_health()
{
    int health = requested_block_health - requested_block_damage;
    IF_ASSERT(health < 0) health = 0;
    return health;
}

Vec3i get_requested_block_position()
{
    return last_requested_block;
}

void received_block_damage_response(unsigned int request_id, unsigned int dmg)
{
    IF_ASSERT(request_id >= REQUEST_DMG_ID_MAX) return;
    // if request_id matches last request id,
    if (request_id == last_request_id)
        requested_block_damage = dmg;   // set current dmg
}
#endif

#if DC_SERVER
bool create_item_container_block(const Vec3i& position, ItemContainerType container_type, ItemContainerID container_id)
{
    IF_ASSERT(!is_valid_z(position)) return false;
    Vec3i p = translate_position(position);

    IF_ASSERT(!isItemContainer(t_map::get(p))) return false;

    class MapChunk* c = main_map->get_chunk(p);
    IF_ASSERT(c == NULL) return false;
    return c->chunk_item_container.add(p, container_type, container_id);
}

void smelter_on(ItemContainerID container_id)
{
    Vec3i pos;
    bool success = get_container_location(container_id, pos);
    if (!success) return;
    int palette = get_palette(pos);
    if (palette >= 4) return;
    palette += 4;
    broadcast_set_palette(pos, palette);
}

void smelter_off(ItemContainerID container_id)
{
    Vec3i pos;
    bool success = get_container_location(container_id, pos);
    if (!success) return;
    int palette = get_palette(pos);
    if (palette < 4) return;
    palette -= 4;
    broadcast_set_palette(pos, palette);
}

void handle_explosive_block(const Vec3i& position)
{   // make sure to destroy the block before calling this
    IF_ASSERT(!is_valid_z(position)) return;
    t_gen::create_explosion(translate_position(position));
}

// Do on client connect
void send_client_map_special(ClientID client_id)
{
    main_map->control_node_list.send_control_nodes_to_client(client_id);
}

void add_control_node(const Vec3i& position)
{
    //printf("Server adding control node at: %d %d %d \n", x,y,z);
    main_map->control_node_list.server_add_control_node(position);
}

#endif

}   // t_map
