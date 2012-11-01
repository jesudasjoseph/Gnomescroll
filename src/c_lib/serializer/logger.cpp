#include "logger.hpp"

#include <serializer/constants.hpp>

#if GS_SERIALIZER
#include <serializer/containers.hpp>
#include <serializer/items.hpp>
#include <serializer/players.hpp>
#endif

namespace serializer
{

char log_folder[NAME_MAX+1];

char player_log_path[NAME_MAX+1];
char container_log_path[NAME_MAX+1];
char map_log_path[NAME_MAX+1];
char mech_log_path[NAME_MAX+1];

FILE* player_log = NULL;
FILE* container_log = NULL;
FILE* map_log = NULL;
FILE* mech_log = NULL;

void init_logger()
{    
    GS_ASSERT(player_log == NULL);
    GS_ASSERT(container_log == NULL);
    GS_ASSERT(map_log == NULL);
    GS_ASSERT(mech_log == NULL);    
}

void teardown_logger()
{
    if (map_log != NULL) fclose(map_log);
    if (mech_log != NULL) fclose(mech_log);
    if (player_log != NULL) fclose(player_log);
    if (container_log != NULL) fclose(container_log);
}

void set_log_paths(const char* save_folder)
{
    int wrote = 0;

    wrote = snprintf(log_folder, NAME_MAX+1, "%s%s%s", WORLD_DATA_PATH, save_folder, LOG_FOLDER);
    GS_ASSERT_ABORT(wrote <= NAME_MAX+1);
    log_folder[NAME_MAX] = '\0';
    create_path(log_folder);

    wrote = snprintf(player_log_path, NAME_MAX+1, "%s%s%s%s", WORLD_DATA_PATH, save_folder, LOG_FOLDER, PLAYER_LOG_FILENAME);
    GS_ASSERT_ABORT(wrote <= NAME_MAX+1);
    player_log_path[NAME_MAX] = '\0';
    wrote = snprintf(container_log_path, NAME_MAX+1, "%s%s%s%s", WORLD_DATA_PATH, save_folder, LOG_FOLDER, CONTAINER_LOG_FILENAME);
    GS_ASSERT_ABORT(wrote <= NAME_MAX+1);
    container_log_path[NAME_MAX] = '\0';
    wrote = snprintf(map_log_path, NAME_MAX+1, "%s%s%s%s", WORLD_DATA_PATH, save_folder, LOG_FOLDER, MAP_LOG_FILENAME);
    GS_ASSERT_ABORT(wrote <= NAME_MAX+1);
    map_log_path[NAME_MAX] = '\0';
    wrote = snprintf(mech_log_path, NAME_MAX+1, "%s%s%s%s", WORLD_DATA_PATH, save_folder, LOG_FOLDER, MECH_LOG_FILENAME);
    GS_ASSERT_ABORT(wrote <= NAME_MAX+1);
    mech_log_path[NAME_MAX] = '\0';

    player_log = fopen(player_log_path, "a");
    container_log = fopen(container_log_path, "a");
    map_log = fopen(map_log_path, "a");
    mech_log = fopen(mech_log_path, "a");
}

void log_map_load_error(const char* msg)
{
    GS_ASSERT(map_log != NULL);
    if (map_log == NULL) return;
    fprintf(map_log, "LoadError: %s\n", msg);
    fprintf(container_log, LOG_LINE_SEPARATOR);
}

void log_map_save_error(const char* msg)
{
    GS_ASSERT(map_log != NULL);
    if (map_log == NULL) return;
    fprintf(map_log, "SaveError: %s\n", msg);
    fprintf(container_log, LOG_LINE_SEPARATOR);
}

void log_mech_load_error(const char* msg)
{
    GS_ASSERT(mech_log != NULL);
    if (mech_log == NULL) return;
    fprintf(mech_log, "LoadError: %s\n", msg);
    fprintf(mech_log, LOG_LINE_SEPARATOR);
}

void log_mech_save_error(const char* msg)
{
    GS_ASSERT(mech_log != NULL);
    if (mech_log == NULL) return;
    fprintf(mech_log, "SaveError: %s\n", msg);
    fprintf(mech_log, LOG_LINE_SEPARATOR);
}

#if GS_SERIALIZER
void log_container_load_error(const char* msg,
    class ParsedContainerFileData* file_data, class ParsedContainerData* container_data,
    class ParsedItemData* item_data)
{
    GS_ASSERT(container_log != NULL);
    if (container_log == NULL) return;
    fprintf(container_log, "LoadError: %s\n", msg);

    if (file_data != NULL)
        fprintf(container_log, "ParsedContainerFileData: version %d, container_container %d, valid %d\n",
            file_data->version, file_data->container_count, file_data->valid);

    if (container_data != NULL)
        fprintf(container_log, "ParsedContainerData: container_id %d, name %s, item_count %d, pos %d,%d,%d, valid %d\n",
            container_data->container_id, container_data->name, container_data->item_count,
            container_data->position.x, container_data->position.y, container_data->position.z,
            container_data->valid);

    if (item_data != NULL)
    {
        char uuid[UUID_STRING_LENGTH+1];
        write_uuid(uuid, UUID_STRING_LENGTH, item_data->uuid);
        uuid[UUID_STRING_LENGTH] = '\0'; 
        fprintf(player_log, "ParsedItemData: ID %d, uuid %s, name %s, durability %u, stack_size %u, location_name %s, location_id %u, container_slot %u, item_type %d, item_location %d, item_container_type %d\n",
            item_data->id, uuid, item_data->name, item_data->durability, item_data->stack_size,
            item_data->location_name, item_data->location_id, item_data->container_slot,
            item_data->item_type, item_data->item_location, item_data->item_container_type);
    }
    
    fprintf(container_log, LOG_LINE_SEPARATOR);
}

void log_container_save_error(const char* msg)
{
    GS_ASSERT(container_log != NULL);
    if (container_log == NULL) return;
    fprintf(container_log, "SaveError: %s\n", msg);
    fprintf(container_log, LOG_LINE_SEPARATOR);
}

void log_player_save_error(const char* msg)
{
    GS_ASSERT(player_log != NULL);
    if (player_log == NULL) return;
    fprintf(player_log, "SaveError: %s\n", msg);
    fprintf(player_log, LOG_LINE_SEPARATOR);
}

void log_player_load_error(const char* msg,
    class PlayerLoadData* load_data, class PlayerContainerLoadData* container_load_data,
    class ParsedPlayerData* player_data, class ParsedPlayerContainerData* container_data,
    class ParsedItemData* item_data)
{
    GS_ASSERT(player_log != NULL);
    if (player_log == NULL) return;
    fprintf(player_log, "LoadError: %s\n", msg);

    if (load_data != NULL)
        fprintf(player_log, "PlayerLoadData: user_id %d, client_id %d\n", load_data->user_id, load_data->client_id);

    if (container_load_data != NULL)
        fprintf(player_log, "PlayerLoadData: container_type %d\n", container_load_data->container_type);

    if (player_data != NULL)
        fprintf(player_log, "ParsedPlayerData: color %d,%d,%d\n",
            player_data->color.r, player_data->color.g, player_data->color.b);

    if (container_data != NULL)
        fprintf(player_log, "ParsedPlayerContainerData: name %s, user_id %d, container_count %d\n", container_data->name, container_data->user_id, container_data->container_count);

    if (item_data != NULL)
    {
        char uuid[UUID_STRING_LENGTH+1];
        write_uuid(uuid, UUID_STRING_LENGTH, item_data->uuid);
        uuid[UUID_STRING_LENGTH] = '\0'; 
        fprintf(player_log, "ParsedItemData: ID %d, uuid %s, name %s, durability %u, stack_size %u, location_name %s, location_id %u, container_slot %u, item_type %d, item_location %d, item_container_type %d\n",
            item_data->id, uuid, item_data->name, item_data->durability, item_data->stack_size,
            item_data->location_name, item_data->location_id, item_data->container_slot,
            item_data->item_type, item_data->item_location, item_data->item_container_type);
    }
    
    fprintf(player_log, LOG_LINE_SEPARATOR);
}
#endif

}   // serializer
