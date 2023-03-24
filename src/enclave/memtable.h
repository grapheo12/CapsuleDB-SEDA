#pragma once

#include <map>
#include <unordered_map>
#include <string>
#include <vector>
#include "../common.h"
#include "../util/MurMurHash3.h"
#include "../config.h"
#include "cdb_args.h"
#include <atomic>


// #define MT_TH (ConfigMap::GetConfig()->memtable_size)

#define STRING_MURMURHASH_SEED 12413
struct __StringHasher
{
    std::size_t operator()(const std::string &s) const
    {
        uint64_t hsh[2];
        MurmurHash3_x64_128((const void *)s.c_str(), s.size(), STRING_MURMURHASH_SEED, hsh);
        return (uint64_t)(hsh[0] ^ hsh[1]);
    }
};


class MemTable {
public:
    MemTable(uint64_t max_sz);
    // ~MemTable();
    KV_Status Put(const std::string &key, const std::string &value);
    KV_Status Get(const std::string &key, std::string &value);

    std::vector<std::pair<std::string, std::string>> Dump();
    void SetMutable(bool mut);
    bool InitFlush();
    bool EndFlush();
    
    bool IsEmpty() const;
	bool IsMutable() const;
private:   
    
    bool isFull() const;

	// std::map<std::string, std::string> store_;
	std::unordered_map<std::string, std::string, __StringHasher> store_;
    uint64_t size_in_bytes_;
    uint64_t max_size_in_bytes_;
    std::atomic<bool> mutable_; // mutable only when size is less than threshold
    std::atomic<bool> on_flush_;
};
