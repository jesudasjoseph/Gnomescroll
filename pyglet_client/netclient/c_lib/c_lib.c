#include "c_lib.h"

#include <stdio.h>

#include "./t_map/t_map.c"
#include "./t_map/t_properties.c"

#include "./ray_trace/ray_trace.c"

#include "./objects/grenade.c"

//#include "./texture_loader.c"

int init_c_lib() {
    printf("init c_lib\n");
    init_texture_loader();
    return 0;
}
