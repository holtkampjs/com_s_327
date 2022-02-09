#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "terrain.h"
#include "navigation.h"

int main(int argc, char *argv[])
{
    srand(time(NULL));
    nav_test_driver();
    return 0;
}