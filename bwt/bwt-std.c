#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int string_cmp(const void *a, const void *b) 
{ 
    const char **ia = (const char **)a;
    const char **ib = (const char **)b;
    return strcmp(*ia, *ib);
} 

char *str2bwt(char *s)
{
    char *t, **q;
    int i, l;
    l = strlen(s);
    q = (char**)malloc((l + 1) * sizeof(char*));
    for (i = 0; i <= l; ++i)
        q[i] = s + i;
    qsort(q, l+1, sizeof(char*), string_cmp);
    t = (char*)calloc(l + 2, 1);
    for (i = 0; i <= l; ++i)
        t[i] = q[i] == s? '$' : *(q[i] - 1);
    free(q);
    return t;
}

int main(int argc, char *argv[])
{
    char *s;
    if (argc == 1) return 1;
    s = str2bwt(argv[1]);
    puts(s);
    free(s);
    return 0;
}
