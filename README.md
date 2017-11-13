As its name implies, this repo is a collection of code snippets. Most were
written by me; a few were copied from online resources. I used to keep these
snippets in gist or a private notebook, but found it is clumsy to manage them
that way, so I decided to move them here.

```
Standalone targets
==================

Algorithms:

  all-mss: find all max-scoring segments (all-mss.c)
  lis: longest increasing subsequences (lis.c)
  arr-merge: merge two sorted arrays (arr-merge.c)
  arr-shuf: shuffle an array (arr-shuf.c)
  kmer-count: count k-mer in DNA sequences (kmer-cnt.c)
  res-sample: reservior sampling (res-sample.c)
  inv-inthash: invertible integer hash functions (inv-inthash.c)
  checksum/sha1-gs: compute SHA1 checksum from GhostScript (sha1-gs.c)
  checksum/sha1-oauth: compute SHA1 checksum from OAuth (sha1-oauth.c)
  bwt/bwt-std: standard algorithm to construct BWT (bwt-std.c)
  bwt/bwt-bcr1: construct BWT for one string with BCR (bwt-bcr1.c)
  bwt/bwt-bcr: construct BWT for multiple strings with BCR (bwt-bcr.c)

Numerical:

  eigen-test: eigenvalues for symmetric and asymmetric matrices (eigen.c; not by me)

System:

  getopt-test: test standalone getopt_long() from musl (x86-simd-test.c; not by me)
  x86-simd-test: test supported SIMD instruction sets (getopt.{c,h} getopt-test.c)

For fun:

  mastermind: the MasterMind game; computer as code breaker (mastermind.c)


Dependency required
===================

Algorithms:

  checksum/crc32-zlib: compute CRC32 with zlib (crc32-zlib.c)
  checksum/sha1-openssl: compute SHA1 with OpenSSL (sha1-openssl.c)
```
