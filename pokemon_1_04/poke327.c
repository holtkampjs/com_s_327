#include <assert.h>
#include <limits.h>
#include <ncurses.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

#include "heap.h"

#define COLOR_GREY 9
#define COLOR_MUSTARD 10
#define COLOR_LIGHT_GREY 11
#define COLOR_LIGHT_BROWN 12
#define COLOR_BROWN 13
#define COLOR_LIGHTER_BROWN 14
#define COLOR_LIGHT_GREEN 15

#define malloc(size)                                                           \
  ({                                                                           \
    void *_tmp;                                                                \
    assert((_tmp = malloc(size)));                                             \
    _tmp;                                                                      \
  })

typedef struct path {
  heap_node_t *hn;
  uint8_t pos[2];
  uint8_t from[2];
  int32_t cost;
} path_t;

typedef enum dim { dim_x, dim_y, num_dims } dim_t;

typedef int16_t pair_t[num_dims];

#define MAP_X 80
#define MAP_Y 21
#define MIN_TREES 10
#define MIN_BOULDERS 10
#define TREE_PROB 95
#define BOULDER_PROB 95
#define WORLD_SIZE 401

#define mappair(pair) (m->map[pair[dim_y]][pair[dim_x]])
#define mapxy(x, y) (m->map[y][x])
#define heightpair(pair) (m->height[pair[dim_y]][pair[dim_x]])
#define heightxy(x, y) (m->height[y][x])

typedef enum __attribute__((__packed__)) dir_type {
  dir_north,
  dir_south,
  dir_east,
  dir_west,
  dir_north_west,
  dir_south_west,
  dir_north_east,
  dir_south_east,
  num_dir_type,
} dir_type_t;

typedef enum __attribute__((__packed__)) color_type {
  color_dummy,
  color_pc,
  color_boulder,
  color_tree,
  color_path,
  color_mart,
  color_center,
  color_grass,
  color_clearing,
  color_mountain,
  color_forrest,
  color_hiker,
  color_rival,
  color_pacer,
  color_wanderer,
  color_stationary,
  color_random_walker,
  num_colors,
} color_type_t;

typedef enum __attribute__((__packed__)) terrain_type {
  ter_boulder,
  ter_tree,
  ter_path,
  ter_mart,
  ter_center,
  ter_grass,
  ter_clearing,
  ter_mountain,
  ter_forest,
  num_terrain_types
} terrain_type_t;

typedef enum __attribute__((__packed__)) character_type {
  char_pc,
  char_hiker,
  char_rival,
  char_other,
  num_character_types
} character_type_t;

typedef enum __attribute__((__packed__)) npc_type {
  npc_hiker,
  npc_rival,
  npc_pacer,
  npc_wanderer,
  npc_stationary,
  npc_random_walker,
  num_npc_type
} npc_type_t;

typedef struct pc {
  pair_t pos;
} pc_t;

typedef struct npc {
  int type;
  pair_t pos;
  int stat;
  heap_node_t *hn;
  int dir;
} npc_t;

typedef struct map {
  terrain_type_t map[MAP_Y][MAP_X];
  uint8_t height[MAP_Y][MAP_X];
  int8_t n, s, e, w;
  npc_t *npcs;
  int hiker_dist[MAP_Y][MAP_X];
  int rival_dist[MAP_Y][MAP_X];
  heap_t npc_heap;
} map_t;

typedef struct queue_node {
  int x, y;
  struct queue_node *next;
} queue_node_t;

typedef struct world {
  map_t *world[WORLD_SIZE][WORLD_SIZE];
  pair_t cur_idx;
  map_t *cur_map;
  /* Please distance maps in world, not map, since *
   * we only need one pair at any given time.      */
  pc_t pc;
  int num_trainers;
} world_t;

/* Even unallocated, a WORLD_SIZE x WORLD_SIZE array of pointers is a very *
 * large thing to put on the stack.  To avoid that, world is a global.     */
world_t world;

static int32_t move_cost[num_character_types][num_terrain_types] = {
    {INT_MAX, INT_MAX, 10, 10, 10, 20, 10, INT_MAX, INT_MAX},
    {INT_MAX, INT_MAX, 10, INT_MAX, INT_MAX, 15, 10, 15, 15},
    {INT_MAX, INT_MAX, 10, INT_MAX, INT_MAX, 20, 10, INT_MAX, INT_MAX},
    {INT_MAX, INT_MAX, 10, INT_MAX, INT_MAX, 20, 10, INT_MAX, INT_MAX},
};

static int32_t path_cmp(const void *key, const void *with) {
  return ((path_t *)key)->cost - ((path_t *)with)->cost;
}

static int32_t edge_penalty(int8_t x, int8_t y) {
  return (x == 1 || y == 1 || x == MAP_X - 2 || y == MAP_Y - 2) ? 2 : 1;
}

static void dijkstra_path(map_t *m, pair_t from, pair_t to) {
  static path_t path[MAP_Y][MAP_X], *p;
  static uint32_t initialized = 0;
  heap_t h;
  uint32_t x, y;

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

  while ((p = heap_remove_min(&h))) {
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

static int build_paths(map_t *m) {
  pair_t from, to;

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

static int smooth_height(map_t *m) {
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
      head = tail = malloc(sizeof(*tail));
    } else {
      tail->next = malloc(sizeof(*tail));
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
    i = height[y][x];

    if (x - 1 >= 0 && y - 1 >= 0 && !height[y - 1][x - 1]) {
      height[y - 1][x - 1] = i;
      tail->next = malloc(sizeof(*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x - 1;
      tail->y = y - 1;
    }
    if (x - 1 >= 0 && !height[y][x - 1]) {
      height[y][x - 1] = i;
      tail->next = malloc(sizeof(*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x - 1;
      tail->y = y;
    }
    if (x - 1 >= 0 && y + 1 < MAP_Y && !height[y + 1][x - 1]) {
      height[y + 1][x - 1] = i;
      tail->next = malloc(sizeof(*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x - 1;
      tail->y = y + 1;
    }
    if (y - 1 >= 0 && !height[y - 1][x]) {
      height[y - 1][x] = i;
      tail->next = malloc(sizeof(*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x;
      tail->y = y - 1;
    }
    if (y + 1 < MAP_Y && !height[y + 1][x]) {
      height[y + 1][x] = i;
      tail->next = malloc(sizeof(*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x;
      tail->y = y + 1;
    }
    if (x + 1 < MAP_X && y - 1 >= 0 && !height[y - 1][x + 1]) {
      height[y - 1][x + 1] = i;
      tail->next = malloc(sizeof(*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x + 1;
      tail->y = y - 1;
    }
    if (x + 1 < MAP_X && !height[y][x + 1]) {
      height[y][x + 1] = i;
      tail->next = malloc(sizeof(*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x + 1;
      tail->y = y;
    }
    if (x + 1 < MAP_X && y + 1 < MAP_Y && !height[y + 1][x + 1]) {
      height[y + 1][x + 1] = i;
      tail->next = malloc(sizeof(*tail));
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

static void find_building_location(map_t *m, pair_t p) {
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

static int place_pokemart(map_t *m) {
  pair_t p;

  find_building_location(m, p);

  mapxy(p[dim_x], p[dim_y]) = ter_mart;
  mapxy(p[dim_x] + 1, p[dim_y]) = ter_mart;
  mapxy(p[dim_x], p[dim_y] + 1) = ter_mart;
  mapxy(p[dim_x] + 1, p[dim_y] + 1) = ter_mart;

  return 0;
}

static int place_center(map_t *m) {
  pair_t p;

  find_building_location(m, p);

  mapxy(p[dim_x], p[dim_y]) = ter_center;
  mapxy(p[dim_x] + 1, p[dim_y]) = ter_center;
  mapxy(p[dim_x], p[dim_y] + 1) = ter_center;
  mapxy(p[dim_x] + 1, p[dim_y] + 1) = ter_center;

  return 0;
}

static int map_terrain(map_t *m, int8_t n, int8_t s, int8_t e, int8_t w) {
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
      head = tail = malloc(sizeof(*tail));
    } else {
      tail->next = malloc(sizeof(*tail));
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
        m->map[y][x - 1] = i;
        tail->next = malloc(sizeof(*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x - 1;
        tail->y = y;
      } else if (!added_current) {
        added_current = 1;
        m->map[y][x] = i;
        tail->next = malloc(sizeof(*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x;
        tail->y = y;
      }
    }

    if (y - 1 >= 0 && !m->map[y - 1][x]) {
      if ((rand() % 100) < 20) {
        m->map[y - 1][x] = i;
        tail->next = malloc(sizeof(*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x;
        tail->y = y - 1;
      } else if (!added_current) {
        added_current = 1;
        m->map[y][x] = i;
        tail->next = malloc(sizeof(*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x;
        tail->y = y;
      }
    }

    if (y + 1 < MAP_Y && !m->map[y + 1][x]) {
      if ((rand() % 100) < 20) {
        m->map[y + 1][x] = i;
        tail->next = malloc(sizeof(*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x;
        tail->y = y + 1;
      } else if (!added_current) {
        added_current = 1;
        m->map[y][x] = i;
        tail->next = malloc(sizeof(*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x;
        tail->y = y;
      }
    }

    if (x + 1 < MAP_X && !m->map[y][x + 1]) {
      if ((rand() % 100) < 80) {
        m->map[y][x + 1] = i;
        tail->next = malloc(sizeof(*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x + 1;
        tail->y = y;
      } else if (!added_current) {
        added_current = 1;
        m->map[y][x] = i;
        tail->next = malloc(sizeof(*tail));
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
    mapxy(n, 0) = ter_path;
    mapxy(n, 1) = ter_path;
  }
  if (s != -1) {
    mapxy(s, MAP_Y - 1) = ter_path;
    mapxy(s, MAP_Y - 2) = ter_path;
  }
  if (w != -1) {
    mapxy(0, w) = ter_path;
    mapxy(1, w) = ter_path;
  }
  if (e != -1) {
    mapxy(MAP_X - 1, e) = ter_path;
    mapxy(MAP_X - 2, e) = ter_path;
  }

  return 0;
}

static int place_boulders(map_t *m) {
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

static int place_trees(map_t *m) {
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

int is_move_invalid(int x, int y, int npc_count, int character_type) {
  if (x < 1 || x > MAP_X - 1 || y < 1 || y > MAP_Y - 1)
    return 1;

  // Check terrain is valid
  if (move_cost[character_type][world.cur_map->map[y][x]] == INT_MAX)
    return 1;

  // Check for pc
  if (character_type != char_pc && x == world.pc.pos[dim_x] &&
      y == world.pc.pos[dim_y])
    return 1;

  // Check for other npcs
  while (npc_count) {
    if (x == world.cur_map->npcs[npc_count - 1].pos[dim_x] &&
        y == world.cur_map->npcs[npc_count - 1].pos[dim_y])
      return 1;
    npc_count--;
  }

  return 0;
}

int npc_cmp(const void *key, const void *with) {
  return ((npc_t *)key)->stat - ((npc_t *)with)->stat;
}

void spawn_npcs() {
  int i;
  int x, y;

  world.cur_map->npcs =
      malloc(world.num_trainers * sizeof(*world.cur_map->npcs));

  heap_init(&world.cur_map->npc_heap, npc_cmp, NULL);

  for (i = 0; i < world.num_trainers; i++) {
    world.cur_map->npcs[i].type = i % num_npc_type;

    switch (i % num_npc_type) {
    case npc_hiker:
      do {
        x = rand() % (MAP_X - 2) + 1;
        y = rand() % (MAP_Y - 2) + 1;
      } while (is_move_invalid(x, y, i, char_hiker));
      break;
    case npc_rival:
      do {
        x = rand() % (MAP_X - 2) + 1;
        y = rand() % (MAP_Y - 2) + 1;
      } while (is_move_invalid(x, y, i, char_rival));
      break;
    default:
      do {
        x = rand() % (MAP_X - 2) + 1;
        y = rand() % (MAP_Y - 2) + 1;
      } while (is_move_invalid(x, y, i, char_other));
      break;
    }

    world.cur_map->npcs[i].pos[dim_x] = x;
    world.cur_map->npcs[i].pos[dim_y] = y;
    world.cur_map->npcs[i].stat = 0;
    world.cur_map->npcs[i].dir = rand() % num_dir_type;
    world.cur_map->npcs[i].hn =
        heap_insert(&world.cur_map->npc_heap, &world.cur_map->npcs[i]);
  }
}

// New map expects cur_idx to refer to the index to be generated.  If that
// map has already been generated then the only thing this does is set
// cur_map.
static int new_map() {
  int d, p;
  int e, w, n, s;

  if (world.world[world.cur_idx[dim_y]][world.cur_idx[dim_x]]) {
    world.cur_map = world.world[world.cur_idx[dim_y]][world.cur_idx[dim_x]];
    return 0;
  }

  world.cur_map = world.world[world.cur_idx[dim_y]][world.cur_idx[dim_x]] =
      malloc(sizeof(*world.cur_map));

  smooth_height(world.cur_map);

  if (!world.cur_idx[dim_y]) {
    n = -1;
  } else if (world.world[world.cur_idx[dim_y] - 1][world.cur_idx[dim_x]]) {
    n = world.world[world.cur_idx[dim_y] - 1][world.cur_idx[dim_x]]->s;
  } else {
    n = 1 + rand() % (MAP_X - 2);
  }
  if (world.cur_idx[dim_y] == WORLD_SIZE - 1) {
    s = -1;
  } else if (world.world[world.cur_idx[dim_y] + 1][world.cur_idx[dim_x]]) {
    s = world.world[world.cur_idx[dim_y] + 1][world.cur_idx[dim_x]]->n;
  } else {
    s = 1 + rand() % (MAP_X - 2);
  }
  if (!world.cur_idx[dim_x]) {
    w = -1;
  } else if (world.world[world.cur_idx[dim_y]][world.cur_idx[dim_x] - 1]) {
    w = world.world[world.cur_idx[dim_y]][world.cur_idx[dim_x] - 1]->e;
  } else {
    w = 1 + rand() % (MAP_Y - 2);
  }
  if (world.cur_idx[dim_x] == WORLD_SIZE - 1) {
    e = -1;
  } else if (world.world[world.cur_idx[dim_y]][world.cur_idx[dim_x] + 1]) {
    e = world.world[world.cur_idx[dim_y]][world.cur_idx[dim_x] + 1]->w;
  } else {
    e = 1 + rand() % (MAP_Y - 2);
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

  spawn_npcs();

  return 0;
}

static void print_map() {
  int x, y;
  int default_reached = 0;

  for (y = 0; y < MAP_Y; y++) {
    move(3 + y, 0);
    for (x = 0; x < MAP_X; x++) {
      switch (world.cur_map->map[y][x]) {
      case ter_boulder:
      case ter_mountain:
        attron(COLOR_PAIR(color_boulder));
        addch('%');
        break;
      case ter_tree:
      case ter_forest:
        attron(COLOR_PAIR(color_tree));
        addch('^');
        break;
      case ter_path:
        attron(COLOR_PAIR(color_path));
        addch('#');
        break;
      case ter_mart:
        attron(COLOR_PAIR(color_mart));
        addch('M');
        break;
      case ter_center:
        attron(COLOR_PAIR(color_center));
        addch('C');
        break;
      case ter_grass:
        attron(COLOR_PAIR(color_grass));
        addch(':');
        break;
      case ter_clearing:
        attron(COLOR_PAIR(color_clearing));
        addch('.');
        break;
      default:
        default_reached = 1;
        break;
      }
      attrset(A_NORMAL);
    }
  }

  if (default_reached) {
    fprintf(stderr, "Default reached in %s\n", __FUNCTION__);
  }
}

void print_characters() {
  int i;

  attron(COLOR_PAIR(color_pc) | A_BLINK);
  mvaddch(world.pc.pos[dim_y] + 3, world.pc.pos[dim_x], '@');
  attrset(A_NORMAL);

  for (i = 0; i < world.num_trainers; i++) {
    switch (world.cur_map->npcs[i].type) {
    case npc_hiker:
      mvaddch(world.cur_map->npcs[i].pos[dim_y] + 3,
              world.cur_map->npcs[i].pos[dim_x], 'h');
      break;
    case npc_rival:
      mvaddch(world.cur_map->npcs[i].pos[dim_y] + 3,
              world.cur_map->npcs[i].pos[dim_x], 'r');
      break;
    case npc_pacer:
      mvaddch(world.cur_map->npcs[i].pos[dim_y] + 3,
              world.cur_map->npcs[i].pos[dim_x], 'p');
      break;
    case npc_wanderer:
      mvaddch(world.cur_map->npcs[i].pos[dim_y] + 3,
              world.cur_map->npcs[i].pos[dim_x], 'w');
      break;
    case npc_stationary:
      mvaddch(world.cur_map->npcs[i].pos[dim_y] + 3,
              world.cur_map->npcs[i].pos[dim_x], 's');
      break;
    case npc_random_walker:
      mvaddch(world.cur_map->npcs[i].pos[dim_y] + 3,
              world.cur_map->npcs[i].pos[dim_x], 'n');
      break;
    }
  }
}

// The world is global because of its size, so init_world is parameterless
void init_world() {
  world.cur_idx[dim_x] = world.cur_idx[dim_y] = WORLD_SIZE / 2;
  new_map();
}

void delete_world() {
  int x, y;

  for (y = 0; y < WORLD_SIZE; y++) {
    for (x = 0; x < WORLD_SIZE; x++) {
      if (world.world[y][x]) {
        free(world.world[y][x]->npcs);
        free(world.world[y][x]);
        world.world[y][x] = NULL;
      }
    }
  }
}

#define ter_cost(x, y, c) move_cost[c][m->map[y][x]]

static int32_t hiker_cmp(const void *key, const void *with) {
  return (world.cur_map->hiker_dist[((path_t *)key)->pos[dim_y]]
                                   [((path_t *)key)->pos[dim_x]] -
          world.cur_map->hiker_dist[((path_t *)with)->pos[dim_y]]
                                   [((path_t *)with)->pos[dim_x]]);
}

static int32_t rival_cmp(const void *key, const void *with) {
  return (world.cur_map->rival_dist[((path_t *)key)->pos[dim_y]]
                                   [((path_t *)key)->pos[dim_x]] -
          world.cur_map->rival_dist[((path_t *)with)->pos[dim_y]]
                                   [((path_t *)with)->pos[dim_x]]);
}

void pathfind(map_t *m) {
  heap_t h;
  uint32_t x, y;
  static path_t p[MAP_Y][MAP_X], *c;
  static uint32_t initialized = 0;

  if (!initialized) {
    initialized = 1;
    for (y = 0; y < MAP_Y; y++) {
      for (x = 0; x < MAP_X; x++) {
        p[y][x].pos[dim_y] = y;
        p[y][x].pos[dim_x] = x;
      }
    }
  }

  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      world.cur_map->hiker_dist[y][x] = world.cur_map->rival_dist[y][x] =
          INT_MAX;
    }
  }
  world.cur_map->hiker_dist[world.pc.pos[dim_y]][world.pc.pos[dim_x]] =
      world.cur_map->rival_dist[world.pc.pos[dim_y]][world.pc.pos[dim_x]] = 0;

  heap_init(&h, hiker_cmp, NULL);

  for (y = 1; y < MAP_Y - 1; y++) {
    for (x = 1; x < MAP_X - 1; x++) {
      if (ter_cost(x, y, char_hiker) != INT_MAX) {
        p[y][x].hn = heap_insert(&h, &p[y][x]);
      } else {
        p[y][x].hn = NULL;
      }
    }
  }

  while ((c = heap_remove_min(&h))) {
    c->hn = NULL;
    if ((p[c->pos[dim_y] - 1][c->pos[dim_x] - 1].hn) &&
        (world.cur_map->hiker_dist[c->pos[dim_y] - 1][c->pos[dim_x] - 1] >
         world.cur_map->hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
             ter_cost(c->pos[dim_x], c->pos[dim_y], char_hiker))) {
      world.cur_map->hiker_dist[c->pos[dim_y] - 1][c->pos[dim_x] - 1] =
          world.cur_map->hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
          ter_cost(c->pos[dim_x], c->pos[dim_y], char_hiker);
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y] - 1][c->pos[dim_x] - 1].hn);
    }
    if ((p[c->pos[dim_y] - 1][c->pos[dim_x]].hn) &&
        (world.cur_map->hiker_dist[c->pos[dim_y] - 1][c->pos[dim_x]] >
         world.cur_map->hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
             ter_cost(c->pos[dim_x], c->pos[dim_y], char_hiker))) {
      world.cur_map->hiker_dist[c->pos[dim_y] - 1][c->pos[dim_x]] =
          world.cur_map->hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
          ter_cost(c->pos[dim_x], c->pos[dim_y], char_hiker);
      heap_decrease_key_no_replace(&h, p[c->pos[dim_y] - 1][c->pos[dim_x]].hn);
    }
    if ((p[c->pos[dim_y] - 1][c->pos[dim_x] + 1].hn) &&
        (world.cur_map->hiker_dist[c->pos[dim_y] - 1][c->pos[dim_x] + 1] >
         world.cur_map->hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
             ter_cost(c->pos[dim_x], c->pos[dim_y], char_hiker))) {
      world.cur_map->hiker_dist[c->pos[dim_y] - 1][c->pos[dim_x] + 1] =
          world.cur_map->hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
          ter_cost(c->pos[dim_x], c->pos[dim_y], char_hiker);
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y] - 1][c->pos[dim_x] + 1].hn);
    }
    if ((p[c->pos[dim_y]][c->pos[dim_x] - 1].hn) &&
        (world.cur_map->hiker_dist[c->pos[dim_y]][c->pos[dim_x] - 1] >
         world.cur_map->hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
             ter_cost(c->pos[dim_x], c->pos[dim_y], char_hiker))) {
      world.cur_map->hiker_dist[c->pos[dim_y]][c->pos[dim_x] - 1] =
          world.cur_map->hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
          ter_cost(c->pos[dim_x], c->pos[dim_y], char_hiker);
      heap_decrease_key_no_replace(&h, p[c->pos[dim_y]][c->pos[dim_x] - 1].hn);
    }
    if ((p[c->pos[dim_y]][c->pos[dim_x] + 1].hn) &&
        (world.cur_map->hiker_dist[c->pos[dim_y]][c->pos[dim_x] + 1] >
         world.cur_map->hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
             ter_cost(c->pos[dim_x], c->pos[dim_y], char_hiker))) {
      world.cur_map->hiker_dist[c->pos[dim_y]][c->pos[dim_x] + 1] =
          world.cur_map->hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
          ter_cost(c->pos[dim_x], c->pos[dim_y], char_hiker);
      heap_decrease_key_no_replace(&h, p[c->pos[dim_y]][c->pos[dim_x] + 1].hn);
    }
    if ((p[c->pos[dim_y] + 1][c->pos[dim_x] - 1].hn) &&
        (world.cur_map->hiker_dist[c->pos[dim_y] + 1][c->pos[dim_x] - 1] >
         world.cur_map->hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
             ter_cost(c->pos[dim_x], c->pos[dim_y], char_hiker))) {
      world.cur_map->hiker_dist[c->pos[dim_y] + 1][c->pos[dim_x] - 1] =
          world.cur_map->hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
          ter_cost(c->pos[dim_x], c->pos[dim_y], char_hiker);
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y] + 1][c->pos[dim_x] - 1].hn);
    }
    if ((p[c->pos[dim_y] + 1][c->pos[dim_x]].hn) &&
        (world.cur_map->hiker_dist[c->pos[dim_y] + 1][c->pos[dim_x]] >
         world.cur_map->hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
             ter_cost(c->pos[dim_x], c->pos[dim_y], char_hiker))) {
      world.cur_map->hiker_dist[c->pos[dim_y] + 1][c->pos[dim_x]] =
          world.cur_map->hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
          ter_cost(c->pos[dim_x], c->pos[dim_y], char_hiker);
      heap_decrease_key_no_replace(&h, p[c->pos[dim_y] + 1][c->pos[dim_x]].hn);
    }
    if ((p[c->pos[dim_y] + 1][c->pos[dim_x] + 1].hn) &&
        (world.cur_map->hiker_dist[c->pos[dim_y] + 1][c->pos[dim_x] + 1] >
         world.cur_map->hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
             ter_cost(c->pos[dim_x], c->pos[dim_y], char_hiker))) {
      world.cur_map->hiker_dist[c->pos[dim_y] + 1][c->pos[dim_x] + 1] =
          world.cur_map->hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
          ter_cost(c->pos[dim_x], c->pos[dim_y], char_hiker);
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y] + 1][c->pos[dim_x] + 1].hn);
    }
  }
  heap_delete(&h);

  heap_init(&h, rival_cmp, NULL);

  for (y = 1; y < MAP_Y - 1; y++) {
    for (x = 1; x < MAP_X - 1; x++) {
      if (ter_cost(x, y, char_rival) != INT_MAX) {
        p[y][x].hn = heap_insert(&h, &p[y][x]);
      } else {
        p[y][x].hn = NULL;
      }
    }
  }

  while ((c = heap_remove_min(&h))) {
    c->hn = NULL;
    if ((p[c->pos[dim_y] - 1][c->pos[dim_x] - 1].hn) &&
        (world.cur_map->rival_dist[c->pos[dim_y] - 1][c->pos[dim_x] - 1] >
         world.cur_map->rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
             ter_cost(c->pos[dim_x], c->pos[dim_y], char_rival))) {
      world.cur_map->rival_dist[c->pos[dim_y] - 1][c->pos[dim_x] - 1] =
          world.cur_map->rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
          ter_cost(c->pos[dim_x], c->pos[dim_y], char_rival);
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y] - 1][c->pos[dim_x] - 1].hn);
    }
    if ((p[c->pos[dim_y] - 1][c->pos[dim_x]].hn) &&
        (world.cur_map->rival_dist[c->pos[dim_y] - 1][c->pos[dim_x]] >
         world.cur_map->rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
             ter_cost(c->pos[dim_x], c->pos[dim_y], char_rival))) {
      world.cur_map->rival_dist[c->pos[dim_y] - 1][c->pos[dim_x]] =
          world.cur_map->rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
          ter_cost(c->pos[dim_x], c->pos[dim_y], char_rival);
      heap_decrease_key_no_replace(&h, p[c->pos[dim_y] - 1][c->pos[dim_x]].hn);
    }
    if ((p[c->pos[dim_y] - 1][c->pos[dim_x] + 1].hn) &&
        (world.cur_map->rival_dist[c->pos[dim_y] - 1][c->pos[dim_x] + 1] >
         world.cur_map->rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
             ter_cost(c->pos[dim_x], c->pos[dim_y], char_rival))) {
      world.cur_map->rival_dist[c->pos[dim_y] - 1][c->pos[dim_x] + 1] =
          world.cur_map->rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
          ter_cost(c->pos[dim_x], c->pos[dim_y], char_rival);
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y] - 1][c->pos[dim_x] + 1].hn);
    }
    if ((p[c->pos[dim_y]][c->pos[dim_x] - 1].hn) &&
        (world.cur_map->rival_dist[c->pos[dim_y]][c->pos[dim_x] - 1] >
         world.cur_map->rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
             ter_cost(c->pos[dim_x], c->pos[dim_y], char_rival))) {
      world.cur_map->rival_dist[c->pos[dim_y]][c->pos[dim_x] - 1] =
          world.cur_map->rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
          ter_cost(c->pos[dim_x], c->pos[dim_y], char_rival);
      heap_decrease_key_no_replace(&h, p[c->pos[dim_y]][c->pos[dim_x] - 1].hn);
    }
    if ((p[c->pos[dim_y]][c->pos[dim_x] + 1].hn) &&
        (world.cur_map->rival_dist[c->pos[dim_y]][c->pos[dim_x] + 1] >
         world.cur_map->rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
             ter_cost(c->pos[dim_x], c->pos[dim_y], char_rival))) {
      world.cur_map->rival_dist[c->pos[dim_y]][c->pos[dim_x] + 1] =
          world.cur_map->rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
          ter_cost(c->pos[dim_x], c->pos[dim_y], char_rival);
      heap_decrease_key_no_replace(&h, p[c->pos[dim_y]][c->pos[dim_x] + 1].hn);
    }
    if ((p[c->pos[dim_y] + 1][c->pos[dim_x] - 1].hn) &&
        (world.cur_map->rival_dist[c->pos[dim_y] + 1][c->pos[dim_x] - 1] >
         world.cur_map->rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
             ter_cost(c->pos[dim_x], c->pos[dim_y], char_rival))) {
      world.cur_map->rival_dist[c->pos[dim_y] + 1][c->pos[dim_x] - 1] =
          world.cur_map->rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
          ter_cost(c->pos[dim_x], c->pos[dim_y], char_rival);
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y] + 1][c->pos[dim_x] - 1].hn);
    }
    if ((p[c->pos[dim_y] + 1][c->pos[dim_x]].hn) &&
        (world.cur_map->rival_dist[c->pos[dim_y] + 1][c->pos[dim_x]] >
         world.cur_map->rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
             ter_cost(c->pos[dim_x], c->pos[dim_y], char_rival))) {
      world.cur_map->rival_dist[c->pos[dim_y] + 1][c->pos[dim_x]] =
          world.cur_map->rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
          ter_cost(c->pos[dim_x], c->pos[dim_y], char_rival);
      heap_decrease_key_no_replace(&h, p[c->pos[dim_y] + 1][c->pos[dim_x]].hn);
    }
    if ((p[c->pos[dim_y] + 1][c->pos[dim_x] + 1].hn) &&
        (world.cur_map->rival_dist[c->pos[dim_y] + 1][c->pos[dim_x] + 1] >
         world.cur_map->rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
             ter_cost(c->pos[dim_x], c->pos[dim_y], char_rival))) {
      world.cur_map->rival_dist[c->pos[dim_y] + 1][c->pos[dim_x] + 1] =
          world.cur_map->rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
          ter_cost(c->pos[dim_x], c->pos[dim_y], char_rival);
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y] + 1][c->pos[dim_x] + 1].hn);
    }
  }
  heap_delete(&h);
}

void move_npc() {
  npc_t *p;
  struct timeval tv;
  int min, dir;

  gettimeofday(&tv, NULL);

  if ((p = heap_remove_min(&world.cur_map->npc_heap))) {
    switch (p->type) {
    case npc_hiker:
      min = INT_MAX;
      if (!is_move_invalid(p->pos[dim_x], p->pos[dim_y] - 1, world.num_trainers,
                           char_other) &&
          min > world.cur_map->hiker_dist[p->pos[dim_y] - 1][p->pos[dim_x]]) {
        min = world.cur_map->hiker_dist[p->pos[dim_y] - 1][p->pos[dim_x]];
        dir = dir_north;
      }
      if (!is_move_invalid(p->pos[dim_x], p->pos[dim_y] + 1, world.num_trainers,
                           char_other) &&
          min > world.cur_map->hiker_dist[p->pos[dim_y] + 1][p->pos[dim_x]]) {
        min = world.cur_map->hiker_dist[p->pos[dim_y] + 1][p->pos[dim_x]];
        dir = dir_south;
      }
      if (!is_move_invalid(p->pos[dim_x] + 1, p->pos[dim_y], world.num_trainers,
                           char_other) &&
          min > world.cur_map->hiker_dist[p->pos[dim_y]][p->pos[dim_x] + 1]) {
        min = world.cur_map->hiker_dist[p->pos[dim_y]][p->pos[dim_x] + 1];
        dir = dir_east;
      }
      if (!is_move_invalid(p->pos[dim_x] - 1, p->pos[dim_y], world.num_trainers,
                           char_other) &&
          min > world.cur_map->hiker_dist[p->pos[dim_y]][p->pos[dim_x] - 1]) {
        min = world.cur_map->hiker_dist[p->pos[dim_y]][p->pos[dim_x] - 1];
        dir = dir_west;
      }
      if (!is_move_invalid(p->pos[dim_x] - 1, p->pos[dim_y] - 1,
                           world.num_trainers, char_other) &&
          min >
              world.cur_map->hiker_dist[p->pos[dim_y] - 1][p->pos[dim_x] - 1]) {
        min = world.cur_map->hiker_dist[p->pos[dim_y] - 1][p->pos[dim_x] - 1];
        dir = dir_north_west;
      }
      if (!is_move_invalid(p->pos[dim_x] + 1, p->pos[dim_y] - 1,
                           world.num_trainers, char_other) &&
          min >
              world.cur_map->hiker_dist[p->pos[dim_y] - 1][p->pos[dim_x] + 1]) {
        min = world.cur_map->hiker_dist[p->pos[dim_y] - 1][p->pos[dim_x] + 1];
        dir = dir_north_east;
      }
      if (!is_move_invalid(p->pos[dim_x] - 1, p->pos[dim_y] + 1,
                           world.num_trainers, char_other) &&
          min >
              world.cur_map->hiker_dist[p->pos[dim_y] + 1][p->pos[dim_x] - 1]) {
        min = world.cur_map->hiker_dist[p->pos[dim_y] + 1][p->pos[dim_x] - 1];
        dir = dir_south_west;
      }
      if (!is_move_invalid(p->pos[dim_x] + 1, p->pos[dim_y] + 1,
                           world.num_trainers, char_other) &&
          min >
              world.cur_map->hiker_dist[p->pos[dim_y] + 1][p->pos[dim_x] + 1]) {
        min = world.cur_map->hiker_dist[p->pos[dim_y] + 1][p->pos[dim_x] + 1];
        dir = dir_south_east;
      }

      switch (dir) {
      case dir_north:
        p->pos[dim_y]--;
        break;
      case dir_south:
        p->pos[dim_y]++;
        break;
      case dir_east:
        p->pos[dim_x]++;
        break;
      case dir_west:
        p->pos[dim_x]--;
        break;
      case dir_north_west:
        p->pos[dim_y]--;
        p->pos[dim_x]--;
        break;
      case dir_south_west:
        p->pos[dim_y]++;
        p->pos[dim_x]--;
        break;
      case dir_north_east:
        p->pos[dim_y]--;
        p->pos[dim_x]++;
        break;
      case dir_south_east:
        p->pos[dim_y]++;
        p->pos[dim_x]++;
        break;
      }

      p->stat += move_cost[char_hiker]
                          [world.cur_map->map[p->pos[dim_y]][p->pos[dim_x]]] +
                 tv.tv_usec;
      break;
    case npc_rival:
      min = INT_MAX;
      if (!is_move_invalid(p->pos[dim_x], p->pos[dim_y] - 1, world.num_trainers,
                           char_other) &&
          min > world.cur_map->rival_dist[p->pos[dim_y] - 1][p->pos[dim_x]]) {
        min = world.cur_map->rival_dist[p->pos[dim_y] - 1][p->pos[dim_x]];
        dir = dir_north;
      }
      if (!is_move_invalid(p->pos[dim_x], p->pos[dim_y] + 1, world.num_trainers,
                           char_other) &&
          min > world.cur_map->rival_dist[p->pos[dim_y] + 1][p->pos[dim_x]]) {
        min = world.cur_map->rival_dist[p->pos[dim_y] + 1][p->pos[dim_x]];
        dir = dir_south;
      }
      if (!is_move_invalid(p->pos[dim_x] + 1, p->pos[dim_y], world.num_trainers,
                           char_other) &&
          min > world.cur_map->rival_dist[p->pos[dim_y]][p->pos[dim_x] + 1]) {
        min = world.cur_map->rival_dist[p->pos[dim_y]][p->pos[dim_x] + 1];
        dir = dir_east;
      }
      if (!is_move_invalid(p->pos[dim_x] - 1, p->pos[dim_y], world.num_trainers,
                           char_other) &&
          min > world.cur_map->rival_dist[p->pos[dim_y]][p->pos[dim_x] - 1]) {
        min = world.cur_map->rival_dist[p->pos[dim_y]][p->pos[dim_x] - 1];
        dir = dir_west;
      }
      if (!is_move_invalid(p->pos[dim_x] - 1, p->pos[dim_y] - 1,
                           world.num_trainers, char_other) &&
          min >
              world.cur_map->rival_dist[p->pos[dim_y] - 1][p->pos[dim_x] - 1]) {
        min = world.cur_map->rival_dist[p->pos[dim_y] - 1][p->pos[dim_x] - 1];
        dir = dir_north_west;
      }
      if (!is_move_invalid(p->pos[dim_x] + 1, p->pos[dim_y] - 1,
                           world.num_trainers, char_other) &&
          min >
              world.cur_map->rival_dist[p->pos[dim_y] - 1][p->pos[dim_x] + 1]) {
        min = world.cur_map->rival_dist[p->pos[dim_y] - 1][p->pos[dim_x] + 1];
        dir = dir_north_east;
      }
      if (!is_move_invalid(p->pos[dim_x] - 1, p->pos[dim_y] + 1,
                           world.num_trainers, char_other) &&
          min >
              world.cur_map->rival_dist[p->pos[dim_y] + 1][p->pos[dim_x] - 1]) {
        min = world.cur_map->rival_dist[p->pos[dim_y] + 1][p->pos[dim_x] - 1];
        dir = dir_south_west;
      }
      if (!is_move_invalid(p->pos[dim_x] + 1, p->pos[dim_y] + 1,
                           world.num_trainers, char_other) &&
          min >
              world.cur_map->rival_dist[p->pos[dim_y] + 1][p->pos[dim_x] + 1]) {
        min = world.cur_map->rival_dist[p->pos[dim_y] + 1][p->pos[dim_x] + 1];
        dir = dir_south_east;
      }

      switch (dir) {
      case dir_north:
        p->pos[dim_y]--;
        break;
      case dir_south:
        p->pos[dim_y]++;
        break;
      case dir_east:
        p->pos[dim_x]++;
        break;
      case dir_west:
        p->pos[dim_x]--;
        break;
      case dir_north_west:
        p->pos[dim_y]--;
        p->pos[dim_x]--;
        break;
      case dir_south_west:
        p->pos[dim_y]++;
        p->pos[dim_x]--;
        break;
      case dir_north_east:
        p->pos[dim_y]--;
        p->pos[dim_x]++;
        break;
      case dir_south_east:
        p->pos[dim_y]++;
        p->pos[dim_x]++;
        break;
      }

      p->stat += move_cost[char_rival]
                          [world.cur_map->map[p->pos[dim_y]][p->pos[dim_x]]] +
                 tv.tv_usec;
      break;
    case npc_pacer:
      if (p->dir % 4 == dir_north) {
        if (!is_move_invalid(p->pos[dim_x], p->pos[dim_y] - 1,
                             world.num_trainers, char_other)) {
          p->pos[dim_y] = p->pos[dim_y] - 1;
        } else {
          p->dir = dir_south;
        }
      } else if (p->dir % 4 == dir_south) {
        if (!is_move_invalid(p->pos[dim_x], p->pos[dim_y] + 1,
                             world.num_trainers, char_other)) {
          p->pos[dim_y] = p->pos[dim_y] + 1;
        } else {
          p->dir = dir_north;
        }
      } else if (p->dir % 4 == dir_east) {
        if (!is_move_invalid(p->pos[dim_x] + 1, p->pos[dim_y],
                             world.num_trainers, char_other)) {
          p->pos[dim_x] = p->pos[dim_x] + 1;
        } else {
          p->dir = dir_west;
        }
      } else if (p->dir % 4 == dir_west) {
        if (!is_move_invalid(p->pos[dim_x] - 1, p->pos[dim_y],
                             world.num_trainers, char_other)) {
          p->pos[dim_x] = p->pos[dim_x] - 1;
        } else {
          p->dir = dir_east;
        }
      }
      p->stat += move_cost[char_other]
                          [world.cur_map->map[p->pos[dim_y]][p->pos[dim_x]]] +
                 tv.tv_usec;
      break;
    case npc_wanderer:
      if (p->dir % 4 == dir_north) {
        if (world.cur_map->map[p->pos[dim_y]][p->pos[dim_x]] ==
                world.cur_map->map[p->pos[dim_y] - 1][p->pos[dim_x]] &&
            !is_move_invalid(p->pos[dim_x], p->pos[dim_y] - 1,
                             world.num_trainers, char_other)) {
          p->pos[dim_y] = p->pos[dim_y] - 1;
        } else {
          p->dir = dir_south;
        }
      } else if (p->dir % 4 == dir_south) {
        if (world.cur_map->map[p->pos[dim_y]][p->pos[dim_x]] ==
                world.cur_map->map[p->pos[dim_y] + 1][p->pos[dim_x]] &&
            !is_move_invalid(p->pos[dim_x], p->pos[dim_y] + 1,
                             world.num_trainers, char_other)) {
          p->pos[dim_y] = p->pos[dim_y] + 1;
        } else {
          p->dir = dir_north;
        }
      } else if (p->dir % 4 == dir_east) {
        if (world.cur_map->map[p->pos[dim_y]][p->pos[dim_x]] ==
                world.cur_map->map[p->pos[dim_y]][p->pos[dim_x] + 1] &&
            !is_move_invalid(p->pos[dim_x] + 1, p->pos[dim_y],
                             world.num_trainers, char_other)) {
          p->pos[dim_x] = p->pos[dim_x] + 1;
        } else {
          p->dir = dir_west;
        }
      } else if (p->dir % 4 == dir_west) {
        if (world.cur_map->map[p->pos[dim_y]][p->pos[dim_x]] ==
                world.cur_map->map[p->pos[dim_y]][p->pos[dim_x] - 1] &&
            !is_move_invalid(p->pos[dim_x] - 1, p->pos[dim_y],
                             world.num_trainers, char_other)) {
          p->pos[dim_x] = p->pos[dim_x] - 1;
        } else {
          p->dir = dir_east;
        }
      }
      p->stat += move_cost[char_other]
                          [world.cur_map->map[p->pos[dim_y]][p->pos[dim_x]]] +
                 tv.tv_usec;
      break;
    case npc_stationary:
    case npc_random_walker:
      p->stat += move_cost[char_other]
                          [world.cur_map->map[p->pos[dim_y]][p->pos[dim_x]]] +
                 tv.tv_usec;
      break;
    }
    p->hn = heap_insert(&world.cur_map->npc_heap, p);
  }
}

void init_pc() {
  int x, y;

  do {
    x = rand() % (MAP_X - 2) + 1;
    y = rand() % (MAP_Y - 2) + 1;
  } while (world.cur_map->map[y][x] != ter_path);

  world.pc.pos[dim_x] = x;
  world.pc.pos[dim_y] = y;
}

void print_hiker_dist() {
  int x, y;

  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      if (world.cur_map->hiker_dist[y][x] == INT_MAX) {
        printf("   ");
      } else {
        printf(" %02d", world.cur_map->hiker_dist[y][x] % 100);
      }
    }
    printf("\n");
  }
}

void print_rival_dist() {
  int x, y;

  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      if (world.cur_map->rival_dist[y][x] == INT_MAX ||
          world.cur_map->rival_dist[y][x] < 0) {
        printf("   ");
      } else {
        printf(" %02d", world.cur_map->rival_dist[y][x] % 100);
      }
    }
    printf("\n");
  }
}

void initialize_window() {
  int x, y;

  initscr();
  raw();
  // cbreak();
  keypad(stdscr, TRUE);

  start_color();
  init_color(COLOR_MUSTARD, 600, 600, 200);
  init_color(COLOR_LIGHT_BROWN, 400, 150, 0);
  init_color(COLOR_LIGHTER_BROWN, 500, 180, 0);
  init_color(COLOR_BROWN, 300, 100, 0);
  init_color(COLOR_GREY, 350, 350, 350);
  init_color(COLOR_LIGHT_GREY, 500, 500, 500);
  init_color(COLOR_LIGHT_GREEN, 300, 750, 45);
  init_color(COLOR_GREEN, 200, 500, 40);

  // init_pair(color_pc, COLOR_RED, COLOR_BLACK);
  // init_pair(color_clearing, COLOR_MUSTARD, COLOR_LIGHTER_BROWN);
  // init_pair(color_grass, COLOR_GREEN, COLOR_LIGHT_GREEN);
  // init_pair(color_mart, COLOR_BLACK, COLOR_BLUE);
  // init_pair(color_center, COLOR_BLACK, COLOR_BLUE);
  // init_pair(color_path, COLOR_LIGHT_BROWN, COLOR_BROWN);
  // init_pair(color_boulder, COLOR_LIGHT_GREY, COLOR_GREY);
  // init_pair(color_tree, COLOR_BROWN, COLOR_GREEN);

  init_pair(color_pc, COLOR_RED, COLOR_BLACK);
  init_pair(color_clearing, COLOR_MUSTARD, COLOR_BLACK);
  init_pair(color_grass, COLOR_GREEN, COLOR_BLACK);
  init_pair(color_mart, COLOR_BLUE, COLOR_BLACK);
  init_pair(color_center, COLOR_BLUE, COLOR_BLACK);
  init_pair(color_path, COLOR_LIGHT_BROWN, COLOR_BLACK);
  init_pair(color_boulder, COLOR_LIGHT_GREY, COLOR_BLACK);
  init_pair(color_tree, COLOR_BROWN, COLOR_BLACK);

  getmaxyx(stdscr, y, x);

  while (y < MAP_Y + 3 || x < MAP_X + 1) {
    mvprintw(0, 0, "Please make your terminal at least %d X %d and hit any key",
             MAP_X, MAP_Y + 3);
    mvprintw(y / 2, x / 2 - 4, "%d X %d", x - 1, y);
    refresh();
    getmaxyx(stdscr, y, x);
    erase();
  }
}

int main(int argc, char *argv[]) {
  struct timeval tv;
  uint32_t seed;
  char c;
  int x, y;

  if (argc == 2) {
    seed = atoi(argv[1]);
    world.num_trainers = 10;
  } else if (argc == 3 && strcmp(argv[1], "--numtrainers") == 0) {
    gettimeofday(&tv, NULL);
    seed = (tv.tv_usec ^ (tv.tv_sec << 20)) & 0xffffffff;
    world.num_trainers = atoi(argv[2]);
  } else if (argc == 4 && strcmp(argv[2], "--numtrainers") == 0) {
    seed = atoi(argv[1]);
    world.num_trainers = atoi(argv[3]);
  } else {
    world.num_trainers = 10;
    gettimeofday(&tv, NULL);
    seed = (tv.tv_usec ^ (tv.tv_sec << 20)) & 0xffffffff;
  }

  initialize_window();

  erase();
  mvprintw(0, 0, "Using seed: %u", seed);
  attron(A_BLINK | A_BOLD);
  mvprintw((MAP_Y + 3) / 2, MAP_X / 2 - 13, "Hit any key to start game");
  refresh();
  getch();
  attrset(A_NORMAL);
  erase();
  srand(seed);

  init_world();
  init_pc();
  pathfind(world.cur_map);

  timeout(0);
  do {
    if (c != -1)
      erase();
    print_map();
    print_characters();

    mvprintw(LINES - 1, 0,
             "Current position is %d%cx%d%c (%d,%d).  "
             "Enter command: ",
             abs(world.cur_idx[dim_x] - (WORLD_SIZE / 2)),
             world.cur_idx[dim_x] - (WORLD_SIZE / 2) >= 0 ? 'E' : 'W',
             abs(world.cur_idx[dim_y] - (WORLD_SIZE / 2)),
             world.cur_idx[dim_y] - (WORLD_SIZE / 2) <= 0 ? 'N' : 'S',
             world.cur_idx[dim_x] - (WORLD_SIZE / 2),
             world.cur_idx[dim_y] - (WORLD_SIZE / 2));

    refresh();

    halfdelay(1);
    c = getch();
    switch (c) {
    case 's':
      if (!is_move_invalid(world.pc.pos[dim_x], world.pc.pos[dim_y] + 1,
                           world.num_trainers, char_pc)) {
        world.pc.pos[dim_y]++;
        pathfind(world.cur_map);
      }
      break;
    case 'w':
      if (!is_move_invalid(world.pc.pos[dim_x], world.pc.pos[dim_y] - 1,
                           world.num_trainers, char_pc)) {
        world.pc.pos[dim_y]--;
        pathfind(world.cur_map);
      }
      break;
    case 'a':
      if (!is_move_invalid(world.pc.pos[dim_x] - 1, world.pc.pos[dim_y],
                           world.num_trainers, char_pc)) {
        world.pc.pos[dim_x]--;
        pathfind(world.cur_map);
      }
      break;
    case 'd':
      if (!is_move_invalid(world.pc.pos[dim_x] + 1, world.pc.pos[dim_y],
                           world.num_trainers, char_pc)) {
        world.pc.pos[dim_x]++;
        pathfind(world.cur_map);
      }
    case 'f':
      scanw(" %d %d", &x, &y);
      if (x >= -(WORLD_SIZE / 2) && x <= WORLD_SIZE / 2 &&
          y >= -(WORLD_SIZE / 2) && y <= WORLD_SIZE / 2) {
        world.cur_idx[dim_x] = x + (WORLD_SIZE / 2);
        world.cur_idx[dim_y] = y + (WORLD_SIZE / 2);
        new_map();
      }
      break;
    case '?':
    case 'h':
      printw("Move with 'e'ast, 'w'est, 'n'orth, 's'outh or 'f'ly x y.\n"
             "Quit with 'q'.  '?' a this help message.");
      break;
    default:
      break;
    }

    move_npc();

  } while (c != 'q');

  delete_world();

  endwin();

  return 0;
}
