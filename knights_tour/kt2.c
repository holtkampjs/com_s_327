#include <stdio.h>

#define NUM_MOVES 8

typedef struct _coordinate
{
    int x;
    int y;
} vector;

int board[5][5];
vector moves[] = {
    {-1, 2},
    {-2, 1},
    {-2, -1},
    {-1, -2},
    {1, -2},
    {2, -1},
    {2, 1},
    {1, 2},
};

int is_valid_move(int x, int y)
{
    return x >= 0 && x < 5 && y >= 0 && y < 5 && board[y][x] == 0;
}

int show_tour(int *tour, int len)
{
    int i;
    for (i = 0; i < len; i++)
        if (i == len - 1)
            printf("%d\n", tour[i]);
        else
            printf("%d,", tour[i]);

    return 1;
}

int knights_tour_util(int x, int y, int move, int *tour)
{
    int i, xMove, yMove;
    int flag = 0;

    board[y][x] = move;

    tour[move] = 5 * y + x + 1;

    if (move + 1 == 25)
        return show_tour(tour, move + 1);

    for (i = 0; i < NUM_MOVES; i++)
    {
        xMove = x + moves[i].x;
        yMove = y + moves[i].y;

        if (is_valid_move(xMove, yMove) && knights_tour_util(xMove, yMove, move + 1, tour))
            flag = 1;
    }

    board[y][x] = 0;

    return flag;
}

void solve_knights_tour()
{
    int i, j;
    int *path = (int *)malloc(25 * sizeof(int));

    for (i = 0; i < 5; i++)
        for (j = 0; j < 5; j++)
            board[i][j] = 0;

    for (i = 0; i < 5; i++)
        for (j = 0; j < 5; j++)
            knights_tour_util(i, j, 0, path);

    free(path);
}

int main(int argc, char *argv[])
{
    solve_knights_tour();
    return 0;
}