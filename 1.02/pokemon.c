#include "terrain.h"

int main(int argc, char *argv[])
{
    map_t m = {
        .north_exit = -1,
        .east_exit = -1,
        .south_exit = -1,
        .west_exit = -1,
        .hasPokeMart = 1,
        .hasPokemonCenter = 1};

    map_init(&m);
    map_display(&m);
    map_dispose(&m);

    return 0;
}