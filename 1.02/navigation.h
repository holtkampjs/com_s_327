#ifndef NAVIGATION_H
#define NAVIGATION_H

#include "terrain.h"

#define MAPS_N 399

typedef struct navigation
{
    int id;
    map_t **maps;
} navigation_t;

navigation_t *nav_init();

/**
 * @brief Disposes of a navigation struct given
 *
 * @param n struct navigation
 * @return int
 */
int nav_dispose(navigation_t *n);

/**
 * @brief Displays the current map
 *
 * @param n struct navigation
 * @return int
 */
int display(navigation_t *n);

/**
 * @brief Test driver for the navigation logic
 *
 * @return int
 */
int nav_test_driver();

#endif