#ifndef __QGEN__
#define __QGEN__

#include <vector>
#include <string>
#include <arpa/inet.h>
#include "blockingconcurrentqueue.h"


struct RequestContext
{
    uint64_t uid;
    void *body;
    size_t sz;
};

typedef moodycamel::BlockingConcurrentQueue<RequestContext *> RequestQueue;


std::vector<RequestQueue *> *
generate_queue_list(int n);

#endif