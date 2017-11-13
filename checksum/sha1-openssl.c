#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <openssl/sha.h>

#define BUF_LEN 0x100000

int main(int argc, char *argv[])
{
	unsigned char buf[BUF_LEN];
	int l, fd;
	SHA_CTX ctx;
	unsigned char sha1[SHA_DIGEST_LENGTH];

	if (argc < 2) {
		fprintf(stderr, "Usage: sha1openssl <file>\n");
		return 1;
	}
	SHA_Init(&ctx);
	fd = open(argv[1], O_RDONLY);
	while ((l = read(fd, buf, BUF_LEN)) > 0)
		SHA1_Update(&ctx, buf, l);
	SHA1_Final(sha1, &ctx);
	for (l = 0; l < SHA_DIGEST_LENGTH; ++l)
		printf("%x", sha1[l]);
	printf("  %s\n", argv[1]);
	close(fd);
	return 0;
}
