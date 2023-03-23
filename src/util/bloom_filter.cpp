#include "bloom_filter.h"

BloomFilter::ParamSet *BloomFilter::get_optimal_paramset(int input_size, double fp_rate)
{
    ParamSet *params = new ParamSet;
    params->input_size = input_size;

    // Ref: https://corte.si/posts/code/bloom-filter-rules-of-thumb/
    params->num_hashes = -log(fp_rate)/log(2.0);
    if (params->num_hashes > MAX_SEEDS){
        params->num_hashes = MAX_SEEDS;
    }
    
    int bits_per_elt = -log(fp_rate)/(log(2.0) * log(2.0));
    params->filter_size = ceil((double)(bits_per_elt * input_size) / 64.0);

    if (params->filter_size > MAX_FILTER_SIZE){
        params->filter_size = MAX_FILTER_SIZE;
    }

    params->__mod_mask_sft = log2(64.0 * params->filter_size);
    params->filter_size = (1 << (params->__mod_mask_sft)) / 64;

    return params;
}


BloomFilter::BloomFilter::BloomFilter(ParamSet *p)
{
    this->params.filter_size = p->filter_size;
    this->params.input_size = p->input_size;
    this->params.num_hashes = p->num_hashes;
    this->params.__mod_mask_sft = p->__mod_mask_sft;

    this->data = new uint64_t[this->params.filter_size];
    for (int i = 0; i < this->params.filter_size; i++){
        data[i] = 0;
    }
}

uint64_t BloomFilter::BloomFilter::__hash_mod(char *str, int len, int seed_idx)
{
    uint64_t hsh[2];
    MurmurHash3_x64_128((const void *)str, (const int)len, SEEDS[seed_idx], &hsh);

    // Hash mod filter_size
    // Max filter_size (in bits) is 64 * 64.
    // So __mod_mask_sft <= 12
    // Hence ignore hsh[1].
    hsh[1] = 0;
    hsh[0] = hsh[0] & ((1 << this->params.__mod_mask_sft) - 1);

    return hsh[0];
}

void BloomFilter::BloomFilter::add(char *str, int len)
{
    for (int i = 0; i < this->params.num_hashes; i++){
        
        uint64_t rem = this->__hash_mod(str, len, i);

        uint64_t idx = rem >> 6;                        // / 64
        uint64_t bit_idx = rem & ((1 << 6) - 1);        // % 64

        this->data[idx] = this->data[idx] | (1 << bit_idx);

    }
}

bool BloomFilter::BloomFilter::mayBePresent(char *str, int len)
{
    bool present = true;
    for (int i = 0; i < this->params.num_hashes; i++){
        uint64_t rem = this->__hash_mod(str, len, i);
        uint64_t idx = rem >> 6;                        // / 64
        uint64_t bit_idx = rem & ((1 << 6) - 1);        // % 64

        uint64_t res = this->data[idx] & (1 << bit_idx);
        present = present && (res > 0);
    }

    return present;
}