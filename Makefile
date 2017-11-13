define TARGETS_BODY

===> Standalone targets <===

getopt-test: test standalone getopt_long() from musl (x86-simd-test.c)
eigen-test: eigenvalues for symmetric and non-symmetric matrices (eigen.c)
inv-inthash: invertible integer hash functions (inv-inthash.c)
mastermind: the MasterMind game; computer as code breaker (mastermind.c)
x86-simd-test: test supported SIMD instruction sets (getopt.{c,h} getopt-test.c)

checksum/sha1-gs: compute SHA1 checksum from GhostScript (sha1-gs.c)
checksum/sha1-oauth: compute SHA1 checksum from OAuth (sha1-oauth.c)

endef
export TARGETS_BODY

STANDALONE=getopt-test mastermind x86-simd-test inv-inthash eigen-test \
	checksum/sha1-gs checksum/sha1-oauth

DEPENDENT=checksum/crc32-zlib checksum/sha1-openssl

all:list standalone

list:
	@echo "$$TARGETS_BODY"

standalone:$(STANDALONE)

dependent:$(DEPENDENT)

x86-simd-test:x86-simd-test.c
	$(CC) -O2 -Wall -o $@ $<

getopt-test:getopt-test.c getopt.c getopt.h
	$(CC) -O2 -Wall -o $@ getopt.c getopt-test.c

eigen-test:eigen.c
	$(CC) -O2 -Wall -DLH3_EIGEN_MAIN -o $@ $<

mastermind:mastermind.c
	$(CC) -O2 -Wall -o $@ $<

inv-inthash:inv-inthash.c
	$(CC) -O2 -Wall -o $@ $<

checksum/sha1-gs:checksum/sha1-gs.c
	$(CC) -O2 -Wall -o $@ $<

checksum/sha1-oauth:checksum/sha1-oauth.c
	$(CC) -O2 -Wall -o $@ $<

checksum/crc32-zlib:checksum/crc32-zlib.c
	$(CC) -O2 -Wall -o $@ $< -lz

clean:
	rm -fr *.dSYM *.o *.a $(STANDALONE) $(DEPENDENT)
