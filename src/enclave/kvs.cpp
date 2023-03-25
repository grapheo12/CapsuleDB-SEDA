#include "kvs.h"
// #include "local.h"
// #include "dc.h"
#include "cdb_t.h"
#include "../common.h"
#include <chrono>
#include <thread>
#include <iostream>
#include <cassert>

KVStore::KVStore(uint64_t mt_max_sz):
    cur_mt_(0),
    flush_target_(-1)
{

#ifdef USE_DC
    // ps_ = new DCStore();
#else
    ps_ = new LocalDiskStore();
#endif

    usage_q.push_back(0);
    // All memtables are empty at this point.
    // So all are mutable.
    for (int i = 0; i < MT_NUM; i++){
        mt_[i] = new MemTable(mt_max_sz);
        mt_[i]->SetMutable(true);
    }
}

KVStore::~KVStore()
{
    for (int i = 0; i < MT_NUM; i++){
        delete mt_[i];
    }
}

KV_Status KVStore::Put(const std::string &key, const std::string &value)
{
    if (!mt_[cur_mt_]->IsMutable()) {
        // This memtable is full, let's switch to one that has some space.
        
        bool __found_empty = false;
        for (int i = 0; i < MT_NUM; i++){
            if (mt_[i]->IsMutable()){
                cur_mt_ = i;
                usage_q.push_back(i);
                while (usage_q.size() > MT_NUM){
                    usage_q.pop_front();
                }
                __found_empty = true;
                break;
            }
        }

        if (!__found_empty){
            // Tell the host to wait for a flush operation.
            *flush_sema = true;
            return WAIT_FLUSH;
        }
    }

    // curr_host_wec->e_put_in_mt++;
	auto ret = mt_[cur_mt_]->Put(key, value);
    if (ret == JUST_FULL) {
        *flush_sema = true;
        ret = WRITE_PASS;
    }

    return ret;
}

KV_Status KVStore::Get(const std::string &key, std::string &value) {
	//std::cerr << "KVStore::Get" << std::endl;
    KV_Status ret = READ_PASS;

    /* search current MemTable and Immutable MemTables in turn. */ 
    for (int i = usage_q.size() - 1; i >= 0; i--) {
        int idx = usage_q[i];
        ret = mt_[ idx % MT_NUM ]->Get(key, value);
        if (ret == READ_PASS) {
            INCP_COUNTER(curr_host_rec, e_found_in_mt)
            break;
        }
    }

    // Moved to memtable_stage.cpp;
    // if (ret == READ_FAIL) {
    //     ret = ps_->Get(key, value);
    // }

    return ret;
}


/* assuming single flush thread */
std::pair<size_t, size_t> KVStore::InitFlush(
    std::string &data_hash_str,
    std::string &meta_hash_str,
    host_buf &hbuf
) 
{
    // First, check if Immutable MemTable to flush (check sz)
    // Then, call record creation func of underlying PS. 
    // last. Release Immutable Memtable from memory. 
    std::pair<size_t, size_t> buf_used(0, 0);

    if (flush_target_ < 0) {
        for (int i = 0; i < MT_NUM; i++) {
			//std::cerr << "InitFlush search" << std::endl;
            if (mt_[i]->InitFlush()) {
                flush_target_ = i;
                break;
            }
        } 
    }

    if (flush_target_ >= 0) {
        auto raw_kv = mt_[flush_target_]->Dump();
        // buf_used = ps_->CreateNewRecord(raw_kv, data_hash_str, meta_hash_str, hbuf);
        // retry later if failed
    }  

    return buf_used; 
}

bool KVStore::EndFlush(
    const std::string &hash_str
)
{
    // ps_->FixRecord(hash_str);
    mt_[flush_target_]->EndFlush();
	flush_target_ = -1;

    *flush_sema = false;
    for (int i = 0; i < MT_NUM; i++) {
        if (!mt_[i]->IsMutable()) *flush_sema = true;
    }

	return true;
}

void KVStore::ForceReadyToDie()
{
    for (int i = 0; i < MT_NUM; i++)
        mt_[i]->SetMutable(false);
    //*flush_sema = true;
}

bool KVStore::IsReadyToDie() {
    for (int i = 0; i < MT_NUM; i++)
        if (!mt_[i]->IsEmpty())
            return false;

    return true;
}


bool KVStore::Load(const std::string &root_hash) {
    return false;
    // return ps_->LoadIndex(root_hash);
}
