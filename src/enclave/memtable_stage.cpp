#include "cdb_t.h"
#include "kvs.h"
#include "../config.h"
#include "../util/queue_generator.h"
#include "stage_comm_types.h"
#include <atomic>

extern std::vector<RequestQueue *> *global_enclave_queues;

void enclave_memtable_stage(
    uint64_t cfg_,
    uint64_t end_signal_ptr,
    int in_q_idx,
    int out_q_idx,
    int dc_q_idx,
    int flush_q_idx
){
    Config *cfg = (Config *)cfg_;
    std::atomic<bool> &end_signal = *((std::atomic<bool> *)end_signal_ptr);
    RequestQueue *in_q = (RequestQueue *)global_enclave_queues->at(in_q_idx);
    RequestQueue *out_q = (RequestQueue *)global_enclave_queues->at(out_q_idx);

    KVStore kvs(cfg->memtable_size);

    fprintf(stderr, "Starting memtable...\n");

    while (!end_signal.load()){
        RequestContext *ctx = in_q->pop();

        Cmd *cmd = (Cmd *)ctx->body;
        CmdResult *res = new CmdResult;

        if (cmd->type == CMD_READ){
            std::string val;
            KV_Status status = kvs.Get(cmd->ops[0], val);

            res->result = val;
            res->status = status;
        }else if (cmd->type == CMD_WRITE){
            KV_Status status = kvs.Put(cmd->ops[0], cmd->ops[1]);
            res->status = status;
        }else{
            delete res;
            delete ctx;
            // DROP!
            continue;
        }

        free(ctx->body);
        ctx->body = res;

        out_q->push(ctx);
    }




}