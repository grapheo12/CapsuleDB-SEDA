#pragma once

#include <openssl/sha.h>
#include <cstdio>
#include <time.h>
#include <arpa/inet.h>
#include "config.h"
#include <memory>
#include <functional>
#include <string>

#define KEYMAXSIZE (ConfigMap::GetConfig()->key_maxsize)
#define VALMAXSIZE (ConfigMap::GetConfig()->val_maxsize)
#define MAXLEVEL (ConfigMap::GetConfig()->maxlevel)
#define RECORDMAXSIZE (ConfigMap::GetConfig()->record_maxsize)
#define HASHSIZE SHA256_DIGEST_LENGTH

#define ENCLAVE_WORKER_TNUM 1
#define HOST_WORKER_TNUM 1

#define HOST_RW_WORKER_TNUM 1
#define HOST_FLUSH_TNUM 1

#define USE_DC
// #define USE_TOWNCRIER

using defer = std::shared_ptr<void>;
//#define DEFER(f) shared_ptr<void> _(nullptr, bind([]{f}));


enum IO_Status {
    IO_WRITE_FAIL = 1,
    IO_WRITE_PASS = 2,
    IO_READ_FAIL = 3,
    IO_READ_PASS = 4
};


#define DEBUG_LEVEL 0

#if DEBUG_LEVEL==0
#define TIMEIT(f, msg) (f);
#else
#define TIMEIT(f, msg) {clock_t __start, __end;\
__start = clock(); (f); __end = clock();\
fprintf(stderr, "[TIMEIT] %s: %s:%d: @%lf ms: %lf us\n", (msg), __FILE__, __LINE__,\
(((double)__end) * 1e+3)/CLOCKS_PER_SEC, (((double)(__end - __start)) * 1e+6)/CLOCKS_PER_SEC);}
#endif

struct write_event_counter {
    unsigned int e_put_in_mt;
    unsigned int e_wait_flush;
};

struct read_event_counter {
    unsigned int e_wait_flush;
    unsigned int e_found_in_mt;
    unsigned int e_found_in_rec;
    unsigned int e_l0_search;
    unsigned int e_bin_search;
    unsigned int e_enclave_cache_read;
    unsigned int e_enclave_cache_hit;
    unsigned int e_enclave_cache_miss;
};

extern read_event_counter *curr_host_rec;
extern write_event_counter *curr_host_wec;

#if DEBUG_LEVEL==2
#define PRINT_RCOUNTER(counter) {\
    fprintf(stderr, "read_counter::wait_flush | %d\n", counter.e_wait_flush);\
    fprintf(stderr, "read_counter::found_in_mt | %d\n", counter.e_found_in_mt);\
    fprintf(stderr, "read_counter::found_in_rec | %d\n", counter.e_found_in_rec);\
    fprintf(stderr, "read_counter::l0_search | %d\n", counter.e_l0_search);\
    fprintf(stderr, "read_counter::bin_search | %d\n", counter.e_bin_search);\
    fprintf(stderr, "read_counter::enclave_cahce_read | %d\n", counter.e_enclave_cache_read);\
    fprintf(stderr, "read_counter::enclave_cache_hit | %d\n", counter.e_enclave_cache_hit);\
    fprintf(stderr, "read_counter::enclave_cache_miss | %d\n", counter.e_enclave_cache_miss);}
#define PRINT_WCOUNTER(counter) {\
    fprintf(stderr, "write_counter::put_in_mt | %d\n", counter.e_put_in_mt);\
    fprintf(stderr, "write_counter::wait_flush | %d\n", counter.e_wait_flush);}
#define INC_COUNTER(counter, field) counter.field++;
#define INCP_COUNTER(counterp, field) {if (counterp) counterp->field++;}
#else
#define PRINT_RCOUNTER(counter) {}
#define PRINT_WCOUNTER(counter) {}
#define INC_COUNTER(counter, field) {}
#define INCP_COUNTER(counterp, field) {}
#endif
