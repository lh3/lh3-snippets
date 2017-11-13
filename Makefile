define TARGETS_BODY

===> List of targets <===

getopt-test: test standalone getopt_long() from musl (x86-simd-test.c)
inv-inthash: invertible integer hash functions (inv-inthash.c)
mastermind: the MasterMind game; computer as code breaker (mastermind.c)
x86-simd-test: test supported SIMD instruction sets (getopt.{c,h} getopt-test.c)

endef
export TARGETS_BODY

EXE=getopt-test mastermind x86-simd-test inv-inthash

all:list compile

list:
	@echo "$$TARGETS_BODY"

compile:$(EXE)

x86-simd-test:x86-simd-test.c
	$(CC) -O2 -Wall -o $@ $<

getopt-test:getopt-test.c getopt.c getopt.h
	$(CC) -O2 -Wall -o $@ getopt.c getopt-test.c

mastermind:mastermind.c
	$(CC) -O2 -Wall -o $@ $<

inv-inthash:inv-inthash.c
	$(CC) -O2 -Wall -o $@ $<

clean:
	rm -fr *.dSYM *.o *.a $(EXE)
