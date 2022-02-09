#include <stdlib.h>
#include <stdio.h>

#include "terrain.h"
#include "navigation.h"

#define get_id(x, y) (199 * (y) + (x) + 199)

navigation_t *nav_init()
{
    navigation_t *n = malloc(sizeof(*n));

    n->maps = malloc(MAPS_N * MAPS_N * sizeof(*(n->maps)));
    if (n->maps == NULL)
    {
        fprintf(stderr, "Failed to initiallize navigation\n");
        free(n);
        return NULL;
    }

    n->id = get_id(0, 0);
    *(n->maps + n->id) = map_init(-1, -1, -1, -1, 1, 1);

    return n;
}

// TODO: Likely where new map creation will happen
map_t *nav_get_map(navigation_t *n)
{
    map_t *m = *(n->maps + n->id);

    return m;
}

int display(navigation_t *n)
{
    map_display(nav_get_map(n));
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