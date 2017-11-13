// from wiki, retrieved on 2016-07-24
#include <stdlib.h>
#include <string.h>

#define type_t int
#define sort_lt(a, b) ((a) < (b))

long ks_lis1(long n, const type_t *a, long *b)
{
    long i, k, L = 0, *M, *P = b;
    M = (long*)malloc((n + 1) * sizeof(long));
    for (i = 0; i < n; ++i) {
        long lo = 1, hi = L, newL;
        while (lo <= hi) {
            long mid = (lo + hi + 1) >> 1;
            if (a[M[mid]] < a[i]) lo = mid + 1;
            else hi = mid - 1;
        }
        newL = lo, P[i] = M[newL - 1], M[newL] = i;
        if (newL > L) L = newL;
    }
    k = M[L];
    memcpy(M, P, n * sizeof(long));
    for (i = L - 1; i >= 0; --i) b[i] = k, k = M[k];
    free(M);
    return L;
}

/* main function for testing */

#include <stdio.h>
static int arr[] = { 10, 3, 8, 4, 2, 13, 5, 24, 6, 26 };
int main(void)
{
    long i, m, lis[10];
    m = ks_lis1(10, arr, lis);
    for (i = 0; i < m; ++i) printf("%d\n", arr[lis[i]]);
    return 0;
}
