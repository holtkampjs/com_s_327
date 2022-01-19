#define N 5
#define KNIGHT_MOVES 8

struct move
{
    int x;
    int y;
};

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

    return 0;
}

int solve_knights_tour_recur(int x, int y, int moveNum, struct move *knightMoves[KNIGHT_MOVES], int board[N][N], int path[])
{
    int i, xMove, yMove;
    int tourCompleted = -1;

    board[x][y] = 1;
    path[moveNum++] = (x + 1) + N * (y + 1);

    if (moveNum == N * N)
        return print_path(path);

    for (i = 0; i < KNIGHT_MOVES; i++)
    {
        xMove = x + knightMoves[i]->x;
        yMove = y + knightMoves[i]->y;

        if (is_unvisited_on_board(xMove, yMove, &board))
            if (solve_knights_tour_recur(xMove, yMove, moveNum, knightMoves, board, path) == 0)
                tourCompleted = 0;
    }

    return tourCompleted;
}

void solve_knights_tour()
{
    int i, j;
    int board[N][N];
    int path[N * N];
    int noToursFound = 1;

    struct move moves[KNIGHT_MOVES] = {{2, 1}, {1, 2}, {-1, 2}, {-2, 1}, {-2, -1}, {-1, -2}, {1, -2}, {2, -1}};

    for (j = 0; j < N; j++)
        for (i = 0; i < N; i++)
            board[j][i] = 0;

    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++)
            if (solve_knights_tour_recur(i, j, 0, &moves, board, path) != 0)
                noToursFound = 0;

    if (noToursFound)
        printf("No tours found\n");
}

int main(int argc, char *argv[])
{
    solve_knights_tour();
    return 0;
}