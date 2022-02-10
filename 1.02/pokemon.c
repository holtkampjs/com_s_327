#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "navigation.h"

int nav_test_driver()
{
    int fly_x, fly_y, end = 0;
    char cmd;
    navigation_t nav;

    nav_init(&nav);

    do
    {
        // Placeholder for message space
        putchar('\n');
        putchar('\n');
        putchar('\n');

        // Display map
        nav_display(&nav);
        printf("[ %d, %d ]\n", nav.x, nav.y);

        // Read command
        scanf(" %c", &cmd);

        switch (cmd)
        {
        case 'n':
            end = nav_north(&nav);
            break;
        case 's':
            end = nav_south(&nav);
            break;
        case 'e':
            end = nav_east(&nav);
            break;
        case 'w':
            end = nav_west(&nav);
            break;
        case 'f':
            if (scanf("%d %d", &fly_x, &fly_y) == 2)
            {
                nav_fly(&nav, fly_x, fly_y);
                break;
            }
        case 'q':
            break;
        default:
            printf("Invalid input\n");
        }

        if (end)
        {
            printf("Perished.\nThe world is flat, congratulations, you discovered it.\nYour prize is death.\n");
            break;
        }

    } while (cmd != 'q');

    nav_dispose(&nav);
    return 0;
}

int main(int argc, char *argv[])
{
    srand(time(NULL));
    nav_test_driver();
    return 0;
}