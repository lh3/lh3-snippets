// gcc -O2 this_prog.c -o kmer-count; ./count-k -k7 CAGTCCAGTAC GATAGACAGATGACTAGCA
#include <stdio.h>
#include "khash.h"

static inline uint64_t bfc_hash_64(uint64_t key)
{ // more sophisticated hash function to reduce collisions
    key = (~key + (key << 21)); // key = (key << 21) - key - 1;
    key = key ^ key >> 24;
    key = ((key + (key << 3)) + (key << 8)); // key * 265
    key = key ^ key >> 14;
    key = ((key + (key << 2)) + (key << 4)); // key * 21
    key = key ^ key >> 28;
    key = (key + (key << 31));
    return key;
}

KHASH_INIT(64, khint64_t, int, 1, bfc_hash_64, kh_int64_hash_equal)

unsigned char seq_nt4_table[128] = { // Table to change "ACGTN" to 01234
    4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,
    4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,
    4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,
    4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,
    4, 0, 4, 1,  4, 4, 4, 2,  4, 4, 4, 4,  4, 4, 4, 4,
    4, 4, 4, 4,  3, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,
    4, 0, 4, 1,  4, 4, 4, 2,  4, 4, 4, 4,  4, 4, 4, 4,
    4, 4, 4, 4,  3, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4
};

void count_seq(khash_t(64) *h, int k, int len, char *seq)
{
    int i, l;
    uint64_t x[2], mask = (1ULL<<k*2) - 1, shift = (k - 1) * 2;
    for (i = l = 0, x[0] = x[1] = 0; i < len; ++i) {
        int absent, c = (uint8_t)seq[i] < 128? seq_nt4_table[(uint8_t)seq[i]] : 4;
        if (c < 4) { // not an "N" base
            x[0] = (x[0] << 2 | c) & mask;                  // forward strand
            x[1] = x[1] >> 2 | (uint64_t)(3 - c) << shift;  // reverse strand
            if (++l >= k) { // we find a k-mer
                khint_t itr;
                uint64_t y = x[0] < x[1]? x[0] : x[1];
                itr = kh_put(64, h, y, &absent); // only add one strand!
                if (absent) kh_val(h, itr) = 0;
                ++kh_val(h, itr);
            }
        } else l = 0, x[0] = x[1] = 0; // if there is an "N", restart
    }
}

void print_count(const khash_t(64) *h, int k) // print all counts
{
    khint_t itr;
    char kmer[33];
    kmer[k] = 0;
    for (itr = 0; itr < kh_end(h); ++itr) {
        int i;
        if (!kh_exist(h, itr)) continue;
        for (i = 0; i < k; ++i)
            kmer[i] = "ACGT"[kh_key(h, itr) >> (i<<1) & 3];
        printf("%s\t%d\n", kmer, kh_val(h, itr));
    }
}

#include <unistd.h>
int main(int argc, char *argv[])
{
    khash_t(64) *h;
    int i, k = 5;
    while ((i = getopt(argc, argv, "k:")) >= 0)
        if (i == 'k') k = atoi(optarg);
    h = kh_init(64);
    for (i = optind; i < argc; ++i)
        count_seq(h, k, strlen(argv[i]), argv[i]);
    print_count(h, k);
    kh_destroy(64, h);
    return 0;
}
