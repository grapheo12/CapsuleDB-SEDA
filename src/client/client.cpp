#include <iostream>
#include <string>
#include <stdlib.h>
#include <thread>
#include <atomic>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unordered_map>
#include <functional>
#include <sstream>
#include "../common.h"
#include "../util/blockingconcurrentqueue.h"
#include "net_request.pb.h"
using namespace net_request;

#define PRINT_ERROR_MESSAGE(cond) if ((cond)) { std::cerr << "Usage: ./client remote_addr:port my_addr:port (Non-sudo so port > 1000)" << std::endl; exit(0); }


struct __replyAddr
{
    char replyaddr[INET_ADDRSTRLEN];
    uint16_t replyport;
};
static std::atomic<uint64_t> uid_cnt;
static std::unordered_map<uint64_t, std::function<void(std::string)>> callback_map;

void reply_to_caller(char *msg_buff, size_t n)
{
    // Replace with own parsing function and decryption etc
    uint64_t uid = (uint64_t)atoll(msg_buff);
    auto it = callback_map.find(uid);
    if (it == callback_map.end())
        return;

    it->second(std::string(msg_buff, n));
    callback_map.erase(uid);
}

void init_net_rx(Config *cfg, std::atomic<bool>& end_signal, uint16_t port)
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
        std::cout << "Message recv: " << n << std::endl;
        reply_to_caller(msg_buff, n);
    }

    std::cout << "init_net_rx: Ending" << std::endl;
    close(sockfd);
    delete[] msg_buff;

    return;

}

struct ClientRequest
{
    std::string body;
    std::function<void(std::string)> callback;
};

typedef moodycamel::BlockingConcurrentQueue<ClientRequest> ClientRequestQueue;

void init_net_tx(Config *cfg, std::atomic<bool>& end_signal, ClientRequestQueue *cq, char *remote_addr, uint16_t remote_port, char *my_addr, uint16_t my_port)
{
    std::cout << "init_net_tx: Starting transmission" << std::endl;

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0){
        perror("init_net_tx: Socket creation failed. Dying");
        return;
    }
    sockaddr_in sa;        
    memset(&sa, 0, sizeof(sa));
    inet_pton(AF_INET, remote_addr, &(sa.sin_addr));
    sa.sin_port = htons(remote_port);
    sa.sin_family = AF_INET;


    while (!end_signal.load()){
        ClientRequest req;
        cq->wait_dequeue(req);

        uint64_t uid = uid_cnt.fetch_add(1, std::memory_order_seq_cst);
        callback_map[uid] = req.callback;

        sockaddr_in sa;
        inet_pton(AF_INET, remote_addr, &(sa.sin_addr));
        sa.sin_port = htons(remote_port);
        sa.sin_family = AF_INET;

        NetRequest nr;
        std::stringstream ss;
        ss << uid << "|" << req.body;
        nr.set_body(ss.str());
        nr.set_sz(req.body.size());
        nr.set_replyaddr(std::string(my_addr, INET_ADDRSTRLEN));
        nr.set_replyport((uint32_t)my_port);

        std::string serial = nr.SerializeAsString();

        std::cout << "Sent bytes: " << sendto(sockfd, serial.c_str(), serial.size(), MSG_CONFIRM,
            (sockaddr *)&sa, sizeof(sa)) << std::endl;

    }

    std::cout << "init_net_tx: Ending" << std::endl;
    close(sockfd);
    return;
}


static std::atomic<bool> end_signal;

int main(int argc, char *argv[])
{
    PRINT_ERROR_MESSAGE(argc != 3)

    char *remote_addr_port = argv[1];
    char *my_addr_port = argv[2];

    char *my_addr = strtok(my_addr_port, ":");
    PRINT_ERROR_MESSAGE(!my_addr)
    char *my_port_str = strtok(NULL, ":");
    PRINT_ERROR_MESSAGE(!my_port_str)
    int my_port = atoi(my_port_str);
    PRINT_ERROR_MESSAGE(my_port <= 1000)

    char *remote_addr = strtok(remote_addr_port, ":");
    PRINT_ERROR_MESSAGE(!remote_addr)
    char *remote_port_str = strtok(NULL, ":");
    PRINT_ERROR_MESSAGE(!remote_port_str)
    int remote_port = atoi(remote_port_str);
    PRINT_ERROR_MESSAGE(remote_port <= 1000)

    end_signal.store(false);
    
    Config cfg;
    cfg.key_maxsize = 100;
    cfg.val_maxsize = 1000;

    ClientRequestQueue cq;

    auto rx_thread = std::thread(init_net_rx, &cfg, std::ref(end_signal), (uint16_t)my_port);
    auto tx_thread = std::thread(init_net_tx, &cfg, std::ref(end_signal), &cq, remote_addr, remote_port, my_addr, my_port);

    while (true){
        std::string s;
        std::getline(std::cin, s);
        ClientRequest cr;
        cr.body = s;
        cr.callback = [](std::string msg){
            std::cout << "Response: " << msg << std::endl;
        };

        cq.enqueue(cr);
    }
    rx_thread.join();
    tx_thread.join();

    return 0;
}