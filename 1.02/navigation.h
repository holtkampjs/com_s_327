#ifndef NAVIGATION_H
#define NAVIGATION_H

#include "terrain.h"

#define MAPS_N 400

typedef struct navigation
{
    int x;
    int y;
    map_t **maps;
} navigation_t;

int nav_init(navigation_t *n);
int nav_dispose(navigation_t *n);
int nav_display(navigation_t *n);
int nav_north(navigation_t *n);
int nav_south(navigation_t *n);
int nav_east(navigation_t *n);
int nav_west(navigation_t *n);
int nav_fly(navigation_t *n, int x, int y);

#endif