#pragma once

#include <string>

struct Config {
    uint64_t key_maxsize;
    uint64_t val_maxsize;
    uint64_t maxlevel;

    size_t record_maxsize;
    uint64_t memtable_size;
    uint64_t merkle_groupsize;
    uint64_t enclave_cachesize;

    uint64_t dc_client_id;
    std::string dc_server_addr;

    std::string ecdsa_priv_key;
    std::string aes_key;

    long ocec_skip_length;
    long ocec_max_height;

    std::string tc_name;
    std::string tc_addr;
};

class ConfigMap {
    private:
        static ConfigMap *__c;
        Config *c;

        ConfigMap();
        ~ConfigMap();

    public:
        ConfigMap(const ConfigMap &) = delete;
        void operator=(const ConfigMap &) = delete;

        static void ReadFromFile(const char *path);
        static void LoadConfig(Config *c);
        static Config *GetConfig();
        static void DestroyConfig();
};

/** Config.ini File Format
 * 
 * [keys]
 * ecdsa_priv_key = 
 * aes_key = 
 * 
 * [datacapsule]
 * dc_client_id =
 * dc_server_addr = 
 * 
 * [lsm]
 * record_maxsize =
 * memtable_size = 
 * merkle_groupsize =
 * enclave_cachesize =
 * key_maxsize =
 * val_maxsize =
 * maxlevel =
 * 
 * [compaction]
 * skip_length =
 * max_height =
 * 
 * [towncrier]
 * name =
 * addr =
 * 
 * 
*/