#include <stdio.h>

#define N 5
#define KNIGHT_MOVES 8

struct move
{
    int x;
    int y;
};

int count = 0;

void print_board(int arr[], int len)
{
    int i, j;
    int board[N][N];

    for (j = 0; j < N; j++)
        for (i = 0; i < N; i++)
            board[j][i] = 0;

    for (i = 0; i < len; i++)
        board[(arr[i] - 1) / N][(arr[i] - 1) % N] = i + 1;

    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
            printf("%2d ", board[i][j]);
        printf("\n");
    }

    printf("\n");
}

int print_path(int arr[], int len)
{
    int i;
    char separator;

    separator = ',';
    for (i = 0; i < len; i++)
    {
        if (i == len - 1)
            separator = '\n';
        printf("%d%c", separator, arr[i]);
    }

    return 1;
}

int is_on_board(int x, int y)
{
    return x >= 0 && x < N && y >= 0 && y < N;
}

int solve_knights_tour_recur(int x, int y, int moveNum, struct move knightMoves[KNIGHT_MOVES], int board[][N], int path[])
{
    int i, nextX, nextY;
    int completed = 0;

    board[x][y] = 1;
    path[moveNum++] = (x + 1) + N * y;

    if (moveNum == N * N)
    {
        print_board(path, moveNum);
        return print_path(path, N * N);
    }

    for (i = 0; i < KNIGHT_MOVES; i++)
    {
        if (moveNum == 20)
        {
            printf("go\n");
            print_board(path, moveNum);
        }

        nextX = x + knightMoves[i].x;
        nextY = y + knightMoves[i].y;

        if (is_on_board(nextX, nextY) && board[nextX][nextY] == 0)
            if (solve_knights_tour_recur(nextX, nextY, moveNum, knightMoves, board, path))
                completed = 1;
    }

    return completed;
}

void solve_knights_tour()
{
    int i, j;
    int board[N][N];
    int path[N * N];
    int hasFinishedTour = 0;

    struct move moves[KNIGHT_MOVES] = {{2, 1}, {1, 2}, {-1, 2}, {-2, 1}, {-2, -1}, {-1, -2}, {1, -2}, {2, -1}};

    for (j = 0; j < N; j++)
        for (i = 0; i < N; i++)
            board[j][i] = 0;

    for (j = 0; j < N; j++)
        for (i = 0; i < N; i++)
            if (solve_knights_tour_recur(i, j, 0, moves, board, path))
                hasFinishedTour = 1;

    if (hasFinishedTour == 0)
        printf("No tours found\n");
}

int main(int argc, char *argv[])
{
    solve_knights_tour();
    return 0;
}