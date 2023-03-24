#ifndef __QGEN__
#define __QGEN__

#include <vector>
#include <string>
#include <arpa/inet.h>
#include "atomic_queue/atomic_queue.h"
#include <climits>


struct RequestContext
{
    uint64_t uid;
    void *body;
    size_t sz;
};

using Element = RequestContext *;
using RequestQueue = atomic_queue::AtomicQueueB2<Element, std::allocator<Element>, true, false, true>;


std::vector<RequestQueue *> *
generate_queue_list(int n);

#endif