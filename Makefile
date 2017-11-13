list:
	@echo 'x86-simd-test: test supported SIMD instruction sets'

x86-simd-test:x86-simd-test.c
	$(CC) -O2 -Wall -o $@ $<

clean:
	rm -fr *.dSYM x86-simd-test 
