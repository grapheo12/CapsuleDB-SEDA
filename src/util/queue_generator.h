#ifndef __QGEN__
#define __QGEN__

#include <vector>
#include <string>
#include <arpa/inet.h>
#include "atomic_queue/atomic_queue.h"
#include <climits>


struct RequestContext
{
    char reply_addr[INET_ADDRSTRLEN];
    uint16_t reply_port;
    char *original_body;
    void *body;
    size_t sz;
    uint64_t client_seqno;
};

using Element = RequestContext *;
using RequestQueue = atomic_queue::AtomicQueueB2<Element, std::allocator<Element>, true, false, true>;


std::vector<RequestQueue *> *
generate_queue_list(int n);

#endif