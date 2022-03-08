#include <stdio.h>
#include <stdlib.h>

void swapchar(int *x, int *y)
{
    int tmp = *x;
    *x = *y;
    *y = tmp;
}

char *strndup(const char *s, size_t n)
{
    int i;
    char *copy = malloc((n + 1) * sizeof(*copy));
    if (copy == NULL)
        return NULL;

    for (i = 0; i < n && *s; i++, s++)
        copy[i] = *s;

    copy[i] = '\0';
    return copy;
}

void *memcpy(void *dest, const void *src, size_t n)
{
    int i;
    for (i = 0; i < n; i++)
        ((char *)dest)[i] = ((char *)src)[i];
    return dest;
}

int main()
{
    // Question 1
    // int x = 'a', y = 'b';
    // swapchar(&x, &y);
    // printf("%c %c\n", x, y); // prints b a

    // Question 2
    // char *s = "Hello, World!";
    // // char *copy = strndup(s, 20);
    // char *copy = strndup(s, 10);
    // printf("%s\n", copy);

    // Question 6
    char *src = "Test string";
    char dest[50];
    memcpy(dest, src, 10);
    printf("%s\n", dest);

    return 0;
}