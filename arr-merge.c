#define type_t int

void merge_sorted(long n_a, const type_t *a, long n_b, const type_t *b, type_t *c)
{
    long i = 0, j = 0, k = 0;
    while (i < n_a && j < n_b)
        c[k++] = a[i] < b[j]? a[i++] : b[j++];
    while (i < n_a) c[k++] = a[i++];
    while (j < n_b) c[k++] = b[j++];
}

/* main function */

#include <stdio.h>
static int a[] = { 1, 3, 5, 10, 20 };
static int b[] = { 2, 4, 5, 23, 26 };
int main(void)
{
    int i, c[10];
    merge_sorted(5, a, 5, b, c);
    for (i = 0; i < 10; ++i) printf("%d\n", c[i]);
    return 0;
}
