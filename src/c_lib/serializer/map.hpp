#pragma once

#include <common/files.hpp>

namespace serializer
{

struct SerializedChunk
{
    uint32_t xchunk;
    uint32_t ychunk;
    struct MAP_ELEMENT data[sizeof(struct MAP_ELEMENT)*16*16*128];
};

const char default_map_file[] = "./world/map/map-" STR(GS_VERSION) ".map";

extern bool should_save_map;
extern bool map_save_completed;
extern char* map_tmp_name;
extern char* map_final_name;
extern bool map_save_memcpy_in_progress;

extern class BlockSerializer* block_serializer;

#if PTHREADS_ENABLED
void wait_for_threads();
#endif

void save_map(const char* filename);
void load_map(const char* filename);

// uses default map names
void save_map();
void load_map();

void check_map_save_state();

void init_map_serializer();
void teardown_map_serializer();

class BlockSerializer
{
    public:

        static const int prefix_length = sizeof(uint32_t);
        static const int version = GS_VERSION;
        static const int chunk_number = 32*32;

        char file_name[256];
        size_t file_size;

        char* write_buffer;
        struct SerializedChunk* chunk;

        #if PTHREADS_ENABLED
        int* version_array; // [chunk_number];
        #else
        struct SerializedChunk* chunks; //[chunk_number];
        #endif
    
    BlockSerializer();
    ~BlockSerializer();

    static void push_int(char* buffer, int &index, uint32_t value)
    {
        *((uint32_t*)(buffer+index)) = value;
        index += sizeof(uint32_t);
    }

    static void pop_int(char* buffer, int &index, int &value)
    {
        value = *((uint32_t*)(buffer+index));
        index += sizeof(uint32_t);
    }

    void load(const char* filename);

    void save(const char* filename);

    #if PTHREADS_ENABLED
    //this is called until map is done saving
    //will memcpy map and yield after ms milliseconds
    void save_iter(int max_ms);
    #endif
};

}   // serializer