#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define MAP_DIM_X 80
#define MAP_DIM_Y 21
#define SEED_COUNT 9

#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))
#define square(a) (a * a)

typedef struct seed
{
    int x;
    int y;
    int type;
} seed_t;

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
    int i, j, k, l, rad, isObstacle;
    seed_t *seed;
    int *cur;

    for (i = 0, rad = 0; i < ((MAP_DIM_X - 2) * (MAP_DIM_Y - 2)); rad++)
    {
        for (j = 0; j < SEED_COUNT; j++)
        {
            seed = &seeds[j % SEED_COUNT];
            isObstacle = seed->type == obstacle;

            for (k = max(seed->y - rad, 0); k < min(seed->y + rad, MAP_DIM_Y); k++)
            {
                for (l = max(seed->x - rad * (isObstacle ? 1 : 4), 0); l < min(seed->x + rad * (isObstacle ? 1 : 4), MAP_DIM_X); l++)
                {
                    cur = map + MAP_DIM_X * k + l;
                    if (*cur == 0)
                    {
                        if (isObstacle)
                        {
                            if (rand() % 10 == 0 && (square(l - seed->x) + square(k - seed->y)) < rad)
                            {
                                *cur = rand() % 2 ? tree : boulder;
                                i++;
                            }
                        }
                        else
                        {
                            if (rand() % 4 && square(0.25 * (l - seed->x)) + square(k - seed->y) < square(rad))
                            {
                                *cur = seed->type;
                                i++;
                            }
                        }
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
            printf("%c", *(map + MAP_DIM_X * i + j));
        printf("\n");
    }
}

// Assumes map is wider than tall
void add_border()
{
    int i;
    for (i = 0; i < MAP_DIM_X; i++)
    {
        *(map + i) = '%';
        *(map + MAP_DIM_X * (MAP_DIM_Y - 1) + i) = '%';

        if (i < MAP_DIM_Y - 2)
        {
            *(map + MAP_DIM_X * (i + 1)) = '%';
            *(map + MAP_DIM_X * (i + 2) - 1) = '%';
        }
    }
}

int *map_gen()
{
    seed_t *seeds;

    srand(time(NULL));

    seeds = (seed_t *)malloc(SEED_COUNT * sizeof(seed_t));
    map = (int *)calloc((MAP_DIM_X * MAP_DIM_Y), sizeof(int));

    set_seeds(seeds);

    add_border();
    generate_terrain(seeds);
    print_map();

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