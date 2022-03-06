#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

#include "navigation.h"

world_t world;

int main(int argc, char *argv[])
{
    uint32_t seed;
    char c;
    int x, y, end = 0;

    seed = argc == 2 ? atoi(argv[1]) : time(NULL);

    printf("Using seed: %u\n", seed);
    srand(seed);

    init_world(&world);

    do
    {
        print_map(&world);
        printf("Current position is %d%cx%d%c (%d,%d). "
               "Enter command: ",
               abs(world.x - WORLD_SIZE / 2),
               world.x >= 0 ? 'E' : 'W',
               abs(world.y - WORLD_SIZE / 2),
               world.y <= 0 ? 'N' : 'S',
               world.x,
               world.y);
        scanf(" %c", &c);

        switch (c)
        {
        case 'n':
            end = travel_north(&world);
            break;
        case 's':
            end = travel_south(&world);
            break;
        case 'e':
            end = travel_east(&world);
            break;
        case 'w':
            end = travel_west(&world);
            break;
        case 'q':
            break;
        case 'f':
            if (scanf(" %d %d", &x, &y) == 2)
            {
                travel_fly(&world, x, y);
                break;
            }
        case '?':
        case 'h':
            printf("Move with 'e'ast, 'w'est, 'n'orth, 's'outh or 'f'ly x y.\n"
                   "Quit with 'q'.  '?' and 'h' print this help message.\n");
            break;
        default:
            fprintf(stderr, "%c: Invalid input.  Enter '?' for help.\n", c);
            break;
        }

        if (end)
        {
            printf("Perished.\nThe world is flat, congratulations, you discovered it.\nYour prize is death.\n");
            break;
        }
    } while (c != 'q');

    delete_world(&world);
    return 0;
}