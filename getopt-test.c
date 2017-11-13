#include <stdio.h>
#include "getopt.h"

static struct option long_options[] = {
	{ "foo", no_argument,       0, 'f' },
	{ "bar", required_argument, 0, 'b' },
	{ "opt", optional_argument, 0, 'o' },
	{ 0, 0, 0, 0}
};

int main(int argc, char *argv[])
{
	int i, c, option_index;
	printf("Options:\n");
	while ((c = getopt_long(argc, argv, "fb:o", long_options, &option_index)) >= 0) {
		if (c == 'f') printf("  -f\n");
		else if (c == 'b') printf("  -b %s\n", optarg);
		else if (c == 'o') {
			if (optarg) printf("  -o %s\n", optarg);
			else printf(" -o\n");
		}
	}
	printf("Arguments:\n");
	for (i = optind; i < argc; ++i)
		printf("  %s\n", argv[i]);
	return 0;
}
