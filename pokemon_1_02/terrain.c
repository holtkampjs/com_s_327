#include <stdio.h>
#include <stdlib.h>
#include "terrain.h"

#define NUM_SEEDS 11

#define inbounds(val, max) ((val) >= 0 && (val) < (max))
#define offset(x, y) (DIM_X * (y) + (x))
#define on_board(x, y) (inbounds((x), DIM_X) && inbounds((y), DIM_Y))

typedef struct weight
{
    int w;
    int dir;
} weight_t;

typedef enum symbol
{
    path = '#',
    pokemon_center = 'C',
    pokemon_mart = 'M',
    grass = ',',
    boulder = '%',
    tree = '^',
    clearing = '.',
    obstacle = '?'
} symbol_t;

#define createSeed(t) ((seed_t){rand() % DIM_X, \
                                rand() % DIM_Y, \
                                (t) % 3 == 0 ? grass : ((t) % 3 == 1 ? clearing : obstacle)})

typedef struct seed
{
    int x;
    int y;
    int type;
} seed_t;

int map_display(map_t *m)
{
    int i, j;
    for (i = 0; i < DIM_Y; i++)
    {
        for (j = 0; j < DIM_X; j++)
            putchar(*(m->terrain + offset(j, i)));
        putchar('\n');
    }
    return 0;
}

int get_weight(symbol_t s)
{
    switch (s)
    {
    case boulder:
        return 100;
    case tree:
        return 75;
    case clearing:
        return 9;
    case grass:
        return 20;
    case path:
        return 3;
    default:
        return 0;
    }
}

int map_place_building(map_t *m, symbol_t s)
{
    int x, y, dist;
    while (1)
    {
        x = rand() % (DIM_X - 10) + 5;
        y = rand() % (DIM_Y - 6) + 3;

        if (*(m->terrain + offset(x, y)) != clearing ||
            *(m->terrain + offset(x + 1, y)) != clearing ||
            *(m->terrain + offset(x, y + 1)) != clearing ||
            *(m->terrain + offset(x + 1, y + 1)) != clearing)
            continue;

        for (dist = 1; *(m->terrain + offset(x + dist, y)) == clearing; dist++)
            ;
        if (dist > 2 && *(m->terrain + offset(x + dist, y)) == path)
            break;
        for (dist = 1; *(m->terrain + offset(x, y + dist)) == clearing; dist++)
            ;
        if (dist > 2 && *(m->terrain + offset(x, y + dist)) == path)
            break;
        for (dist = 1; *(m->terrain + offset(x - dist, y)) == clearing; dist++)
            ;
        if (dist > 2 && *(m->terrain + offset(x - dist, y)) == path)
            break;
        for (dist = 1; *(m->terrain + offset(x, y - dist)) == clearing; dist++)
            ;
        if (dist > 2 && *(m->terrain + offset(x, y - dist)) == path)
            break;
    }

    *(m->terrain + offset(x, y)) = s;
    *(m->terrain + offset(x + 1, y)) = s;
    *(m->terrain + offset(x, y + 1)) = s;
    *(m->terrain + offset(x + 1, y + 1)) = s;

    return 0;
}

int map_calculate_weights(map_t *, weight_t *, int, int, int);

int map_calculate_weights_util(map_t *m, weight_t *w, int x, int y, int current_weight, int dir)
{
    int next_weight = current_weight + get_weight(*(m->terrain + offset(x, y)));
    if ((w + offset(x, y))->w > next_weight)
    {
        (w + offset(x, y))->w = next_weight;
        (w + offset(x, y))->dir = dir;
        map_calculate_weights(m, w, x, y, next_weight);
    }
    return 0;
}

int map_calculate_weights(map_t *m, weight_t *w, int x, int y, int current_weight)
{
    if (on_board(x, y - 1))
        map_calculate_weights_util(m, w, x, y - 1, current_weight, 0);
    if (on_board(x + 1, y))
        map_calculate_weights_util(m, w, x + 1, y, current_weight, 1);
    if (on_board(x, y + 1))
        map_calculate_weights_util(m, w, x, y + 1, current_weight, 2);
    if (on_board(x - 1, y))
        map_calculate_weights_util(m, w, x - 1, y, current_weight, 3);

    return 0;
}

int map_trace_path(map_t *m, weight_t *w, int x, int y)
{
    while ((w + offset(x, y))->w)
    {
        *(m->terrain + offset(x, y)) = path;
        switch ((w + offset(x, y))->dir)
        {
        case 0:
            y++;
            break;
        case 1:
            x--;
            break;
        case 2:
            y--;
            break;
        case 3:
            x++;
            break;
        }
    }

    return 0;
}

int map_generate_path(map_t *m, int start_x, int start_y, int dest_x, int dest_y)
{
    int i, j;
    weight_t *weights = malloc((DIM_X * DIM_Y) * sizeof(*weights));
    for (i = 0; i < DIM_Y; i++)
        for (j = 0; j < DIM_X; j++)
            (weights + offset(j, i))->w = 2048;

    (weights + offset(start_x, start_y))->w = 0;
    map_calculate_weights(m, weights, start_x, start_y, 0);
    map_trace_path(m, weights, dest_x, dest_y);

    free(weights);
    return 0;
}

int expand_seed(map_t *m, seed_t s, int rad, int *remaining)
{
    int i, j, xRad = 4, type = s.type;
    if (s.type == obstacle)
    {
        xRad = 1;
        type = rand() % 2 ? tree : boulder;
    }

    for (i = max(0, s.y - rad); remaining && i < min(s.y + rad, DIM_Y); i++)
    {
        for (j = max(0, s.x - xRad * rad); remaining && j < min(s.x + xRad * rad, DIM_X); j++)
        {
            if (!(*(m->terrain + offset(j, i))) && rand() % 5 && square(((float)1 / (float)xRad) * (j - s.x)) + square(i - s.y) < square(rad))
            {
                *(m->terrain + offset(j, i)) = type;
                *remaining = *remaining - 1;
            }
        }
    }

    return 0;
}

int map_generate_genome(map_t *m)
{
    int i, r, cellsLeft;
    seed_t seeds[NUM_SEEDS];
    for (i = 0; i < NUM_SEEDS; i++)
        seeds[i] = createSeed(i);

    cellsLeft = (DIM_X - 2) * (DIM_Y - 2);

    // Grow seeds in loop
    for (r = 0; cellsLeft; r++)
        for (i = 0; i < NUM_SEEDS; i++)
            expand_seed(m, seeds[i], r, &cellsLeft);

    return 0;
}

int map_set_exits(map_t *m)
{
    if (!m->north_exit || !inbounds(m->north_exit, DIM_X))
        m->north_exit = rand() % (DIM_X - 10) + 5;
    if (!m->south_exit || !inbounds(m->south_exit, DIM_X))
        m->south_exit = rand() % (DIM_X - 10) + 5;
    if (!m->east_exit || !inbounds(m->east_exit, DIM_Y))
        m->east_exit = rand() % (DIM_Y - 6) + 3;
    if (!m->west_exit || !inbounds(m->west_exit, DIM_Y))
        m->west_exit = rand() % (DIM_Y - 6) + 3;

    *(m->terrain + offset(m->north_exit, 0)) = path;
    *(m->terrain + offset(m->south_exit, DIM_Y - 1)) = path;
    *(m->terrain + offset(0, m->west_exit)) = path;
    *(m->terrain + offset(DIM_X - 1, m->east_exit)) = path;

    return 0;
}

int map_create_border(map_t *m)
{
    int i;
    for (i = 0; i < DIM_X; i++)
    {
        *(m->terrain + offset(i, 0)) = boulder;
        *(m->terrain + offset(i, DIM_Y - 1)) = boulder;
    }
    for (i = 0; i < DIM_Y; i++)
    {
        *(m->terrain + offset(0, i)) = boulder;
        *(m->terrain + offset(DIM_X - 1, i)) = boulder;
    }

    return 0;
}

map_t *map_init(int n, int s, int e, int w, int hasPokemonCenter, int hasPokeMart)
{
    map_t *m = malloc(sizeof(*m));
    m->terrain = calloc(((DIM_X * DIM_Y)), sizeof(*m->terrain));
    if (!m->terrain)
    {
        free(m);
        return NULL;
    }

    m->north_exit = n;
    m->south_exit = s;
    m->east_exit = e;
    m->west_exit = w;
    m->hasPokemonCenter = hasPokemonCenter;
    m->hasPokeMart = hasPokeMart;

    map_create_border(m);
    map_set_exits(m);
    map_generate_genome(m);
    map_generate_path(m, 0, m->west_exit, DIM_X - 1, m->east_exit);
    map_generate_path(m, m->north_exit, 0, m->south_exit, DIM_Y - 1);

    if (m->hasPokemonCenter)
        map_place_building(m, pokemon_center);
    if (m->hasPokeMart)
        map_place_building(m, pokemon_mart);

    return m;
}

int map_dispose(map_t *m)
{
    free(m->terrain);
    m->terrain = NULL;
    free(m);
    m = NULL;
    return 0;
}