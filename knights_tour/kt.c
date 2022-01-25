#include <stdio.h>
#include <stdlib.h>

#define NUM_MOVES 8
#define DIM 5

typedef struct _coordinate
{
    int x;
    int y;
} vector;

int board[DIM][DIM];
int *path;
vector moves[] = {{-1, 2}, {-2, 1}, {-2, -1}, {-1, -2}, {1, -2}, {2, -1}, {2, 1}, {1, 2}};

int count = 0;

int is_valid_move(int x, int y)
{
    return x >= 0 && x < DIM && y >= 0 && y < DIM && board[y][x] == 0;
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

    for (i = 0; i < DIM; i++)
    {
        for (j = 0; j < DIM; j++)
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

    path[move] = DIM * y + x + 1;

    if (move == DIM * DIM - 1)
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
    for (i = 0; i < DIM; i++)
        for (j = 0; j < DIM; j++)
            board[i][j] = 0;
}

void solve_knights_tour()
{
    int i, j;
    int success = 0;

    path = (int *)malloc(DIM * DIM * sizeof(int));

    init_board();

    for (i = 0; i < DIM; i++)
        for (j = 0; j < DIM; j++)
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