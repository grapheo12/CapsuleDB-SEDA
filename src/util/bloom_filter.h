#pragma once

/* Bloom Filter implementation.
 * Uses array of 64 bit integers as bitsets.
 * Uses MurmurHash with different seeds for different hash functions
 * Calculates optimal size and number of hash functions based on the size of input and false positive rate.
 */

#include <math.h>
#include <stdint.h>
#include "MurMurHash3.h"

#define MAX_SEEDS 10
#define MAX_FILTER_SIZE 16384

namespace BloomFilter 
{
    const uint64_t SEEDS[MAX_SEEDS] = {19340, 1231, 122, 385, 668,111,222,333,444,555};
    
    struct ParamSet {
        int input_size;
        int filter_size;                    // Number of bits = filter_size * 64
        int num_hashes;
        int __mod_mask_sft;                 // filter_size * 64 = (1 << __mod_mask_sft); used to take fast remainder
    };

    ParamSet *get_optimal_paramset(int input_size, double fp_rate);

    struct BloomFilter
    {
        ParamSet params;
        uint64_t *data;

        BloomFilter(ParamSet *p);
        uint64_t __hash_mod(char *str, int len, int seed_idx);

        void add(char *str, int len);

        bool mayBePresent(char *str, int len);
    };

}
