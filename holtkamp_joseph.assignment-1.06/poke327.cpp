#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "character.h"
#include "heap.h"
#include "io.h"
#include "poke327.h"

#include <bits/stdc++.h>
#include <cstring>
#include <fstream>
#include <ios>
#include <iostream>
#include <iterator>
#include <list>
#include <new>
#include <ostream>
#include <sstream>
#include <string>

using namespace std;
#define stoi(a) (a.empty() ? -1 : stoi(a))

typedef struct queue_node {
  int x, y;
  struct queue_node *next;
} queue_node_t;

World world;

pair_t all_dirs[8] = {
    {-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1},
};

static int32_t path_cmp(const void *key, const void *with) {
  return ((path_t *)key)->cost - ((path_t *)with)->cost;
}

static int32_t edge_penalty(int8_t x, int8_t y) {
  return (x == 1 || y == 1 || x == MAP_X - 2 || y == MAP_Y - 2) ? 2 : 1;
}

static void dijkstra_path(Map *m, pair_t from, pair_t to) {
  static path_t path[MAP_Y][MAP_X], *p;
  static uint32_t initialized = 0;
  heap_t h;
  int x, y;

  if (!initialized) {
    for (y = 0; y < MAP_Y; y++) {
      for (x = 0; x < MAP_X; x++) {
        path[y][x].pos[dim_y] = y;
        path[y][x].pos[dim_x] = x;
      }
    }
    initialized = 1;
  }

  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      path[y][x].cost = INT_MAX;
    }
  }

  path[from[dim_y]][from[dim_x]].cost = 0;

  heap_init(&h, path_cmp, NULL);

  for (y = 1; y < MAP_Y - 1; y++) {
    for (x = 1; x < MAP_X - 1; x++) {
      path[y][x].hn = heap_insert(&h, &path[y][x]);
    }
  }

  while ((p = (path_t *)heap_remove_min(&h))) {
    p->hn = NULL;

    if ((p->pos[dim_y] == to[dim_y]) && p->pos[dim_x] == to[dim_x]) {
      for (x = to[dim_x], y = to[dim_y];
           (x != from[dim_x]) || (y != from[dim_y]);
           p = &path[y][x], x = p->from[dim_x], y = p->from[dim_y]) {
        mapxy(x, y) = ter_path;
        heightxy(x, y) = 0;
      }
      heap_delete(&h);
      return;
    }

    if ((path[p->pos[dim_y] - 1][p->pos[dim_x]].hn) &&
        (path[p->pos[dim_y] - 1][p->pos[dim_x]].cost >
         ((p->cost + heightpair(p->pos)) *
          edge_penalty(p->pos[dim_x], p->pos[dim_y] - 1)))) {
      path[p->pos[dim_y] - 1][p->pos[dim_x]].cost =
          ((p->cost + heightpair(p->pos)) *
           edge_penalty(p->pos[dim_x], p->pos[dim_y] - 1));
      path[p->pos[dim_y] - 1][p->pos[dim_x]].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y] - 1][p->pos[dim_x]].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h,
                                   path[p->pos[dim_y] - 1][p->pos[dim_x]].hn);
    }
    if ((path[p->pos[dim_y]][p->pos[dim_x] - 1].hn) &&
        (path[p->pos[dim_y]][p->pos[dim_x] - 1].cost >
         ((p->cost + heightpair(p->pos)) *
          edge_penalty(p->pos[dim_x] - 1, p->pos[dim_y])))) {
      path[p->pos[dim_y]][p->pos[dim_x] - 1].cost =
          ((p->cost + heightpair(p->pos)) *
           edge_penalty(p->pos[dim_x] - 1, p->pos[dim_y]));
      path[p->pos[dim_y]][p->pos[dim_x] - 1].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y]][p->pos[dim_x] - 1].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h,
                                   path[p->pos[dim_y]][p->pos[dim_x] - 1].hn);
    }
    if ((path[p->pos[dim_y]][p->pos[dim_x] + 1].hn) &&
        (path[p->pos[dim_y]][p->pos[dim_x] + 1].cost >
         ((p->cost + heightpair(p->pos)) *
          edge_penalty(p->pos[dim_x] + 1, p->pos[dim_y])))) {
      path[p->pos[dim_y]][p->pos[dim_x] + 1].cost =
          ((p->cost + heightpair(p->pos)) *
           edge_penalty(p->pos[dim_x] + 1, p->pos[dim_y]));
      path[p->pos[dim_y]][p->pos[dim_x] + 1].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y]][p->pos[dim_x] + 1].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h,
                                   path[p->pos[dim_y]][p->pos[dim_x] + 1].hn);
    }
    if ((path[p->pos[dim_y] + 1][p->pos[dim_x]].hn) &&
        (path[p->pos[dim_y] + 1][p->pos[dim_x]].cost >
         ((p->cost + heightpair(p->pos)) *
          edge_penalty(p->pos[dim_x], p->pos[dim_y] + 1)))) {
      path[p->pos[dim_y] + 1][p->pos[dim_x]].cost =
          ((p->cost + heightpair(p->pos)) *
           edge_penalty(p->pos[dim_x], p->pos[dim_y] + 1));
      path[p->pos[dim_y] + 1][p->pos[dim_x]].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y] + 1][p->pos[dim_x]].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h,
                                   path[p->pos[dim_y] + 1][p->pos[dim_x]].hn);
    }
  }
}

static int build_paths(Map *m) {
  pair_t from, to;

  /*  printf("%d %d %d %d\n", m->n, m->s, m->e, m->w);*/

  if (m->e != -1 && m->w != -1) {
    from[dim_x] = 1;
    to[dim_x] = MAP_X - 2;
    from[dim_y] = m->w;
    to[dim_y] = m->e;

    dijkstra_path(m, from, to);
  }

  if (m->n != -1 && m->s != -1) {
    from[dim_y] = 1;
    to[dim_y] = MAP_Y - 2;
    from[dim_x] = m->n;
    to[dim_x] = m->s;

    dijkstra_path(m, from, to);
  }

  if (m->e == -1) {
    if (m->s == -1) {
      from[dim_x] = 1;
      from[dim_y] = m->w;
      to[dim_x] = m->n;
      to[dim_y] = 1;
    } else {
      from[dim_x] = 1;
      from[dim_y] = m->w;
      to[dim_x] = m->s;
      to[dim_y] = MAP_Y - 2;
    }

    dijkstra_path(m, from, to);
  }

  if (m->w == -1) {
    if (m->s == -1) {
      from[dim_x] = MAP_X - 2;
      from[dim_y] = m->e;
      to[dim_x] = m->n;
      to[dim_y] = 1;
    } else {
      from[dim_x] = MAP_X - 2;
      from[dim_y] = m->e;
      to[dim_x] = m->s;
      to[dim_y] = MAP_Y - 2;
    }

    dijkstra_path(m, from, to);
  }

  if (m->n == -1) {
    if (m->e == -1) {
      from[dim_x] = 1;
      from[dim_y] = m->w;
      to[dim_x] = m->s;
      to[dim_y] = MAP_Y - 2;
    } else {
      from[dim_x] = MAP_X - 2;
      from[dim_y] = m->e;
      to[dim_x] = m->s;
      to[dim_y] = MAP_Y - 2;
    }

    dijkstra_path(m, from, to);
  }

  if (m->s == -1) {
    if (m->e == -1) {
      from[dim_x] = 1;
      from[dim_y] = m->w;
      to[dim_x] = m->n;
      to[dim_y] = 1;
    } else {
      from[dim_x] = MAP_X - 2;
      from[dim_y] = m->e;
      to[dim_x] = m->n;
      to[dim_y] = 1;
    }

    dijkstra_path(m, from, to);
  }

  return 0;
}

static int gaussian[5][5] = {{1, 4, 7, 4, 1},
                             {4, 16, 26, 16, 4},
                             {7, 26, 41, 26, 7},
                             {4, 16, 26, 16, 4},
                             {1, 4, 7, 4, 1}};

static int smooth_height(Map *m) {
  int32_t i, x, y;
  int32_t s, t, p, q;
  queue_node_t *head, *tail, *tmp;
  /*  FILE *out;*/
  uint8_t height[MAP_Y][MAP_X];

  memset(&height, 0, sizeof(height));

  /* Seed with some values */
  for (i = 1; i < 255; i += 20) {
    do {
      x = rand() % MAP_X;
      y = rand() % MAP_Y;
    } while (height[y][x]);
    height[y][x] = i;
    if (i == 1) {
      head = tail = (queue_node_t *)malloc(sizeof(*tail));
    } else {
      tail->next = (queue_node_t *)malloc(sizeof(*tail));
      tail = tail->next;
    }
    tail->next = NULL;
    tail->x = x;
    tail->y = y;
  }

  /*
  out = fopen("seeded.pgm", "w");
  fprintf(out, "P5\n%u %u\n255\n", MAP_X, MAP_Y);
  fwrite(&height, sizeof (height), 1, out);
  fclose(out);
  */

  /* Diffuse the vaules to fill the space */
  while (head) {
    x = head->x;
    y = head->y;
    i = height[y][x];

    if (x - 1 >= 0 && y - 1 >= 0 && !height[y - 1][x - 1]) {
      height[y - 1][x - 1] = i;
      tail->next = (queue_node_t *)malloc(sizeof(*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x - 1;
      tail->y = y - 1;
    }
    if (x - 1 >= 0 && !height[y][x - 1]) {
      height[y][x - 1] = i;
      tail->next = (queue_node_t *)malloc(sizeof(*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x - 1;
      tail->y = y;
    }
    if (x - 1 >= 0 && y + 1 < MAP_Y && !height[y + 1][x - 1]) {
      height[y + 1][x - 1] = i;
      tail->next = (queue_node_t *)malloc(sizeof(*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x - 1;
      tail->y = y + 1;
    }
    if (y - 1 >= 0 && !height[y - 1][x]) {
      height[y - 1][x] = i;
      tail->next = (queue_node_t *)malloc(sizeof(*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x;
      tail->y = y - 1;
    }
    if (y + 1 < MAP_Y && !height[y + 1][x]) {
      height[y + 1][x] = i;
      tail->next = (queue_node_t *)malloc(sizeof(*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x;
      tail->y = y + 1;
    }
    if (x + 1 < MAP_X && y - 1 >= 0 && !height[y - 1][x + 1]) {
      height[y - 1][x + 1] = i;
      tail->next = (queue_node_t *)malloc(sizeof(*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x + 1;
      tail->y = y - 1;
    }
    if (x + 1 < MAP_X && !height[y][x + 1]) {
      height[y][x + 1] = i;
      tail->next = (queue_node_t *)malloc(sizeof(*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x + 1;
      tail->y = y;
    }
    if (x + 1 < MAP_X && y + 1 < MAP_Y && !height[y + 1][x + 1]) {
      height[y + 1][x + 1] = i;
      tail->next = (queue_node_t *)malloc(sizeof(*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x + 1;
      tail->y = y + 1;
    }

    tmp = head;
    head = head->next;
    free(tmp);
  }

  /* And smooth it a bit with a gaussian convolution */
  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      for (s = t = p = 0; p < 5; p++) {
        for (q = 0; q < 5; q++) {
          if (y + (p - 2) >= 0 && y + (p - 2) < MAP_Y && x + (q - 2) >= 0 &&
              x + (q - 2) < MAP_X) {
            s += gaussian[p][q];
            t += height[y + (p - 2)][x + (q - 2)] * gaussian[p][q];
          }
        }
      }
      m->height[y][x] = t / s;
    }
  }
  /* Let's do it again, until it's smooth like Kenny G. */
  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      for (s = t = p = 0; p < 5; p++) {
        for (q = 0; q < 5; q++) {
          if (y + (p - 2) >= 0 && y + (p - 2) < MAP_Y && x + (q - 2) >= 0 &&
              x + (q - 2) < MAP_X) {
            s += gaussian[p][q];
            t += height[y + (p - 2)][x + (q - 2)] * gaussian[p][q];
          }
        }
      }
      m->height[y][x] = t / s;
    }
  }

  return 0;
}

static void find_building_location(Map *m, pair_t p) {
  do {
    p[dim_x] = rand() % (MAP_X - 5) + 3;
    p[dim_y] = rand() % (MAP_Y - 10) + 5;

    if ((((mapxy(p[dim_x] - 1, p[dim_y]) == ter_path) &&
          (mapxy(p[dim_x] - 1, p[dim_y] + 1) == ter_path)) ||
         ((mapxy(p[dim_x] + 2, p[dim_y]) == ter_path) &&
          (mapxy(p[dim_x] + 2, p[dim_y] + 1) == ter_path)) ||
         ((mapxy(p[dim_x], p[dim_y] - 1) == ter_path) &&
          (mapxy(p[dim_x] + 1, p[dim_y] - 1) == ter_path)) ||
         ((mapxy(p[dim_x], p[dim_y] + 2) == ter_path) &&
          (mapxy(p[dim_x] + 1, p[dim_y] + 2) == ter_path))) &&
        (((mapxy(p[dim_x], p[dim_y]) != ter_mart) &&
          (mapxy(p[dim_x], p[dim_y]) != ter_center) &&
          (mapxy(p[dim_x] + 1, p[dim_y]) != ter_mart) &&
          (mapxy(p[dim_x] + 1, p[dim_y]) != ter_center) &&
          (mapxy(p[dim_x], p[dim_y] + 1) != ter_mart) &&
          (mapxy(p[dim_x], p[dim_y] + 1) != ter_center) &&
          (mapxy(p[dim_x] + 1, p[dim_y] + 1) != ter_mart) &&
          (mapxy(p[dim_x] + 1, p[dim_y] + 1) != ter_center))) &&
        (((mapxy(p[dim_x], p[dim_y]) != ter_path) &&
          (mapxy(p[dim_x] + 1, p[dim_y]) != ter_path) &&
          (mapxy(p[dim_x], p[dim_y] + 1) != ter_path) &&
          (mapxy(p[dim_x] + 1, p[dim_y] + 1) != ter_path)))) {
      break;
    }
  } while (1);
}

static int place_pokemart(Map *m) {
  pair_t p;

  find_building_location(m, p);

  mapxy(p[dim_x], p[dim_y]) = ter_mart;
  mapxy(p[dim_x] + 1, p[dim_y]) = ter_mart;
  mapxy(p[dim_x], p[dim_y] + 1) = ter_mart;
  mapxy(p[dim_x] + 1, p[dim_y] + 1) = ter_mart;

  return 0;
}

static int place_center(Map *m) {
  pair_t p;

  find_building_location(m, p);

  mapxy(p[dim_x], p[dim_y]) = ter_center;
  mapxy(p[dim_x] + 1, p[dim_y]) = ter_center;
  mapxy(p[dim_x], p[dim_y] + 1) = ter_center;
  mapxy(p[dim_x] + 1, p[dim_y] + 1) = ter_center;

  return 0;
}

static int map_terrain(Map *m, int8_t n, int8_t s, int8_t e, int8_t w) {
  int32_t i, x, y;
  queue_node_t *head, *tail, *tmp;
  //  FILE *out;
  int num_grass, num_clearing, num_mountain, num_forest, num_total;
  terrain_type_t type;
  int added_current = 0;

  num_grass = rand() % 4 + 2;
  num_clearing = rand() % 4 + 2;
  num_mountain = rand() % 2 + 1;
  num_forest = rand() % 2 + 1;
  num_total = num_grass + num_clearing + num_mountain + num_forest;

  memset(&m->map, 0, sizeof(m->map));

  /* Seed with some values */
  for (i = 0; i < num_total; i++) {
    do {
      x = rand() % MAP_X;
      y = rand() % MAP_Y;
    } while (m->map[y][x]);
    if (i == 0) {
      type = ter_grass;
    } else if (i == num_grass) {
      type = ter_clearing;
    } else if (i == num_grass + num_clearing) {
      type = ter_mountain;
    } else if (i == num_grass + num_clearing + num_mountain) {
      type = ter_forest;
    }
    m->map[y][x] = type;
    if (i == 0) {
      head = tail = (queue_node_t *)malloc(sizeof(*tail));
    } else {
      tail->next = (queue_node_t *)malloc(sizeof(*tail));
      tail = tail->next;
    }
    tail->next = NULL;
    tail->x = x;
    tail->y = y;
  }

  /* Diffuse the vaules to fill the space */
  while (head) {
    x = head->x;
    y = head->y;
    i = m->map[y][x];

    if (x - 1 >= 0 && !m->map[y][x - 1]) {
      if ((rand() % 100) < 80) {
        m->map[y][x - 1] = (terrain_type_t)i;
        tail->next = (queue_node_t *)malloc(sizeof(*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x - 1;
        tail->y = y;
      } else if (!added_current) {
        added_current = 1;
        m->map[y][x] = (terrain_type_t)i;
        tail->next = (queue_node_t *)malloc(sizeof(*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x;
        tail->y = y;
      }
    }

    if (y - 1 >= 0 && !m->map[y - 1][x]) {
      if ((rand() % 100) < 20) {
        m->map[y - 1][x] = (terrain_type_t)i;
        tail->next = (queue_node_t *)malloc(sizeof(*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x;
        tail->y = y - 1;
      } else if (!added_current) {
        added_current = 1;
        m->map[y][x] = (terrain_type_t)i;
        tail->next = (queue_node_t *)malloc(sizeof(*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x;
        tail->y = y;
      }
    }

    if (y + 1 < MAP_Y && !m->map[y + 1][x]) {
      if ((rand() % 100) < 20) {
        m->map[y + 1][x] = (terrain_type_t)i;
        tail->next = (queue_node_t *)malloc(sizeof(*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x;
        tail->y = y + 1;
      } else if (!added_current) {
        added_current = 1;
        m->map[y][x] = (terrain_type_t)i;
        tail->next = (queue_node_t *)malloc(sizeof(*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x;
        tail->y = y;
      }
    }

    if (x + 1 < MAP_X && !m->map[y][x + 1]) {
      if ((rand() % 100) < 80) {
        m->map[y][x + 1] = (terrain_type_t)i;
        tail->next = (queue_node_t *)malloc(sizeof(*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x + 1;
        tail->y = y;
      } else if (!added_current) {
        added_current = 1;
        m->map[y][x] = (terrain_type_t)i;
        tail->next = (queue_node_t *)malloc(sizeof(*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x;
        tail->y = y;
      }
    }

    added_current = 0;
    tmp = head;
    head = head->next;
    free(tmp);
  }

  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      if (y == 0 || y == MAP_Y - 1 || x == 0 || x == MAP_X - 1) {
        mapxy(x, y) = ter_boulder;
      }
    }
  }

  m->n = n;
  m->s = s;
  m->e = e;
  m->w = w;

  if (n != -1) {
    mapxy(n, 0) = ter_exit;
    mapxy(n, 1) = ter_path;
  }
  if (s != -1) {
    mapxy(s, MAP_Y - 1) = ter_exit;
    mapxy(s, MAP_Y - 2) = ter_path;
  }
  if (w != -1) {
    mapxy(0, w) = ter_exit;
    mapxy(1, w) = ter_path;
  }
  if (e != -1) {
    mapxy(MAP_X - 1, e) = ter_exit;
    mapxy(MAP_X - 2, e) = ter_path;
  }

  return 0;
}

static int place_boulders(Map *m) {
  int i;
  int x, y;

  for (i = 0; i < MIN_BOULDERS || rand() % 100 < BOULDER_PROB; i++) {
    y = rand() % (MAP_Y - 2) + 1;
    x = rand() % (MAP_X - 2) + 1;
    if (m->map[y][x] != ter_forest && m->map[y][x] != ter_path) {
      m->map[y][x] = ter_boulder;
    }
  }

  return 0;
}

static int place_trees(Map *m) {
  int i;
  int x, y;

  for (i = 0; i < MIN_TREES || rand() % 100 < TREE_PROB; i++) {
    y = rand() % (MAP_Y - 2) + 1;
    x = rand() % (MAP_X - 2) + 1;
    if (m->map[y][x] != ter_mountain && m->map[y][x] != ter_path) {
      m->map[y][x] = ter_tree;
    }
  }

  return 0;
}

void rand_pos(pair_t pos) {
  pos[dim_x] = (rand() % (MAP_X - 2)) + 1;
  pos[dim_y] = (rand() % (MAP_Y - 2)) + 1;
}

void new_hiker() {
  pair_t pos;
  Character *c;

  do {
    rand_pos(pos);
  } while (world.hiker_dist[pos[dim_y]][pos[dim_x]] == INT_MAX ||
           world.cur_map->cmap[pos[dim_y]][pos[dim_x]] || pos[dim_x] < 3 ||
           pos[dim_x] > MAP_X - 4 || pos[dim_y] < 3 || pos[dim_y] > MAP_Y - 4);

  world.cur_map->cmap[pos[dim_y]][pos[dim_x]] = c =
      (Character *)malloc(sizeof(*c));
  c->npc = (npc_t *)malloc(sizeof(*c->npc));
  c->pos[dim_y] = pos[dim_y];
  c->pos[dim_x] = pos[dim_x];
  c->npc->ctype = char_hiker;
  c->npc->mtype = move_hiker;
  c->npc->dir[dim_x] = 0;
  c->npc->dir[dim_y] = 0;
  c->npc->defeated = 0;
  c->pc = NULL;
  c->symbol = 'h';
  c->next_turn = 0;
  heap_insert(&world.cur_map->turn, c);

  //  printf("Hiker at %d,%d\n", pos[dim_x], pos[dim_y]);
}

void new_rival() {
  pair_t pos;
  Character *c;

  do {
    rand_pos(pos);
  } while (world.rival_dist[pos[dim_y]][pos[dim_x]] == INT_MAX ||
           world.rival_dist[pos[dim_y]][pos[dim_x]] < 0 ||
           world.cur_map->cmap[pos[dim_y]][pos[dim_x]] || pos[dim_x] < 3 ||
           pos[dim_x] > MAP_X - 4 || pos[dim_y] < 3 || pos[dim_y] > MAP_Y - 4);

  world.cur_map->cmap[pos[dim_y]][pos[dim_x]] = c =
      (Character *)malloc(sizeof(*c));
  c->npc = (npc_t *)malloc(sizeof(*c->npc));
  c->pos[dim_y] = pos[dim_y];
  c->pos[dim_x] = pos[dim_x];
  c->npc->ctype = char_rival;
  c->npc->mtype = move_rival;
  c->npc->dir[dim_x] = 0;
  c->npc->dir[dim_y] = 0;
  c->npc->defeated = 0;
  c->pc = NULL;
  c->symbol = 'r';
  c->next_turn = 0;
  heap_insert(&world.cur_map->turn, c);
}

void new_char_other() {
  pair_t pos;
  Character *c;

  do {
    rand_pos(pos);
  } while (world.rival_dist[pos[dim_y]][pos[dim_x]] == INT_MAX ||
           world.rival_dist[pos[dim_y]][pos[dim_x]] < 0 ||
           world.cur_map->cmap[pos[dim_y]][pos[dim_x]] || pos[dim_x] < 3 ||
           pos[dim_x] > MAP_X - 4 || pos[dim_y] < 3 || pos[dim_y] > MAP_Y - 4);

  world.cur_map->cmap[pos[dim_y]][pos[dim_x]] = c =
      (Character *)malloc(sizeof(*c));
  c->npc = (npc_t *)malloc(sizeof(*c->npc));
  c->pos[dim_y] = pos[dim_y];
  c->pos[dim_x] = pos[dim_x];
  c->npc->ctype = char_other;
  switch (rand() % 4) {
  case 0:
    c->npc->mtype = move_pace;
    c->symbol = 'p';
    break;
  case 1:
    c->npc->mtype = move_wander;
    c->symbol = 'w';
    break;
  case 2:
    c->npc->mtype = move_sentry;
    c->symbol = 's';
    break;
  case 3:
    c->npc->mtype = move_walk;
    c->symbol = 'n';
    break;
  }
  rand_dir(c->npc->dir);
  c->npc->defeated = 0;
  c->pc = NULL;
  c->next_turn = 0;
  heap_insert(&world.cur_map->turn, c);
}

void place_characters() {
  world.cur_map->num_trainers = 2;

  // Always place a hiker and a rival, then place a random number of others
  new_hiker();
  new_rival();
  do {
    // higher probability of non- hikers and rivals
    switch (rand() % 10) {
    case 0:
      new_hiker();
      break;
    case 1:
      new_rival();
      break;
    default:
      new_char_other();
      break;
    }
  } while (++world.cur_map->num_trainers < MIN_TRAINERS ||
           ((rand() % 100) < ADD_TRAINER_PROB));
}

void init_pc() {
  int x, y;

  do {
    x = rand() % (MAP_X - 2) + 1;
    y = rand() % (MAP_Y - 2) + 1;
  } while (world.cur_map->map[y][x] != ter_path);

  world.pc.pos[dim_x] = x;
  world.pc.pos[dim_y] = y;
  world.pc.symbol = '@';
  world.pc.pc = (pc_t *)malloc(sizeof(*world.pc.pc));
  world.pc.npc = NULL;

  world.cur_map->cmap[y][x] = &world.pc;
  world.pc.next_turn = 0;

  heap_insert(&world.cur_map->turn, &world.pc);
}

void place_pc() {
  Character *c;

  if (world.pc.pos[dim_x] == 1) {
    world.pc.pos[dim_x] = MAP_X - 2;
  } else if (world.pc.pos[dim_x] == MAP_X - 2) {
    world.pc.pos[dim_x] = 1;
  } else if (world.pc.pos[dim_y] == 1) {
    world.pc.pos[dim_y] = MAP_Y - 2;
  } else if (world.pc.pos[dim_y] == MAP_Y - 2) {
    world.pc.pos[dim_y] = 1;
  }

  world.cur_map->cmap[world.pc.pos[dim_y]][world.pc.pos[dim_x]] = &world.pc;

  if ((c = (Character *)heap_peek_min(&world.cur_map->turn))) {
    world.pc.next_turn = c->next_turn;
  } else {
    world.pc.next_turn = 0;
  }
}

// New map expects cur_idx to refer to the index to be generated.  If that
// map has already been generated then the only thing this does is set
// cur_map.
int new_map(int teleport) {
  int d, p;
  int e, w, n, s;
  int x, y;

  if (world.world[world.cur_idx[dim_y]][world.cur_idx[dim_x]]) {
    world.cur_map = world.world[world.cur_idx[dim_y]][world.cur_idx[dim_x]];
    place_pc();

    return 0;
  }

  world.cur_map = world.world[world.cur_idx[dim_y]][world.cur_idx[dim_x]] =
      (Map *)malloc(sizeof(*world.cur_map));

  smooth_height(world.cur_map);

  if (!world.cur_idx[dim_y]) {
    n = -1;
  } else if (world.world[world.cur_idx[dim_y] - 1][world.cur_idx[dim_x]]) {
    n = world.world[world.cur_idx[dim_y] - 1][world.cur_idx[dim_x]]->s;
  } else {
    n = 3 + rand() % (MAP_X - 6);
  }
  if (world.cur_idx[dim_y] == WORLD_SIZE - 1) {
    s = -1;
  } else if (world.world[world.cur_idx[dim_y] + 1][world.cur_idx[dim_x]]) {
    s = world.world[world.cur_idx[dim_y] + 1][world.cur_idx[dim_x]]->n;
  } else {
    s = 3 + rand() % (MAP_X - 6);
  }
  if (!world.cur_idx[dim_x]) {
    w = -1;
  } else if (world.world[world.cur_idx[dim_y]][world.cur_idx[dim_x] - 1]) {
    w = world.world[world.cur_idx[dim_y]][world.cur_idx[dim_x] - 1]->e;
  } else {
    w = 3 + rand() % (MAP_Y - 6);
  }
  if (world.cur_idx[dim_x] == WORLD_SIZE - 1) {
    e = -1;
  } else if (world.world[world.cur_idx[dim_y]][world.cur_idx[dim_x] + 1]) {
    e = world.world[world.cur_idx[dim_y]][world.cur_idx[dim_x] + 1]->w;
  } else {
    e = 3 + rand() % (MAP_Y - 6);
  }

  map_terrain(world.cur_map, n, s, e, w);

  place_boulders(world.cur_map);
  place_trees(world.cur_map);
  build_paths(world.cur_map);
  d = (abs(world.cur_idx[dim_x] - (WORLD_SIZE / 2)) +
       abs(world.cur_idx[dim_y] - (WORLD_SIZE / 2)));
  p = d > 200 ? 5 : (50 - ((45 * d) / 200));
  //  printf("d=%d, p=%d\n", d, p);
  if ((rand() % 100) < p || !d) {
    place_pokemart(world.cur_map);
  }
  if ((rand() % 100) < p || !d) {
    place_center(world.cur_map);
  }

  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      world.cur_map->cmap[y][x] = NULL;
    }
  }

  heap_init(&world.cur_map->turn, cmp_char_turns, delete_character);

  if ((world.cur_idx[dim_x] == WORLD_SIZE / 2) &&
      (world.cur_idx[dim_y] == WORLD_SIZE / 2)) {
    init_pc();
  } else {
    place_pc();
  }

  pathfind(world.cur_map);
  if (teleport) {
    do {
      world.cur_map->cmap[world.pc.pos[dim_y]][world.pc.pos[dim_x]] = NULL;
      world.pc.pos[dim_x] = rand_range(1, MAP_X - 2);
      world.pc.pos[dim_y] = rand_range(1, MAP_Y - 2);
    } while (world.cur_map->cmap[world.pc.pos[dim_y]][world.pc.pos[dim_x]] ||
             (move_cost[char_pc][world.cur_map->map[world.pc.pos[dim_y]]
                                                   [world.pc.pos[dim_x]]] ==
              INT_MAX) ||
             world.rival_dist[world.pc.pos[dim_y]][world.pc.pos[dim_x]] < 0);
    world.cur_map->cmap[world.pc.pos[dim_y]][world.pc.pos[dim_x]] = &world.pc;
    pathfind(world.cur_map);
  }

  place_characters();

  return 0;
}

// The world is global because of its size, so init_world is parameterless
void init_world() {
  world.quit = 0;
  world.cur_idx[dim_x] = world.cur_idx[dim_y] = WORLD_SIZE / 2;
  new_map(0);
}

void delete_world() {
  int x, y;

  // Only correct because current game never leaves the initial map
  // Need to iterate over all maps in 1.05+
  heap_delete(&world.cur_map->turn);

  for (y = 0; y < WORLD_SIZE; y++) {
    for (x = 0; x < WORLD_SIZE; x++) {
      if (world.world[y][x]) {
        free(world.world[y][x]);
        world.world[y][x] = NULL;
      }
    }
  }
}

void print_hiker_dist() {
  int x, y;

  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      if (world.hiker_dist[y][x] == INT_MAX) {
        printf("   ");
      } else {
        printf(" %5d", world.hiker_dist[y][x]);
      }
    }
    printf("\n");
  }
}

void print_rival_dist() {
  int x, y;

  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      if (world.rival_dist[y][x] == INT_MAX || world.rival_dist[y][x] < 0) {
        printf("   ");
      } else {
        printf(" %02d", world.rival_dist[y][x] % 100);
      }
    }
    printf("\n");
  }
}

void leave_map(pair_t d) {
  if (d[dim_x] == 0) {
    world.cur_idx[dim_x]--;
  } else if (d[dim_y] == 0) {
    world.cur_idx[dim_y]--;
  } else if (d[dim_x] == MAP_X - 1) {
    world.cur_idx[dim_x]++;
  } else {
    world.cur_idx[dim_y]++;
  }
  new_map(0);
}

void game_loop() {
  Character *c;
  pair_t d;

  while (!world.quit) {
    c = (Character *)heap_remove_min(&world.cur_map->turn);

    move_func[c->npc ? c->npc->mtype : move_pc](c, d);

    world.cur_map->cmap[c->pos[dim_y]][c->pos[dim_x]] = NULL;
    if (c->pc && (d[dim_x] == 0 || d[dim_x] == MAP_X - 1 || d[dim_y] == 0 ||
                  d[dim_y] == MAP_Y - 1)) {
      leave_map(d);
      d[dim_x] = c->pos[dim_x];
      d[dim_y] = c->pos[dim_y];
    }
    world.cur_map->cmap[d[dim_y]][d[dim_x]] = c;

    if (c->pc) {
      // Performance bug - pathfinding runs twice after generating a new map
      pathfind(world.cur_map);
    }

    c->next_turn += move_cost[c->npc ? c->npc->ctype : char_pc]
                             [world.cur_map->map[d[dim_y]][d[dim_x]]];

    c->pos[dim_y] = d[dim_y];
    c->pos[dim_x] = d[dim_x];

    heap_insert(&world.cur_map->turn, c);
  }
}

class Pokemon {
private:
  int id;
  string identifier;
  int species_id;
  int height;
  int weight;
  int base_experience;
  int order;
  int is_default;

public:
  Pokemon(int id, string identifier, int species_id, int height, int weight,
          int base_experience, int order, int is_default) {
    this->id = id ? id : -1;
    this->identifier = identifier;
    this->species_id = species_id ? species_id : -1;
    this->height = height ? height : -1;
    this->weight = weight ? weight : -1;
    this->base_experience = base_experience ? base_experience : -1;
    this->order = order ? order : -1;
    this->is_default = is_default ? is_default : -1;
  }

  Pokemon(Pokemon *&p) {
    this->id = p->id;
    this->identifier = p->identifier;
    this->species_id = p->species_id;
    this->height = p->height;
    this->weight = p->weight;
    this->base_experience = p->base_experience;
    this->order = p->order;
    this->is_default = p->is_default;
  }

  friend std::ostream &operator<<(std::ostream &os, const Pokemon &p) {
    if (p.id != -1)
      os << p.id;
    os << ",";
    os << p.identifier;
    os << ",";
    if (p.species_id != -1)
      os << p.species_id;
    os << ",";
    if (p.height != -1)
      os << p.height;
    os << ",";
    if (p.weight != -1)
      os << p.weight;
    os << ",";
    if (p.base_experience != -1)
      os << p.base_experience;
    os << ",";
    if (p.order != -1)
      os << p.order;
    os << ",";
    if (p.is_default != -1)
      os << p.is_default;
    return os;
  }
};

class Moves {
private:
  int id;
  string identifier;
  int generation_id;
  int type_id;
  int power;
  int pp;
  int accuracy;
  int priority;
  int target_id;
  int damage_class_id;
  int effect_id;
  int effect_chance;
  int contest_type_id;
  int contest_effect_id;
  int super_contest_effect_id;

public:
  Moves(int id, string identifier, int generation_id, int type_id, int power,
        int pp, int accuracy, int priority, int target_id, int damage_class_id,
        int effect_id, int effect_chance, int contest_type_id,
        int contest_effect_id, int super_contest_effect_id) {
    this->id = id ? id : -1;
    this->identifier = identifier;
    this->generation_id = generation_id ? generation_id : -1;
    this->type_id = type_id ? type_id : -1;
    this->power = power ? power : -1;
    this->pp = pp ? pp : -1;
    this->accuracy = accuracy ? accuracy : -1;
    this->priority = priority ? priority : -1;
    this->target_id = target_id ? target_id : -1;
    this->damage_class_id = damage_class_id ? damage_class_id : -1;
    this->effect_id = effect_id ? effect_id : -1;
    this->effect_chance = effect_chance ? effect_chance : -1;
    this->contest_type_id = contest_type_id ? contest_type_id : -1;
    this->contest_effect_id = contest_effect_id ? contest_effect_id : -1;
    this->super_contest_effect_id =
        super_contest_effect_id ? super_contest_effect_id : -1;
  }

  Moves(Moves *&m) {
    this->id = m->id;
    this->identifier = m->identifier;
    this->generation_id = m->generation_id;
    this->type_id = m->type_id;
    this->power = m->power;
    this->pp = m->pp;
    this->accuracy = m->accuracy;
    this->priority = m->priority;
    this->target_id = m->target_id;
    this->damage_class_id = m->damage_class_id;
    this->effect_id = m->effect_id;
    this->effect_chance = m->effect_chance;
    this->contest_type_id = m->contest_type_id;
    this->contest_effect_id = m->contest_effect_id;
    this->super_contest_effect_id = m->super_contest_effect_id;
  }

  friend std::ostream &operator<<(std::ostream &os, const Moves &m) {
    if (m.id != -1)
      os << m.id;
    os << ",";
    os << m.identifier;
    os << ",";
    if (m.generation_id != -1)
      os << m.generation_id;
    os << ",";
    if (m.type_id != -1)
      os << m.type_id;
    os << ",";
    if (m.power != -1)
      os << m.power;
    os << ",";
    if (m.pp != -1)
      os << m.pp;
    os << ",";
    if (m.accuracy != -1)
      os << m.accuracy;
    os << ",";
    if (m.priority != -1)
      os << m.priority;
    os << ",";
    if (m.target_id != -1)
      os << m.target_id;
    os << ",";
    if (m.damage_class_id != -1)
      os << m.damage_class_id;
    os << ",";
    if (m.effect_id != -1)
      os << m.effect_id;
    os << ",";
    if (m.effect_chance != -1)
      os << m.effect_chance;
    os << ",";
    if (m.contest_type_id != -1)
      os << m.contest_type_id;
    os << ",";
    if (m.contest_effect_id != -1)
      os << m.contest_effect_id;
    os << ",";
    if (m.super_contest_effect_id != -1)
      os << m.super_contest_effect_id;
    return os;
  }
};

class PokemonMoves {
private:
  int pokemon_id;
  int version_group_id;
  int move_id;
  int pokemon_move_method_id;
  int level;
  int order;

public:
  PokemonMoves(int pokemon_id, int version_group_id, int move_id,
               int pokemon_move_method_id, int level, int order) {
    this->pokemon_id = pokemon_id ? pokemon_id : -1;
    this->version_group_id = version_group_id ? version_group_id : -1;
    this->move_id = move_id ? move_id : -1;
    this->pokemon_move_method_id =
        pokemon_move_method_id ? pokemon_move_method_id : -1;
    this->level = level ? level : -1;
    this->order = order ? order : -1;
  }

  PokemonMoves(PokemonMoves *&pm) {
    this->pokemon_id = pm->pokemon_id;
    this->version_group_id = pm->version_group_id;
    this->move_id = pm->move_id;
    this->pokemon_move_method_id = pm->pokemon_move_method_id;
    this->level = pm->level;
    this->order = pm->order;
  }

  friend std::ostream &operator<<(std::ostream &os, const PokemonMoves &pm) {
    if (pm.pokemon_id != -1)
      os << pm.pokemon_id;
    os << ",";
    if (pm.version_group_id != -1)
      os << pm.version_group_id;
    os << ",";
    if (pm.move_id != -1)
      os << pm.move_id;
    os << ",";
    if (pm.pokemon_move_method_id != -1)
      os << pm.pokemon_move_method_id;
    os << ",";
    if (pm.level != -1)
      os << pm.level;
    os << ",";
    if (pm.order != -1)
      os << pm.order;
    return os;
  }
};

class PokemonSpecies {
private:
  int id;
  string identifier;
  int generation_id;
  int evolves_from_species_id;
  int evolution_chain_id;
  int color_id;
  int shape_id;
  int habitat_id;
  int gender_rate;
  int capture_rate;
  int base_happiness;
  int is_baby;
  int hatch_counter;
  int has_gender_differences;
  int growth_rate_id;
  int forms_switchable;
  int is_legendary;
  int is_mythical;
  int order;
  int conquest_order;

public:
  PokemonSpecies(int id, string identifier, int generation_id,
                 int evolves_from_species_id, int evolution_chain_id,
                 int color_id, int shape_id, int habitat_id, int gender_rate,
                 int capture_rate, int base_happiness, int is_baby,
                 int hatch_counter, int has_gender_differences,
                 int growth_rate_id, int forms_switchable, int is_legendary,
                 int is_mythical, int order, int conquest_order) {
    this->id = id ? id : -1;
    this->identifier = identifier;
    this->generation_id = generation_id ? generation_id : -1;
    this->evolves_from_species_id =
        evolves_from_species_id ? evolves_from_species_id : -1;
    this->evolution_chain_id = evolution_chain_id ? evolution_chain_id : -1;
    this->color_id = color_id ? color_id : -1;
    this->shape_id = shape_id ? shape_id : -1;
    this->habitat_id = habitat_id ? habitat_id : -1;
    this->gender_rate = gender_rate ? gender_rate : -1;
    this->capture_rate = capture_rate ? capture_rate : -1;
    this->base_happiness = base_happiness ? base_happiness : -1;
    this->is_baby = is_baby ? is_baby : -1;
    this->hatch_counter = hatch_counter ? hatch_counter : -1;
    this->has_gender_differences =
        has_gender_differences ? has_gender_differences : -1;
    this->growth_rate_id = growth_rate_id ? growth_rate_id : -1;
    this->forms_switchable = forms_switchable ? forms_switchable : -1;
    this->is_legendary = is_legendary ? is_legendary : -1;
    this->is_mythical = is_mythical ? is_mythical : -1;
    this->order = order ? order : -1;
    this->conquest_order = conquest_order ? conquest_order : -1;
  }

  PokemonSpecies(PokemonSpecies *&ps) {
    this->id = ps->id;
    this->identifier = ps->identifier;
    this->generation_id = ps->generation_id;
    this->evolves_from_species_id = ps->evolves_from_species_id;
    this->evolution_chain_id = ps->evolution_chain_id;
    this->color_id = ps->color_id;
    this->shape_id = ps->shape_id;
    this->habitat_id = ps->habitat_id;
    this->gender_rate = ps->gender_rate;
    this->capture_rate = ps->capture_rate;
    this->base_happiness = ps->base_happiness;
    this->is_baby = ps->is_baby;
    this->hatch_counter = ps->hatch_counter;
    this->has_gender_differences = ps->has_gender_differences;
    this->growth_rate_id = ps->growth_rate_id;
    this->forms_switchable = ps->forms_switchable;
    this->is_legendary = ps->is_legendary;
    this->is_mythical = ps->is_mythical;
    this->order = ps->order;
    this->conquest_order = ps->conquest_order;
  }

  friend std::ostream &operator<<(std::ostream &os, const PokemonSpecies &ps) {
    if (ps.id != -1)
      os << ps.id;
    os << ",";
    os << ps.identifier;
    os << ",";
    if (ps.generation_id != -1)
      os << ps.generation_id;
    os << ",";
    if (ps.evolves_from_species_id != -1)
      os << ps.evolves_from_species_id;
    os << ",";
    if (ps.evolution_chain_id != -1)
      os << ps.evolution_chain_id;
    os << ",";
    if (ps.color_id != -1)
      os << ps.color_id;
    os << ",";
    if (ps.shape_id != -1)
      os << ps.shape_id;
    os << ",";
    if (ps.habitat_id != -1)
      os << ps.habitat_id;
    os << ",";
    if (ps.gender_rate != -1)
      os << ps.gender_rate;
    os << ",";
    if (ps.capture_rate != -1)
      os << ps.capture_rate;
    os << ",";
    if (ps.base_happiness != -1)
      os << ps.base_happiness;
    os << ",";
    if (ps.is_baby != -1)
      os << ps.is_baby;
    os << ",";
    if (ps.hatch_counter != -1)
      os << ps.hatch_counter;
    os << ",";
    if (ps.has_gender_differences != -1)
      os << ps.has_gender_differences;
    os << ",";
    if (ps.growth_rate_id != -1)
      os << ps.growth_rate_id;
    os << ",";
    if (ps.forms_switchable != -1)
      os << ps.forms_switchable;
    os << ",";
    if (ps.is_legendary != -1)
      os << ps.is_legendary;
    os << ",";
    if (ps.is_mythical != -1)
      os << ps.is_mythical;
    os << ",";
    if (ps.order != -1)
      os << ps.order;
    os << ",";
    if (ps.conquest_order != -1)
      os << ps.conquest_order;
    return os;
  }
};

class Experience {
private:
  int growth_rate_id;
  int level;
  int experience;

public:
  Experience(int growth_rate_id, int level, int experience) {
    this->growth_rate_id = growth_rate_id ? growth_rate_id : -1;
    this->level = level ? level : -1;
    this->experience = experience ? experience : -1;
  }

  Experience(Experience *&e) {
    this->growth_rate_id = e->growth_rate_id;
    this->level = e->level;
    this->experience = e->experience;
  }

  friend std::ostream &operator<<(std::ostream &os, const Experience &e) {
    if (e.growth_rate_id != -1)
      os << e.growth_rate_id;
    os << ",";
    if (e.level != -1)
      os << e.level;
    os << ",";
    if (e.experience != -1)
      os << e.experience;
    return os;
  }
};

class TypeNames {
private:
  int type_id;
  int local_language;
  string name;

public:
  TypeNames(int type_id, int local_language, string name) {
    this->type_id = type_id ? type_id : -1;
    this->local_language = local_language ? local_language : -1;
    this->name = name;
  }

  TypeNames(TypeNames *&tn) {
    this->type_id = tn->type_id;
    this->local_language = tn->local_language;
    this->name = tn->name;
  }

  friend std::ostream &operator<<(std::ostream &os, const TypeNames &tn) {
    if (tn.type_id != -1)
      os << tn.type_id;
    os << ",";
    if (tn.local_language != -1)
      os << tn.local_language;
    os << ",";
    os << tn.name;
    return os;
  }
};

class Database {
private:
public:
  list<Pokemon> *pokemon;
  list<Moves> *moves;
  list<PokemonMoves> *pokemonMoves;
  list<PokemonSpecies> *pokemonSpecies;
  list<Experience> *experience;
  list<TypeNames> *typeNames;

  Database() {
    pokemon = new list<Pokemon>();
    moves = new list<Moves>();
    pokemonMoves = new list<PokemonMoves>();
    pokemonSpecies = new list<PokemonSpecies>();
    experience = new list<Experience>();
    typeNames = new list<TypeNames>();
  }

  ~Database() {
    // TODO: deallocate all lists
  }

  void printPokemon() {
    list<Pokemon>::iterator itr;
    for (itr = pokemon->begin(); itr != pokemon->end(); ++itr)
      cout << *itr << endl;
  }

  void printMoves() {
    list<Moves>::iterator itr;
    for (itr = moves->begin(); itr != moves->end(); ++itr)
      cout << *itr << endl;
  }

  void printPokemonMoves() {
    list<PokemonMoves>::iterator itr;
    for (itr = pokemonMoves->begin(); itr != pokemonMoves->end(); ++itr)
      cout << *itr << endl;
  }

  void printPokemonSpecies() {
    list<PokemonSpecies>::iterator itr;
    for (itr = pokemonSpecies->begin(); itr != pokemonSpecies->end(); ++itr)
      cout << *itr << endl;
  }

  void printExperience() {
    list<Experience>::iterator itr;
    for (itr = experience->begin(); itr != experience->end(); ++itr)
      cout << *itr << endl;
  }

  void printTypeNames() {
    list<TypeNames>::iterator itr;
    for (itr = typeNames->begin(); itr != typeNames->end(); ++itr)
      cout << *itr << endl;
  }
};

class CsvParser {
private:
  Database *db;

  string getPath(int attempt, string filename) {
    switch (attempt) {
    case 0:
      cout << "Looking in: ";
      return "/share/cs327/pokedex/pokedex/data/csv/" + filename + ".csv";
    case 1:
      string home_dir(getenv("HOME"));
      cout << "Looking in: ";
      if (home_dir.empty())
        break;
      return home_dir + "/.poke327/pokedex/pokedex/data/csv/" + filename +
             ".csv";
    }

    return "";
  }

  int addPokemon(char *str) {
    Pokemon *p;
    int id;
    string identifier;
    int species_id;
    int height;
    int weight;
    int base_experience;
    int order;
    int is_default;

    vector<string> tokens;
    stringstream check(str);
    string tmp;
    while (getline(check, tmp, ','))
      tokens.push_back(tmp);

    id = stoi(tokens[0]);
    identifier = tokens[1];
    species_id = stoi(tokens[2]);
    height = stoi(tokens[3]);
    weight = stoi(tokens[4]);
    base_experience = stoi(tokens[5]);
    order = stoi(tokens[6]);
    is_default = stoi(tokens[7]);

    p = new Pokemon(id, identifier, species_id, height, weight, base_experience,
                    order, is_default);

    db->pokemon->emplace_back(p);

    return 0;
  }

  int addMoves(char *str) {
    Moves *m;

    int id;
    string identifier;
    int generation_id;
    int type_id;
    int power;
    int pp;
    int accuracy;
    int priority;
    int target_id;
    int damage_class_id;
    int effect_id;
    int effect_chance;
    int contest_type_id;
    int contest_effect_id;
    int super_contest_effect_id;

    vector<string> tokens;
    stringstream check(str);
    string tmp;
    while (getline(check, tmp, ','))
      tokens.push_back(tmp);

    id = stoi(tokens[0]);
    identifier = tokens[1];
    generation_id = stoi(tokens[2]);
    type_id = stoi(tokens[3]);
    power = stoi(tokens[4]);
    pp = stoi(tokens[5]);
    accuracy = stoi(tokens[6]);
    priority = stoi(tokens[7]);
    target_id = stoi(tokens[8]);
    damage_class_id = stoi(tokens[9]);
    effect_id = stoi(tokens[10]);
    effect_chance = stoi(tokens[11]);
    contest_type_id = stoi(tokens[12]);
    contest_effect_id = stoi(tokens[13]);
    super_contest_effect_id = stoi(tokens[14]);

    m = new Moves(id, identifier, generation_id, type_id, power, pp, accuracy,
                  priority, target_id, damage_class_id, effect_id,
                  effect_chance, contest_type_id, contest_effect_id,
                  super_contest_effect_id);

    db->moves->emplace_back(m);

    return 0;
  }

  int addPokemonMoves(char *str) {
    PokemonMoves *pm;

    int pokemon_id;
    int version_group_id;
    int move_id;
    int pokemon_move_method_id;
    int level;
    int order;

    vector<string> tokens;
    stringstream check(str);
    string tmp;
    while (getline(check, tmp, ','))
      tokens.push_back(tmp);

    pokemon_id = stoi(tokens[0]);
    version_group_id = stoi(tokens[1]);
    move_id = stoi(tokens[2]);
    pokemon_move_method_id = stoi(tokens[3]);
    level = stoi(tokens[4]);
    order = stoi(tokens[5]);

    pm = new PokemonMoves(pokemon_id, version_group_id, move_id,
                          pokemon_move_method_id, level, order);

    db->pokemonMoves->emplace_back(pm);

    return 0;
  }

  int addPokemonSpecies(char *str) {
    PokemonSpecies *ps;

    int id;
    string identifier;
    int generation_id;
    int evolves_from_species_id;
    int evolution_chain_id;
    int color_id;
    int shape_id;
    int habitat_id;
    int gender_rate;
    int capture_rate;
    int base_happiness;
    int is_baby;
    int hatch_counter;
    int has_gender_differences;
    int growth_rate_id;
    int forms_switchable;
    int is_legendary;
    int is_mythical;
    int order;
    int conquest_order;

    vector<string> tokens;
    stringstream check(str);
    string tmp;
    while (getline(check, tmp, ','))
      tokens.push_back(tmp);

    id = stoi(tokens[0]);
    identifier = tokens[1];
    generation_id = stoi(tokens[2]);
    evolves_from_species_id = stoi(tokens[3]);
    evolution_chain_id = stoi(tokens[4]);
    color_id = stoi(tokens[5]);
    shape_id = stoi(tokens[6]);
    habitat_id = stoi(tokens[7]);
    gender_rate = stoi(tokens[8]);
    capture_rate = stoi(tokens[9]);
    base_happiness = stoi(tokens[10]);
    is_baby = stoi(tokens[11]);
    hatch_counter = stoi(tokens[12]);
    has_gender_differences = stoi(tokens[13]);
    growth_rate_id = stoi(tokens[14]);
    forms_switchable = stoi(tokens[15]);
    is_legendary = stoi(tokens[16]);
    is_mythical = stoi(tokens[17]);
    order = stoi(tokens[18]);
    conquest_order = stoi(tokens[19]);

    ps = new PokemonSpecies(
        id, identifier, generation_id, evolves_from_species_id,
        evolution_chain_id, color_id, shape_id, habitat_id, gender_rate,
        capture_rate, base_happiness, is_baby, hatch_counter,
        has_gender_differences, growth_rate_id, forms_switchable, is_legendary,
        is_mythical, order, conquest_order);

    db->pokemonSpecies->emplace_back(ps);

    return 0;
  }

  int addExperience(char *str) {
    Experience *e;

    int growth_rate_id;
    int level;
    int experience;

    vector<string> tokens;
    stringstream check(str);
    string tmp;
    while (getline(check, tmp, ','))
      tokens.push_back(tmp);

    growth_rate_id = stoi(tokens[0]);
    level = stoi(tokens[1]);
    experience = stoi(tokens[2]);

    e = new Experience(growth_rate_id, level, experience);

    db->experience->emplace_back(e);

    return 0;
  }

  int addTypeNames(char *str) {
    TypeNames *tn;

    int type_id;
    int local_language;
    string name;

    vector<string> tokens;
    stringstream check(str);
    string tmp;
    while (getline(check, tmp, ','))
      tokens.push_back(tmp);

    type_id = stoi(tokens[0]);
    local_language = stoi(tokens[1]);
    name = tokens[2];

    tn = new TypeNames(type_id, local_language, name);

    db->typeNames->emplace_back(tn);

    return 0;
  }

public:
  CsvParser() { db = new Database(); }
  ~CsvParser() { delete db; }

  int parse(const char *filename) {
    int i;
    ifstream file;
    string path;
    char line[400];

    for (i = 0; !file.is_open(); i++) {
      path = getPath(i, filename);
      cout << path << endl;

      if (path.empty()) {
        cout << "Unable to locate: \"" << filename << ".csv\"" << endl;
        cout << "\tTry one of these: pokemon, moves, pokemon_moves, "
                "pokemon_species, experience, or type_names"
             << endl;
        return 1;
      }

      file.open(path);

      if (file.is_open()) {
        file.getline(line, 400, '\n');
        // cout << "Headers:" << endl;
        if (strlen(line) < 1) {
          file.close();
        }
      }
    }

    if (strcmp(filename, "pokemon") == 0) {
      while (file.getline(line, 400, '\n'))
        addPokemon(line);
      db->printPokemon();

    } else if (strcmp(filename, "moves") == 0) {
      while (file.getline(line, 400, '\n'))
        addMoves(line);
      db->printMoves();

    } else if (strcmp(filename, "pokemon_moves") == 0) {
      while (file.getline(line, 400, '\n'))
        addPokemonMoves(line);
      db->printPokemonMoves();

    } else if (strcmp(filename, "pokemon_species") == 0) {
      while (file.getline(line, 400, '\n'))
        addPokemonSpecies(line);
      db->printPokemonSpecies();

    } else if (strcmp(filename, "experience") == 0) {
      while (file.getline(line, 400, '\n'))
        addExperience(line);
      db->printExperience();
    } else if (strcmp(filename, "type_names") == 0) {
      while (file.getline(line, 400, '\n'))
        addTypeNames(line);
      db->printTypeNames();
    } else {
      cout << "\tTry one of these: pokemon, moves, pokemon_moves, "
              "pokemon_species, experience, or type_names"
           << endl;
    }

    file.close();
    return 0;
  }
};

int main(int argc, char *argv[]) {
  // struct timeval tv;
  // uint32_t seed;
  // //  char c;
  // //  int x, y;

  // if (argc == 2) {
  //   seed = atoi(argv[1]);
  // } else {
  //   gettimeofday(&tv, NULL);
  //   seed = (tv.tv_usec ^ (tv.tv_sec << 20)) & 0xffffffff;
  // }

  // printf("Using seed: %u\n", seed);
  // srand(seed);

  // io_init_terminal();

  // init_world();

  // game_loop();

  // delete_world();

  // io_reset_terminal();

  if (argc < 2)
    return 1;

  CsvParser *parser = new CsvParser();

  parser->parse(argv[1]);

  delete parser;

  return 0;
}
