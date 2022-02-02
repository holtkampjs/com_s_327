#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <limits.h>

#define MAP_DIM_X 80
#define MAP_DIM_Y 21
#define SEED_COUNT 11

#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))
#define square(a) ((a) * (a))
#define inbounds(x, y) (((x) > 0) && ((x) < MAP_DIM_X) && ((y) > 0) && ((y) < MAP_DIM_Y))
#define index(x, y) (MAP_DIM_X * (y) + (x))

typedef struct seed
{
    int x;
    int y;
    int type;
} seed_t;

typedef struct trail
{
    int weight;
    int dir;
} trail_t;

enum terrain
{
    path = '#',
    pokemon_center = 'C',
    pokemon_mart = 'M',
    grass = ',',
    boulder = '%',
    tree = '^',
    clearing = ' ',
    obstacle = '?'
};

enum terrain_weights
{
    wpath = 3,
    wclearing = 9,
    wgrass = 20,
    wtree = 75,
    wboulder = 100
};

enum compass
{
    dir_n,
    dir_e,
    dir_s,
    dir_w
};

int *map;

void set_seeds(seed_t *arr)
{
    int i;
    for (i = 0; i < SEED_COUNT; i++)
        *(arr + i) = (seed_t){
            .x = (rand() % MAP_DIM_X),
            .y = (rand() % MAP_DIM_Y),
            .type = (i % 3 == 0 ? grass : ((i % 3 == 1) ? clearing : obstacle))};
}

void generate_terrain(seed_t *seeds)
{
    int i, j, k, l, rad, radiusOffset;
    seed_t *seed;
    int *cur;

    for (i = 0, rad = 0; i < ((MAP_DIM_X - 2) * (MAP_DIM_Y - 2)); rad++)
    {
        for (j = 0; j < SEED_COUNT; j++)
        {
            seed = &seeds[j % SEED_COUNT];
            radiusOffset = (seed->type == obstacle) ? 1 : 4;

            for (k = max(seed->y - rad, 0); k < min(seed->y + rad, MAP_DIM_Y); k++)
            {
                for (l = max(seed->x - rad * radiusOffset, 0); l < min(seed->x + rad * radiusOffset, MAP_DIM_X); l++)
                {
                    cur = map + index(l, k);
                    if (*cur == 0 && rand() % 5 && square(((float)1 / (float)radiusOffset) * (l - seed->x)) + square(k - seed->y) < square(rad))
                    {
                        if (seed->type != obstacle)
                            *cur = seed->type;
                        else if (rand() % 2)
                            *cur = tree;
                        else
                            *cur = boulder;
                        i++;
                    }
                }
            }
        }
    }
}

void print_map()
{
    int i, j;
    for (i = 0; i < MAP_DIM_Y; i++)
    {
        for (j = 0; j < MAP_DIM_X; j++)
            printf("%c", *(map + index(j, i)));
        printf("\n");
    }
    fflush(stdout);
}

int get_weight(int x, int y)
{
    switch (*(map + index(x, y)))
    {
    case boulder:
        return wboulder;
    case tree:
        return wtree;
    case clearing:
        return wclearing;
    case grass:
        return wgrass;
    case path:
        return wpath;
    }

    return 0;
}

void print_weights()
{
    int i, j;
    for (i = 0; i < MAP_DIM_Y; i++)
    {
        for (j = 0; j < MAP_DIM_X; j++)
            printf("%3d ", get_weight(j, i));
        printf("\n");
    }
    fflush(stdout);
}

void add_border()
{
    int i;
    for (i = 0; i < MAP_DIM_X; i++)
    {
        *(map + index(i, 0)) = '%';
        *(map + index(i, MAP_DIM_Y - 1)) = '%';
    }

    for (i = 0; i < MAP_DIM_Y - 2; i++)
    {
        *(map + index(0, i + 1)) = '%';
        *(map + index(MAP_DIM_X - 1, i + 1)) = '%';
    }
}

int create_weighted_path(trail_t *trail, int x, int y, int xDest, int yDest)
{
    int current_weight;
    if (x == xDest && y == yDest)
        return 0;

    current_weight = (trail + index(x, y))->weight;
    if (inbounds(x, y - 1) && get_weight(x, y - 1) + current_weight < (trail + index(x, y - 1))->weight)
    {
        (trail + index(x, y - 1))->weight = get_weight(x, y - 1) + current_weight;
        (trail + index(x, y - 1))->dir = dir_n;
        create_weighted_path(trail, x, y - 1, xDest, yDest);
    }
    if (inbounds(x + 1, y) && get_weight(x + 1, y) + current_weight < (trail + index(x + 1, y))->weight)
    {
        (trail + index(x + 1, y))->weight = get_weight(x + 1, y) + current_weight;
        (trail + index(x + 1, y))->dir = dir_e;
        create_weighted_path(trail, x + 1, y, xDest, yDest);
    }
    if (inbounds(x, y + 1) && get_weight(x, y + 1) + current_weight < (trail + index(x, y + 1))->weight)
    {
        (trail + index(x, y + 1))->weight = get_weight(x, y + 1) + current_weight;
        (trail + index(x, y + 1))->dir = dir_s;
        create_weighted_path(trail, x, y + 1, xDest, yDest);
    }
    if (inbounds(x - 1, y) && get_weight(x - 1, y) + current_weight < (trail + index(x - 1, y))->weight)
    {
        (trail + index(x - 1, y))->weight = get_weight(x - 1, y) + current_weight;
        (trail + index(x - 1, y))->dir = dir_w;
        create_weighted_path(trail, x - 1, y, xDest, yDest);
    }

    return 0;
}

int trace_path(trail_t *trail, int x, int y)
{
    if ((trail + index(x, y))->weight == 0)
        return 0;

    *(map + index(x, y)) = path;

    switch ((trail + index(x, y))->dir)
    {
    case dir_n:
        return trace_path(trail, x, y + 1);
    case dir_e:
        return trace_path(trail, x - 1, y);
    case dir_s:
        return trace_path(trail, x, y - 1);
    case dir_w:
        return trace_path(trail, x + 1, y);
    }

    return 1;
}

void add_path(seed_t *seeds, int north, int south, int east, int west)
{
    int i, j;
    trail_t *weighted_map = malloc((MAP_DIM_Y * MAP_DIM_X) * sizeof(*weighted_map));

    // Add entrances if not preset
    if (north < 0 || north >= MAP_DIM_X)
        north = rand() % (MAP_DIM_X - 10) + 5;
    if (south < 0 || south >= MAP_DIM_X)
        south = rand() % (MAP_DIM_X - 10) + 5;
    if (east < 0 || east >= MAP_DIM_Y)
        east = rand() % (MAP_DIM_Y - 6) + 3;
    if (west < 0 || west >= MAP_DIM_Y)
        west = rand() % (MAP_DIM_Y - 6) + 3;

    *(map + index(north, 0)) = path;
    *(map + index(south, MAP_DIM_Y - 1)) = path;
    *(map + index(0, west)) = path;
    *(map + index(MAP_DIM_X - 1, east)) = path;

    for (i = 0; i < MAP_DIM_Y; i++)
        for (j = 0; j < MAP_DIM_X; j++)
            (weighted_map + index(j, i))->weight = (MAP_DIM_X * MAP_DIM_Y * wboulder);

    (weighted_map + index(0, west))->weight = 0;
    create_weighted_path(weighted_map, 0, west, MAP_DIM_X - 1, east);
    trace_path(weighted_map, MAP_DIM_X - 1, east);

    for (i = 0; i < MAP_DIM_Y; i++)
        for (j = 0; j < MAP_DIM_X; j++)
            (weighted_map + index(j, i))->weight = (MAP_DIM_X * MAP_DIM_Y * wboulder);

    (weighted_map + index(north, 0))->weight = 0;
    create_weighted_path(weighted_map, north, 0, south, MAP_DIM_Y - 1);
    trace_path(weighted_map, south, MAP_DIM_Y - 1);

    free(weighted_map);
}

int *map_gen()
{
    seed_t *seeds;

    srand(time(NULL));

    seeds = malloc(SEED_COUNT * sizeof(*seeds));
    map = calloc((MAP_DIM_X * MAP_DIM_Y), sizeof(*map));

    set_seeds(seeds);

    add_border();
    generate_terrain(seeds);
    add_path(seeds, -1, -1, -1, -1);

    print_map();
    // print_weights();

    free(seeds);
    return map;
}

int main(int argc, char *argv[])
{
    free(map_gen());
    return 0;
}

/*
    Constraints:
    - Map is 80 X 21 (x X y) with a total display of 80 X 24
    - Must contain at least two regions of tall grass
    - Must contain at least two clearings
    - At least 1 road North-South and 1 road East-West
    - Roads must intersect at least once
    - Pokemon Center and Mart will be 2 X 2
    - Pokemon Center and Mart must be reachable from path without passing through tall grass
    - Edges of map are boulders with one exit on each edge

      |-------------------------|
      |      Symbol Lookup      |
      |-------------------------|
      |   Path            #     |
      |   Pokemon Center  C     |
      |   Pokemon Mart    M     |
      |   Tall Grass      ,     |
      |   Boulder         %     |
      |   Tree            ^     |
      |-------------------------|
*/