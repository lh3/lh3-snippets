#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

const char *mm_colors = "RGBWOPYSE";
const char *mm_numbers = "123456789";

long *mm_enumerate(int m, int n, int rep, long *_z) // m colors, n columns
{
	int j, *b;
	long *a, k, x = 1L, z;
	b = alloca(m * sizeof(int));
	for (j = 0; j < n; ++j) x *= m;
	a = calloc(x, sizeof(long));
	for (k = z = 0; k < x; ++k) {
		if (!rep) {
			long y = k;
			memset(b, 0, m * sizeof(int));
			for (j = 0; j < n; ++j) {
				if (++b[y%m] > 1) break;
				y /= m;
			}
			if (j < n) continue;
		}
		a[z++] = k;
	}
	*_z = z;
	return a;
}

unsigned mm_eval(int m, int n, long t, long g)
{
	int i, j, *ct, *cg, n_precise = 0, n_present = 0;
	ct = alloca(m * sizeof(int)); memset(ct, 0, m * sizeof(int));
	cg = alloca(m * sizeof(int)); memset(cg, 0, m * sizeof(int));
	for (j = 0; j < n; ++j, t /= m, g /= m) {
		int tt = t % m, gg = g % m;
		if (tt == gg) ++n_precise;
		++ct[tt], ++cg[gg];
	}
	for (i = 0; i < m; ++i)
		n_present += ct[i] < cg[i]? ct[i] : cg[i];
	return (unsigned)n_precise<<16 | (n_present-n_precise);
}

char *mm_int2str(int m, int n, const char *colors, long g)
{
	char *r;
	int j;
	r = calloc(n + 1, 1);
	for (j = 0; j < n; ++j, g /= m)
		r[j] = colors[g%m];
	return r;
}

int main(int argc, char *argv[])
{
	int c, n_colors = 6, n_cols = 4, repeated = 0, n_rounds = 0, show_number = 0;
	long *a, n_a;

	srand48(time(0));
	while ((c = getopt(argc, argv, "c:n:rhN")) >= 0) {
		if (c == 'c') n_colors = atoi(optarg);
		else if (c == 'n') n_cols = atoi(optarg);
		else if (c == 'r') repeated = 1;
		else if (c == 'N') show_number = 1;
		else if (c == 'h') {
			fprintf(stderr, "Usage: ./mastermind [options]\n");
			fprintf(stderr, "Options:\n");
			fprintf(stderr, "  -c INT     number of colors [6]\n");
			fprintf(stderr, "  -n INT     number of positions [4]\n");
			fprintf(stderr, "  -r         with repitition\n");
			fprintf(stderr, "  -N         use numbers instead of colors\n");
			fprintf(stderr, "Note:\n");
			fprintf(stderr, "  For each round, input two numbers separated by a space:\n");
			fprintf(stderr, "    First:  number of codes with correct colors AND positions.\n");
			fprintf(stderr, "    Second: number of codes with correct colors but incorrect positions.\n");
			return 1;
		}
	}

	a = mm_enumerate(n_colors, n_cols, repeated, &n_a);
	for (;;) {
		int g, e[2];
		long k, *b, n_b = 0;
		char *gstr;
		g = a[(long)(n_a * drand48())];
		printf(" * Round %d, %ld solutions remains\n", ++n_rounds, n_a);
		gstr = mm_int2str(n_colors, n_cols, show_number? mm_numbers : mm_colors, g);
		printf("   Guess: %s\n", gstr);
		free(gstr);
		printf(" > ");
		scanf("%d%d", &e[0], &e[1]);
		if (e[0] == n_cols) {
			printf("* DONE in %d rounds\n", n_rounds);
			break;
		}
		b = calloc(n_a, sizeof(long));
		for (k = 0; k < n_a; ++k) {
			unsigned r;
			r = mm_eval(n_colors, n_cols, a[k], g);
			if (r>>16 == e[0] && (r&0xffff) == e[1])
				b[n_b++] = a[k];
		}
		free(a);
		a = b, n_a = n_b;
		if (n_b == 0) {
			printf("* Conflicting input!\n");
			break;
		}
	}
	free(a);
	return 0;
}
