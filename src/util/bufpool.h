#include <atomic>

/* Dumb ring buffer */
class BufPool {
public:
    BufPool(
        size_t chunk_size,
        size_t chunk_cnt
    ): chunk_size_(chunk_size), chunk_cnt_(chunk_cnt)
    {
        buf_pool = new char*[chunk_cnt_];
        for(int i = 0; i < chunk_cnt_; i++)
            buf_pool[i] = new char[chunk_size_];
        cursor_ = 0;
    }
    ~BufPool() {
        for(int i = 0; i < chunk_cnt_; i++)
            delete[] buf_pool[i];
        delete[] buf_pool; 
    }
    char *GetBuf() { return buf_pool[(cursor_++) % chunk_cnt_]; }

private:
    char **buf_pool;
    const size_t chunk_size_;        
    const size_t chunk_cnt_;
    std::atomic<int> cursor_;
};