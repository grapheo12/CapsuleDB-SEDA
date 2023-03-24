#include "queue_generator.h"

std::vector<RequestQueue *> *
generate_queue_list(int n)
{
    std::vector<RequestQueue *> * qvec
        = new std::vector<RequestQueue *>(n);
    
    for (int i = 0; i < n; i++){
        qvec->at(i) = new RequestQueue(1024);
    }

    return qvec;
}
