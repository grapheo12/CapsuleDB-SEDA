#include <iostream>
#include <unordered_map>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <openenclave/host.h>
#include "cdb_u.h"
#include "connection_handler.h"
#include "net_request.pb.h"

using namespace net_request;


void init_net_rx(Config *cfg, std::atomic<bool>& end_signal, uint16_t port, RequestQueue *rx_q)
{
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0){
        perror("init_net_rx: Socket creation failed");
        return;
    }

    sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(port);

    if (bind(sockfd, (const sockaddr *)&servaddr, sizeof(servaddr)) < 0){
        perror("init_net_rx: Bind failed");
        return;
    }

    std::cout << "init_net_rx: Starting listening for request on port: " << port << std::endl;

    size_t max_sz = cfg->key_maxsize + cfg->val_maxsize + 100;
    char *msg_buff = new char[max_sz];
    sockaddr_in cliaddr;
    socklen_t cliaddr_len = sizeof(cliaddr);

    while (!end_signal.load()){
        ssize_t n = recvfrom(sockfd, msg_buff, max_sz, MSG_WAITALL, (sockaddr *)&cliaddr, &cliaddr_len);
        NetRequest req;
        if (!req.ParseFromString(std::string(msg_buff, n))){
            continue;
        }

        RequestContext *ctx = new RequestContext;
        ctx->client_seqno = req.client_seqno();

        memcpy(ctx->reply_addr, req.replyaddr().c_str(), INET_ADDRSTRLEN);
        ctx->reply_port = (uint16_t)req.replyport();

        size_t __sz = req.sz();
        if (__sz < cfg->key_maxsize + cfg->val_maxsize + 10){
            __sz = cfg->key_maxsize + cfg->val_maxsize + 10;
        }
        ctx->original_body = (char *)malloc(__sz * sizeof(char));
        memcpy(ctx->original_body, req.body().c_str(), req.sz());
        ctx->sz = req.sz();

        // ctx will be deleted on the consumer side
        rx_q->push(ctx);
    }

    std::cout << "init_net_rx: Ending" << std::endl;
    close(sockfd);
    delete[] msg_buff;

    return;

}

void init_net_tx(oe_enclave_t *enclave, Config *cfg, std::atomic<bool>& end_signal, RequestQueue *tx_q)
{
    std::cout << "init_net_tx: Starting transmission" << std::endl;

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0){
        perror("init_net_tx: Socket creation failed. Dying");
        return;
    }

    while (!end_signal.load()){
        RequestContext *ctx = tx_q->pop();
        sockaddr_in sa;
        memset(&sa, 0, sizeof(sa));
        inet_pton(AF_INET, ctx->reply_addr, &(sa.sin_addr));
        sa.sin_port = htons(ctx->reply_port);
        sa.sin_family = AF_INET;

        std::stringstream ss;
        ss << ctx->client_seqno << "|" << std::string((char *)ctx->original_body, ctx->sz);
        std::string sbody = ss.str();

        sendto(sockfd, sbody.c_str(), sbody.size(), MSG_CONFIRM,
            (sockaddr *)&sa, sizeof(sa));


        // enclave_gc(enclave, ctx->body);
        free(ctx->original_body);
        delete ctx;
    }

    std::cout << "init_net_tx: Ending" << std::endl;
    close(sockfd);
    return;
}

