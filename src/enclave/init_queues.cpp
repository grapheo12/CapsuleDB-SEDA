#include "cdb_t.h"
#include "queue_generator.h"
#include <vector>

std::vector<RequestQueue *> *global_enclave_queues;

void enclave_generate_queues(int n)
{
    global_enclave_queues = generate_queue_list(n);
}