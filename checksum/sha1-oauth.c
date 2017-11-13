/* lh3 note: it is slow */

/* This code is public-domain - it is based on libcrypt placed in the public domain by Wei Dai and other contributors. */

#include <string.h>
#include <stdint.h>

#define HASH_LENGTH 20
#define BLOCK_LENGTH 64

typedef struct sha1nfo {
	union { uint8_t b[BLOCK_LENGTH]; uint32_t w[BLOCK_LENGTH/4]; } buf;
	uint8_t bufOffset;
	union { uint8_t b[HASH_LENGTH]; uint32_t w[HASH_LENGTH/4]; } state;
	uint32_t byteCount;
	uint8_t keyBuffer[BLOCK_LENGTH];
	uint8_t innerHash[HASH_LENGTH];
} sha1nfo;

void sha1_init(sha1nfo *s)
{
	const uint8_t table[] = { 0x01,0x23,0x45,0x67, 0x89,0xab,0xcd,0xef, 0xfe,0xdc,0xba,0x98, 0x76,0x54,0x32,0x10, 0xf0,0xe1,0xd2,0xc3 };
	memcpy(s->state.b, table, HASH_LENGTH);
	s->byteCount = 0;
	s->bufOffset = 0;
}

#define rol32(value, bits) (((value) << (bits)) | ((value) >> (32 - (bits))))

static void sha1_hashBlock(sha1nfo *s)
{
	uint32_t i, t, a = s->state.w[0], b = s->state.w[1], c = s->state.w[2], d = s->state.w[3], e = s->state.w[4];
	for (i = 0; i < 80; i++) {
		if (i >= 16) {
			t = s->buf.w[(i+13)&15] ^ s->buf.w[(i+8)&15] ^ s->buf.w[(i+2)&15] ^ s->buf.w[i&15];
			s->buf.w[i&15] = rol32(t, 1);
		}
		if (i < 20)      t = 0x5a827999 + (d ^ (b & (c ^ d)));
		else if (i < 40) t = 0x6ed9eba1 + (b ^ c ^ d);
		else if (i < 60) t = 0x8f1bbcdc + ((b & c) | (d & (b | c)));
		else             t = 0xca62c1d6 + (b ^ c ^ d);
		t += rol32(a, 5) + e + s->buf.w[i&15];
		e = d; d = c; c = rol32(b, 30); b = a; a = t;
	}
	s->state.w[0] += a; s->state.w[1] += b; s->state.w[2] += c; s->state.w[3] += d; s->state.w[4] += e;
}

static inline void sha1_add(sha1nfo *s, uint8_t data)
{
	s->buf.b[s->bufOffset ^ 3] = data;
	if (++s->bufOffset == BLOCK_LENGTH) {
		sha1_hashBlock(s);
		s->bufOffset = 0;
	}
}

inline void sha1_write1(sha1nfo *s, uint8_t data)
{
	++s->byteCount;
	sha1_add(s, data);
}

inline void sha1_write(sha1nfo *s, const char *data, size_t len)
{
	while (len--) sha1_write1(s, (uint8_t)*data++);
}

const uint8_t *sha1_final(sha1nfo *s)
{
	int i;
	sha1_add(s, 0x80);
	while (s->bufOffset != 56) sha1_add(s, 0);
	sha1_add(s, 0);
	sha1_add(s, 0);
	sha1_add(s, 0);
	sha1_add(s, s->byteCount >> 29);
	sha1_add(s, s->byteCount >> 21);
	sha1_add(s, s->byteCount >> 13);
	sha1_add(s, s->byteCount >> 5);
	sha1_add(s, s->byteCount << 3);
	for (i = 0; i < 5; ++i) {
		uint32_t a = s->state.w[i];
		s->state.w[i] = a<<24 | (a<<8&0x00ff0000) | (a>>8&0x0000ff00) | a>>24;
	}
	return s->state.b;
}

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

#define BUF_LEN 0x100000

int main(int argc, char *argv[])
{
	char buf[BUF_LEN];
	int l, fd;
	sha1nfo s;
	const uint8_t *sha1;

	if (argc < 2) {
		fprintf(stderr, "Usage: sha1oauth <file>\n");
		return 1;
	}
	sha1_init(&s);
	fd = open(argv[1], O_RDONLY);
	while ((l = read(fd, buf, BUF_LEN)) > 0)
		sha1_write(&s, buf, l);
	sha1 = sha1_final(&s);
	for (l = 0; l < HASH_LENGTH; ++l)
		printf("%x", sha1[l]);
	printf("  %s\n", argv[1]);
	close(fd);
	return 0;
}
