#ifndef NAVIGATION_H
#define NAVIGATION_H

#include "terrain.h"

#define WORLD_SIZE 400

typedef struct world
{
    int x;
    int y;
    map_t **maps;
} world_t;

int init_world(world_t *n);
int delete_world(world_t *n);
int print_map(world_t *n);
int travel_north(world_t *n);
int travel_south(world_t *n);
int travel_east(world_t *n);
int travel_west(world_t *n);
int travel_fly(world_t *n, int x, int y);

#endif