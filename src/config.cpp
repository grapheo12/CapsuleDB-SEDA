#include "config.h"
#include "util/INIReader.h"
#include <cstdlib>
#include <iostream>

ConfigMap *ConfigMap::__c = NULL;

ConfigMap::ConfigMap()
{
    c = new Config;
}

ConfigMap::~ConfigMap()
{
    delete c;
}

Config *ConfigMap::GetConfig()
{
    if (!__c){
        return NULL;
    }

    return __c->c;
}

void ConfigMap::ReadFromFile(const char *path)
{
    INIReader reader( (std::string(path)) );
    if (reader.ParseError() != 0){
        std::cerr << "Config parse error" << std::endl;
        exit(1);
    }

    if (!__c){
        __c = new ConfigMap();
    }

    __c->c->ecdsa_priv_key = reader.Get("keys", "ecdsa_priv_key", "enclave_sign_priv.pem");
    __c->c->aes_key = reader.Get("keys", "aes_key", "enclave_enc.key");

    __c->c->dc_client_id = reader.GetInteger("datacapsule", "dc_client_id", 1);
    __c->c->dc_server_addr = reader.Get("datacapsule", "dc_server_addr", "127.0.0.1");

    __c->c->record_maxsize = reader.GetInteger("lsm", "record_maxsize", 1024*1024*3);
    __c->c->memtable_size = reader.GetInteger("lsm", "memtable_size", 2048);
    __c->c->merkle_groupsize = reader.GetInteger("lsm", "merkle_groupsize", 16);
    __c->c->enclave_cachesize = reader.GetInteger("lsm", "enclave_cachesize", 15);
    __c->c->key_maxsize = reader.GetInteger("lsm", "key_maxsize", 100);
    __c->c->val_maxsize = reader.GetInteger("lsm", "val_maxsize", 1000);
    __c->c->maxlevel = reader.GetInteger("lsm", "maxlevel", 2);

    __c->c->ocec_skip_length = reader.GetInteger("compaction", "skip_length", 2);
    __c->c->ocec_max_height = reader.GetInteger("compaction", "max_height", 2);

    __c->c->tc_name = reader.Get("towncrier", "name", "cdb");
    __c->c->tc_addr = reader.Get("towncrier", "addr", "127.0.0.1:2501");
}

void ConfigMap::DestroyConfig()
{
    if (__c) delete __c;
    __c = NULL;
}

void ConfigMap::LoadConfig(Config *cc)
{
    if (!__c){
        __c = new ConfigMap();
    }

    __c->c = cc;

}