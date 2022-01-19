#include <stdio.h>

#define N 5
#define KNIGHT_MOVES 8

struct move
{
    int x;
    int y;
};

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

int print_path(int arr[])
{
    int i, lastIndex;
    char separator;

    separator = ',';
    lastIndex = N * N - 1;
    for (i = 0; i < N * N; i++)
    {
        if (i == lastIndex)
            separator = '\n';
        printf("%d%c", separator, arr[i]);
    }

    return 1;
}

int is_invalid_move(int x, int y, int board[][N])
{
    return x < 0 || y < 0 || x >= N || y >= N || board[x][y] != 0;
}

int solve_knights_tour_recur(int x, int y, int moveNum, struct move knightMoves[KNIGHT_MOVES], int board[][N], int path[])
{
    int i;
    int tourCompleted = 0;

    if (is_invalid_move(x, y, board))
        return 0;

    board[x][y] = 1;
    path[moveNum++] = (x + 1) + N * y;

    if (moveNum == N * N)
    {
        // print_board(path, moveNum);
        return print_path(path);
    }

    for (i = 0; i < KNIGHT_MOVES; i++)
        if (solve_knights_tour_recur(x + knightMoves[i].x, y + knightMoves[i].y, moveNum, knightMoves, board, path))
            tourCompleted = 1;

    return tourCompleted;
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

    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++)
            if (solve_knights_tour_recur(i, j, 0, moves, board, path))
                hasFinishedTour = 1;

    if (hasFinishedTour == 0)
        printf("No tours found\n");

    printf("Final: i = %d, j = %d\n", i, j);
}

int main(int argc, char *argv[])
{
    solve_knights_tour();
    return 0;
}