#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#include "terrain.h"
#include "navigation.h"

#define get_id(x, y) (399 * ((y) + 199) + (x) + 199)

int nav_init(navigation_t *n)
{
    n->maps = malloc(MAPS_N * MAPS_N * sizeof(*(n->maps)));
    n->x = 0;
    n->y = 0;
    return 0;
}

#define get_map(n, dx, dy) (*((n)->maps + get_id((n)->x + (dx), (n)->y + (dy))))
#define in_bounds_nav(x, y) ((x) + 199 >= 0 && (x) + 199 < MAPS_N && (y) + 199 >= 0 && (y) + 199 < MAPS_N)

int find_exits(navigation_t *n, int *n_ex, int *s_ex, int *e_ex, int *w_ex)
{
    if (in_bounds_nav(n->x, n->y - 1))
        *n_ex = get_map(n, 0, -1) != NULL ? get_map(n, 0, -1)->south_exit : -1;
    if (in_bounds_nav(n->x, n->y + 1))
        *s_ex = get_map(n, 0, 1) != NULL ? get_map(n, 0, 1)->north_exit : -1;
    if (in_bounds_nav(n->x + 1, n->y))
        *e_ex = get_map(n, 1, 0) != NULL ? get_map(n, 1, 0)->west_exit : -1;
    if (in_bounds_nav(n->x - 1, n->y))
        *w_ex = get_map(n, -1, 0) != NULL ? get_map(n, -1, 0)->east_exit : -1;
    return 0;
}

int building_check(navigation_t *n, int *center, int *mart)
{
    double d = sqrt((n->x * n->x) + (n->y * n->y));
    int prob = (((-45 * d) / 200) + 50) / 100;
    *center = prob;
    *mart = prob;
    return 0;
}

map_t *nav_get_map(navigation_t *n)
{
    int n_exit, s_exit, e_exit, w_exit, hasCenters, hasMarts;
    map_t *m = *(n->maps + get_id(n->x, n->y));

    if (!m)
    {
        find_exits(n, &n_exit, &s_exit, &e_exit, &w_exit);
        building_check(n, &hasCenters, &hasMarts);
        m = map_init(n_exit, s_exit, e_exit, w_exit, hasCenters, hasMarts);
        *(n->maps + get_id(n->x, n->y)) = m;
    }

    return m;
}

int nav_north(navigation_t *n)
{
    if (!in_bounds_nav(n->x, n->y - 1))
        return 1;
    n->y = n->y - 1;
    return 0;
}

int nav_south(navigation_t *n)
{
    if (!in_bounds_nav(n->x, n->y + 1))
        return 1;
    n->y = n->y + 1;
    return 0;
}

int nav_east(navigation_t *n)
{
    if (!in_bounds_nav(n->x + 1, n->y))
        return 1;
    n->x = n->x + 1;
    return 0;
}

int nav_west(navigation_t *n)
{
    if (!in_bounds_nav(n->x - 1, n->y))
        return 1;
    n->x = n->x - 1;
    return 0;
}

int nav_fly(navigation_t *n, int x, int y)
{
    if (!in_bounds_nav(x, y))
        return 1;
    n->x = x;
    n->y = y;
    return 0;
}

int nav_display(navigation_t *n)
{
    map_t *m = nav_get_map(n);
    map_display(m);
    return 0;
}

int nav_dispose(navigation_t *n)
{
    int i;
    if (!n)
    {
        fprintf(stderr, "Cannot dispose of null pointer\n");
        return 1;
    }
    else if (!n->maps)
    {
        fprintf(stderr, "Maps pointers not initialized\n");
        return 1;
    }

    for (i = 0; i < MAPS_N * MAPS_N; i++)
        if (*(n->maps + i))
            map_dispose(*(n->maps + i));

    free(n->maps);
    return 0;
}
