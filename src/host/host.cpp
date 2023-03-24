#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include "../util/queue_generator.h"
#include "connection_handler.h"

static std::atomic<bool> end_signal;

void short_circuit(Config *cfg, std::atomic<bool>& end_signal, RequestQueue *in_q, RequestQueue *out_q)
{
    while (!end_signal.load()){
        RequestContext *ctx = in_q->pop();
        std::cout << "Replying to " << ctx->uid << " " << ctx->body << std::endl;
        // Short Circuit
        out_q->push(ctx);
    }
}

int main(int argc, char *argv[])
{
    auto qvec = generate_queue_list(2);
    end_signal.store(false);
    Config cfg;
    cfg.key_maxsize = 100;
    cfg.val_maxsize = 1000;

    auto rx_thread = std::thread(init_net_rx, &cfg, std::ref(end_signal), (uint16_t)3001, qvec->at(0));
    auto tx_thread = std::thread(init_net_tx, &cfg, std::ref(end_signal), qvec->at(1));
    auto proc_thread = std::thread(short_circuit, &cfg, std::ref(end_signal), qvec->at(0), qvec->at(1));

    rx_thread.join();
    tx_thread.join();
    proc_thread.join();
    return 0;
}