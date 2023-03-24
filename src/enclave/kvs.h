#include "memtable.h"
// #include "persistent.h"
#include "cdb_args.h"
#include <atomic>
#include <deque>

#define MT_NUM 2

/**
 * An wrapper buffer that indicates "buf" is in host-side memory.
 * Original location: persistent.h @todo: Move it
*/
struct host_buf {
    char *buf_data;
    char *buf_meta;
    size_t buf_data_size;
    size_t buf_meta_size;
};

class KVStore {
public:
    KVStore(uint64_t mt_max_size);
    ~KVStore();
    KV_Status Put(const std::string &key, const std::string &value);
    KV_Status Get(const std::string &key, std::string &value);
    
    /**
     * buf (record buf) cannot be wrapped with string because it's pointing host buffer.
    */  
    std::pair<size_t, size_t> InitFlush(
        std::string &data_hash_str,
        std::string &meta_hash_str,
        host_buf &hbuf);
    bool EndFlush(
        const std::string &hash_str
    );
    void ForceReadyToDie();
    bool IsReadyToDie();

    bool Load(const std::string &root_hash);
    std::atomic<bool> *flush_sema;
private:
    std::atomic<int> cur_mt_;
    std::deque<int> usage_q;
    int flush_target_;
    MemTable* mt_[MT_NUM];
    // PersistentStore *ps_;
};
