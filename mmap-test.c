#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/mman.h>

double cputime(void)
{
	struct rusage r;
	getrusage(RUSAGE_SELF, &r);
	return r.ru_utime.tv_sec + r.ru_stime.tv_sec + 1e-6 * (r.ru_utime.tv_usec + r.ru_stime.tv_usec);
}

double realtime(void)
{
	struct timeval tp;
	struct timezone tzp;
	gettimeofday(&tp, &tzp);
	return tp.tv_sec + tp.tv_usec * 1e-6;
}

int main(int argc, char *argv[])
{
	uint64_t tot_mem = 1ULL<<30, blk_size = 1ULL<<23, allocated = 0;
	int c, i, n_blk, n_blk_allocated = 0;
	uint8_t **blk;
	double t0;
	while ((c = getopt(argc, argv, "t:s:")) >= 0) {
		if (c == 't') tot_mem = (uint64_t)atol(optarg) << 30;
		else if (c == 's') blk_size = (uint64_t)atol(optarg) << 20;
	}
	n_blk = tot_mem / blk_size;
	t0 = realtime();
	blk = (uint8_t**)malloc(n_blk * sizeof(uint8_t*));
	for (i = 0; i < n_blk; ++i) {
		blk[i] = mmap(NULL, blk_size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
		if (blk[i] == NULL) {
			fprintf(stderr, "ERROR: failed to allocate; %lld GB allocated\n", (long long)(allocated>>20));
			break;
		}
		memset(blk[i], 0, blk_size);
		allocated += blk_size;
		++n_blk_allocated;
	}
	fprintf(stderr, "MESSAGE: allocated and filled %lld GB in %.2f seconds (%.2f%% CPU)\n", (long long)(allocated>>20),
			realtime() - t0, 100.0 * cputime() / (realtime() - t0));
	t0 = realtime();
	for (i = 0; i < n_blk_allocated; ++i)
		munmap(blk[i], blk_size);
	fprintf(stderr, "MESSAGE: deallocated %lld GB in %.2f seconds\n", (long long)(allocated>>20), realtime() - t0);
	free(blk);
	return 0;
}
