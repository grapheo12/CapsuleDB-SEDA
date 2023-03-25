#pragma once
#include <atomic>
#include <arpa/inet.h>
#include "../util/queue_generator.h"
#include "../config.h"
#include "../common.h"
#include <openenclave/host.h>

struct __replyAddr
{
    char replyaddr[INET_ADDRSTRLEN];
    uint16_t replyport;
};

void init_net_rx(Config *cfg, std::atomic<bool>& end_signal, uint16_t port, RequestQueue *rx_q);
void init_net_tx(oe_enclave_t *enclave, Config *cfg, std::atomic<bool>& end_signal, RequestQueue *tx_q);