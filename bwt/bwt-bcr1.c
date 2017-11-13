#include <string.h>
#include <stdio.h>

int str2bwt(char *s)
{
    int i, l, c = 0, k = 0;
    l = strlen(s); // length of input string
    for (i = l - 1; i >= 0; --i) {
        int j, r = 0, a = s[i];
        memmove(&s[i], &s[i+1], k); // make room for insertion
        s[i+k] = a; // insert the char
        for (j = i; j < i + k; ++j) r += (s[j] <= a);
        for (; j < l; ++j) r += (s[j] < a);
        k = r + 1, c = a;
    }
    return k; // the position of sentinel '$'
}

int main(int argc, char *argv[])
{
    int k;
    if (argc == 1) return 1;
    k = str2bwt(argv[1]);
    fwrite(argv[1], 1, k, stdout); putchar('$'); puts(&argv[1][k]);
    return 0;
}
