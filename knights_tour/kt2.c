#include <stdio.h>
#include <stdlib.h>

#define NUM_MOVES 8

typedef struct _coordinate
{
    int x;
    int y;
} vector;

int board[5][5];
int *path;
vector moves[] = {{-1, 2}, {-2, 1}, {-2, -1}, {-1, -2}, {1, -2}, {2, -1}, {2, 1}, {1, 2}};

int count = 0;

int is_valid_move(int x, int y)
{
    return x >= 0 && x < 5 && y >= 0 && y < 5 && board[y][x] == 0;
}

int show_tour(int len)
{
    int i;

    for (i = 0; i < len; i++)
        if (i == len - 1)
            printf("%d\n", path[i]);
        else
            printf("%d,", path[i]);

    count++;
    return 1;
}

void print_board()
{
    int i, j;

    for (i = 0; i < 5; i++)
    {
        for (j = 0; j < 5; j++)
            printf(" %2d ", board[i][j]);
        printf("\n");
    }

    printf("\n");
}

int knights_tour_util(int x, int y, int move)
{
    int i, xMove, yMove;
    int flag = 0;

    board[y][x] = move + 1;

    path[move] = 5 * y + x + 1;

    if (move == 24)
        flag = show_tour(move + 1);
    else
        for (i = 0; i < NUM_MOVES; i++)
        {
            xMove = x + moves[i].x;
            yMove = y + moves[i].y;

            if (is_valid_move(xMove, yMove) && knights_tour_util(xMove, yMove, move + 1))
                flag = 1;
        }

    path[move] = 0;
    board[y][x] = 0;

    return flag;
}

void init_board()
{
    int i, j;
    for (i = 0; i < 5; i++)
        for (j = 0; j < 5; j++)
            board[i][j] = 0;
}

void solve_knights_tour()
{
    int i, j;
    int success = 0;

    path = (int *)malloc(25 * sizeof(int));

    init_board();

    for (i = 0; i < 5; i++)
        for (j = 0; j < 5; j++)
            if (knights_tour_util(j, i, 0))
                success = 1;

    if (success)
        printf("%d tours found\n", count);
    else
        printf("No tours found\n");

    free(path);
}

int main(int argc, char *argv[])
{
    solve_knights_tour();
    return 0;
}