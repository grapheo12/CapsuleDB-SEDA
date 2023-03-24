#include <iostream>
#include <unordered_map>
#include <string>
#include <stdlib.h>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "connection_handler.h"
#include "net_request.pb.h"

using namespace net_request;
static std::atomic<uint64_t> uid_cnt;
static std::unordered_map<uint64_t, __replyAddr> addr_map;

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
    uid_cnt.store(0);

    size_t max_sz = cfg->key_maxsize + cfg->val_maxsize + 100;
    char *msg_buff = new char[max_sz];
    sockaddr_in cliaddr;
    socklen_t cliaddr_len = sizeof(cliaddr);

    while (!end_signal.load()){
        ssize_t n = recvfrom(sockfd, msg_buff, max_sz, MSG_WAITALL, (sockaddr *)&cliaddr, &cliaddr_len);
        std::cout << "Recvd msg: " << msg_buff << " Size: " << n << std::endl;
        NetRequest req;
        req.ParseFromString(std::string(msg_buff, n));

        std::cout << "Received: " << req.body() << " Size: " << req.sz() << std::endl;

        RequestContext *ctx = new RequestContext;
        ctx->uid = uid_cnt.fetch_add(1, std::memory_order_seq_cst);            // Sequentially consistent

        __replyAddr replyaddr;
        memcpy(replyaddr.replyaddr, req.replyaddr().c_str(), INET_ADDRSTRLEN);
        replyaddr.replyport = (uint16_t)req.replyport();
        addr_map[ctx->uid] = replyaddr;
        ctx->body = malloc(req.sz() * sizeof(char));
        memcpy(ctx->body, req.body().c_str(), req.sz());
        ctx->sz = req.sz();

        std::cout << "ctx->body: " << (char *)ctx->body << std::endl;

        // ctx will be deleted on the consumer side
        rx_q->push(ctx);
    }

    std::cout << "init_net_rx: Ending" << std::endl;
    close(sockfd);
    delete[] msg_buff;

    return;

}

void init_net_tx(Config *cfg, std::atomic<bool>& end_signal, RequestQueue *tx_q)
{
    std::cout << "init_net_tx: Starting transmission" << std::endl;

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0){
        perror("init_net_tx: Socket creation failed. Dying");
        return;
    }

    while (!end_signal.load()){
        RequestContext *ctx = tx_q->pop();

        auto it = addr_map.find(ctx->uid);
        if (it == addr_map.end()){
            std::cerr << "init_net_tx: Invalid UID" << std::endl;
            continue;
        }

        sockaddr_in sa;
        memset(&sa, 0, sizeof(sa));
        std::cout << "Replying to: " << it->second.replyaddr << ":" << it->second.replyport << std::endl;
        inet_pton(AF_INET, it->second.replyaddr, &(sa.sin_addr));
        sa.sin_port = htons(it->second.replyport);
        sa.sin_family = AF_INET;


        std::cout << "Sent bytes: " << 
        sendto(sockfd, (char *)ctx->body, ctx->sz, MSG_CONFIRM,
            (sockaddr *)&sa, sizeof(sa)) << std::endl;

        free(ctx->body);
        delete ctx;
        addr_map.erase(ctx->uid);
    }

    std::cout << "init_net_tx: Ending" << std::endl;
    close(sockfd);
    return;
}

