#include <stdlib.h>

/* sample $k items from $n ($n > $k) items; write indices to $a */
void reservoir_sample(long n, long k, long *a)
{
    long i, x = 0, y;
    for (i = 0; i < n; ++i) { // can be streamed
        y = x++ < k? x - 1 : (long)(drand48() * x);
        if (y < k) a[y] = i;
    }
}

/* main function */

#include <stdio.h>
int main(void)
{
    long i, a[10];
    reservoir_sample(1000, 10, a);
    for (i = 0; i < 10; ++i) printf("%ld\n", a[i]);
    return 0;
}
