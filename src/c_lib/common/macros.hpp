#pragma once

#define ASSERT_VALID_AGENT_ID(agent_id) GS_ASSERT(agent_id >= 0 && agent_id < AGENT_MAX)
#define IF_INVALID_AGENT_ID(agent_id) if (agent_id < 0 || agent_id >= AGENT_MAX) 

#define ASSERT_VALID_ITEM_ID(item_id) GS_ASSERT(item_id >= 0 && item_id < MAX_ITEMS)
#define IF_INVALID_ITEM_ID(item_id) if (item_id < 0 || item_id >= MAX_ITEMS)

#define ASSERT_VALID_CLIENT_ID(client_id) GS_ASSERT(client_id >= 0 && client_id != NULL_CLIENT && client_id < NetServer::HARD_MAX_CONNECTIONS)
#define IF_INVALID_CLIENT_ID(client_id) if (client_id < 0 || client_id == NULL_CLIENT || client_id >= NetServer::HARD_MAX_CONNECTIONS) 

#define ASSERT_VALID_BLOCK(block_type) GS_ASSERT(block_type >= 0 && block_type < t_map::MAX_CUBES)
#define IF_INVALID_BLOCK(block_type) if (block_type < 0 || block_type >= t_map::MAX_CUBES) 

#define ASSERT_VALID_MECH_TYPE(mech_type) GS_ASSERT(mech_type >= 0 && mech_type < t_mech::MAX_MECHS)
#define IF_INVALID_MECH_TYPE(mech_type) if (mech_type < 0 || mech_type >= t_mech::MAX_MECHS)

#define ASSERT_VALID_CONTAINER_TYPE(container_type) GS_ASSERT(container_type >= 0 && container_type < MAX_CONTAINER_TYPES)
#define IF_INVALID_CONTAINER_TYPE(container_type) if (container_type < 0 || container_type >= MAX_CONTAINER_TYPES) 

#define GS_ABORT() \
    do \
    { \
        printf("GS_ABORT error: %s, line %d function: %s\n", __FILE__, __LINE__, __FUNCTION__); \
        print_trace(); \
        exit(1); \
    } while(0);

#define ASSERT_NOT_NULL(p) \
    if ((p) == NULL) \
    { \
        fprintf(stderr, "ASSERT_NOT_NULL: %s error: %s, line %d \n", __func__, __FILE__, __LINE__); \
        return; \
    }

// macro stringification
#define STR2(x) #x
#define STR(x) STR2(x)


// __GNUG__
// The GNU C++ compiler defines this. Testing it is equivalent to testing (__GNUC__ && __cplusplus). 
// preprocessor macros 
// http://gcc.gnu.org/onlinedocs/cpp/Common-Predefined-Macros.html


#ifdef __GNUC__
    #define OPTIMIZED __attribute__((optimize("-O3")))
    #define STATIC_OPTIMIZED __attribute__((optimize("-O3"))) static

    #define INLINE __attribute__((__always_inline__)) inline 
    #define STATIC_INLINE __attribute__((__always_inline__)) static inline 

    #define INLINE_OPTIMIZED __attribute__((always_inline, optimize("-O3"))) inline
    #define STATIC_INLINE_OPTIMIZED __attribute__((always_inline, optimize("-O3"))) static inline 
#endif

#ifdef __MSVC__
    #define OPTIMIZED
    #define STATIC_OPTIMIZED static

    #define INLINE_OPTIMIZED __forceinline
    #define STATIC_INLINE_OPTIMIZED __forceinline

    #define INLINE __forceinline
    #define STATIC_INLINE __foreceinline static
#endif


/*
#ifndef __MSVC__
    #ifdef _MSC_VER
        #define __MSVC__
    #endif
#endif
*/

/*
#else
#define ALWAYS_INLINE inline
#endif
#endif
*/


/*
#ifndef UNLIKELY
#if COMPILER(GCC)
#define UNLIKELY(x) __builtin_expect((x), 0)
#else
#define UNLIKELY(x) (x)
#endif
#endif

#ifndef LIKELY
#if COMPILER(GCC)
#define LIKELY(x) __builtin_expect((x), 1)
#else
#define LIKELY(x) (x)
#endif
#endif
#endif
*/
