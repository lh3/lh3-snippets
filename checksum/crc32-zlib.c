#include <zlib.h>
#include <fcntl.h>
#include <stdio.h>

#define BUF_LEN 0x100000

int main(int argc, char *argv[])
{
	uLong crc = crc32(0L, Z_NULL, 0);
	unsigned char buf[BUF_LEN];
	int l, fd;

	if (argc < 2) {
		fprintf(stderr, "Usage: crc32-zlib <file>\n");
		return 1;
	}
	fd = open(argv[1], O_RDONLY);
	while ((l = read(fd, buf, BUF_LEN)) > 0)
		crc = crc32(crc, buf, l);
	printf("%lx  %s\n", (long)crc, argv[1]);
	close(fd);
	return 0;
}
