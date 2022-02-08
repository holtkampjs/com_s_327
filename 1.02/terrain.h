#ifndef TERRAIN_H
#define TERRAIN_H

#define DIM_X 80
#define DIM_Y 21

#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))
#define square(a) ((a) * (a))

typedef struct map
{
    int *terrain;
    int hasPokeMart;
    int hasPokemonCenter;
    int north_exit;
    int south_exit;
    int east_exit;
    int west_exit;
} map_t;

typedef enum direction
{
    north,
    east,
    south,
    west
} direction_t;

int map_init(map_t *m);
int map_display(map_t *m);
int map_dispose(map_t *m);

#endif