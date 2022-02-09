#include <stdio.h>

#include "navigation.h"

int nav_test_driver()
{
    //, fly_x, fly_y, count;
    // char cmd;
    navigation_t *nav = nav_init();

    display(nav);

    // while (1)
    // {
    //     count = scanf("%c %d %d", &cmd, &fly_x, &fly_y);
    //     switch (count)
    //     {
    //     case 1:
    //         break;
    //     case 3:
    //         break;
    //     default:
    //         // TODO: Handle invalid argument count
    //     }
    // }

    nav_dispose(nav);
    return 0;
}