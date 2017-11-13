#include <stdlib.h>

#define type_t int

void shuffle(long n, type_t a[])
{
    long i, j;
    for (i = n; i > 1; --i) {
        type_t tmp;
        j = (int)(drand48() * i);
        tmp = a[j]; a[j] = a[i-1]; a[i-1] = tmp;
    }
}

/* main function */

#include <stdio.h>
static int tmp[] = { 1, 2, 3, 4, 5, 6 };
int main(void)
{
    int i;
    shuffle(6, tmp);
    for (i = 0; i < 6; ++i) printf("%d\n", tmp[i]);
    return 0;
}
