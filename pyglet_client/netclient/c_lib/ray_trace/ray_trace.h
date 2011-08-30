#ifndef ray_trace_h
#define ray_trace_h

#include <stdio.h>
#include <math.h>

#include <compat.h>

#include <t_map/t_map.h>
#include <t_map/t_properties.h>

#define ssize 256
#define bsize 65536

int ray_cast(float x0,float y0,float z0, float x1,float y1,float z1);
int ray_cast_lax(float x0,float y0,float z0, float x1,float y1,float z1);

int* _ray_cast3(float x0,float y0,float z0, float x1,float y1,float z1, float* distance);

int* _ray_cast4(float x0,float y0,float z0, float x1,float y1,float z1, float* interval);

//in use
int* _ray_cast5(float x0,float y0,float z0, float x1,float y1,float z1, float* interval, int* collision, int* tile);
int _ray_cast6(float x0,float y0,float z0, float _dfx,float _dfy,float _dfz, float max_l, float *distance, int* collision, int* pre_collision, int* tile, int* side);

#endif
