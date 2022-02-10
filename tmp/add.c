#include <stdio.h>

void add(int a, int b)
{
    int sum = a + b;
    printf("%d\n", sum);
}
int main(int argc, char *argv[])
{
    add(10, 23);
    return 0;
}