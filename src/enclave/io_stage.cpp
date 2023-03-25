#include "cdb_t.h"
#include "kvs.h"
#include "../config.h"
#include "../util/queue_generator.h"
#include "stage_comm_types.h"
#include "net_request.pb.h"
using namespace net_request;

#include <atomic>

extern std::vector<RequestQueue *> *global_enclave_queues;

void enclave_input_stage(
    uint64_t cfg_,
    uint64_t end_signal_ptr,
    uint64_t in_q_,
    int out_q_idx
){
    Config *cfg = (Config *)cfg_;
    std::atomic<bool> &end_signal = *((std::atomic<bool> *)end_signal_ptr);
    RequestQueue *in_q = (RequestQueue *)in_q_;
    RequestQueue *out_q = (RequestQueue *)global_enclave_queues->at(out_q_idx);

    fprintf(stderr, "Starting input...\n");

    while (!end_signal.load()){
        // @todo: Crypto operations here
        RequestContext *ctx = in_q->pop();

        char *data = (char *)ctx->original_body;
        NetCommand nc;
        if (!nc.ParseFromString(std::string(data, ctx->sz))){
            free((char *)ctx->body);
            continue;
        }
        // host_gc((char *)ctx->original_body);
        
        Cmd *cmd = new Cmd;
        if (nc.type() == 0 && nc.ops().size() >= 1){
            cmd->type = CMD_READ;
            cmd->ops[0] = std::string(nc.ops().at(0));
        }else if (nc.type() == 1 && nc.ops().size() >= 2){
            cmd->type = CMD_WRITE;
            cmd->ops[0] = std::string(nc.ops().at(0));
            cmd->ops[1] = std::string(nc.ops().at(1));
        }

        ctx->body = cmd;
        out_q->push(ctx);
    }

}

void enclave_output_stage(
    uint64_t cfg_,
    uint64_t end_signal_ptr,
    int in_q_idx,
    uint64_t out_q_
){
    Config *cfg = (Config *)cfg_;
    std::atomic<bool> &end_signal = *((std::atomic<bool> *)end_signal_ptr);
    RequestQueue *in_q = (RequestQueue *)global_enclave_queues->at(in_q_idx);
    RequestQueue *out_q = (RequestQueue *)out_q_;

    fprintf(stderr, "Starting output...\n");

    while (!end_signal.load()){
        // @todo: Crypto operations here
        CmdResult *cr;
        RequestContext *ctx = in_q->pop();
        cr = (CmdResult *)ctx->body;

        std::string resp_str;
        if (cr->status == READ_FAIL){
            resp_str = "READ_FAIL";
        }else if (cr->status == READ_PASS){
            resp_str = cr->result;
        }else if (cr->status == WRITE_PASS){
            resp_str = "WRITE_PASS";
        }else{
            resp_str = "WRITE_FAIL";
        }

        free(cr);
        // ctx->body = new char[resp_str.size()];
        memcpy(ctx->original_body, resp_str.c_str(), resp_str.size());
        ctx->sz = resp_str.size();

        out_q->push(ctx);
    }


}