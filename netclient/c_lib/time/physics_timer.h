#pragma once

/*
int _start_physics_timer(int frequency); //ms per frame

int _tick_check();

long _get_time();
long _get_tick();
*/

void _START_CLOCK() GNOMESCROLL_API;

int _GET_TICK() GNOMESCROLL_API;

int _GET_MS_TIME() GNOMESCROLL_API;
long _GET_MICROSECOND_TIME();

int _LAST_TICK();

