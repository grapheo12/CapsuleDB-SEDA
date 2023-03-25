#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include "../util/queue_generator.h"
#include "connection_handler.h"
#include "host.h"

bool check_simulate_opt(int* argc, const char* argv[])
{
    for (int i = 0; i < *argc; i++)
    {
        if (strcmp(argv[i], "--simulate") == 0)
        {
            fprintf(stdout, "Running in simulation mode\n");
            memmove(&argv[i], &argv[i + 1], (*argc - i) * sizeof(char*));
            (*argc)--;
            return true;
        }
    }
    return false;
}

bool check_load_opt(int* argc, const char* argv[])
{
    for (int i = 0; i < *argc; i++)
    {
        if (strcmp(argv[i], "--load") == 0)
        {
            memmove(&argv[i], &argv[i + 1], (*argc - i) * sizeof(char*));
            (*argc)--;
            return true;
        }
    }
    return false;
}



static std::atomic<bool> end_signal;

void output_worker(oe_enclave_t *enclave, Config *cfg, std::atomic<bool> *end_signal_ptr, int in_q_idx, RequestQueue *out_q)
{
    fprintf(stderr, "Output thread started...\n");
    enclave_output_stage(enclave,
        (uint64_t)cfg, (uint64_t)end_signal_ptr,
        in_q_idx, (uint64_t)out_q);
}

void input_worker(oe_enclave_t *enclave, Config *cfg, std::atomic<bool> *end_signal_ptr, RequestQueue *in_q, int out_q_idx)
{
    fprintf(stderr, "Input thread started...\n");
    enclave_input_stage(enclave, 
        (uint64_t)cfg, (uint64_t)end_signal_ptr,
        (uint64_t)in_q, out_q_idx);
}

void memtable_worker(oe_enclave_t *enclave, Config *cfg, std::atomic<bool> *end_signal_ptr,
    int in_q_idx, int out_q_idx, int dc_q_idx, int flush_q_idx)
{
    fprintf(stderr, "Memtable thread started...\n");
    enclave_memtable_stage(enclave, 
        (uint64_t)cfg, (uint64_t)end_signal_ptr,
        in_q_idx, out_q_idx,
        dc_q_idx, flush_q_idx);
}

void host_gc(void *ptr)
{
    free(ptr);
}

int main(int argc, const char *argv[])
{
    oe_result_t result;
    int ret = 1;
    oe_enclave_t* enclave = NULL;
    bool load_db;
    std::thread rx_thread, tx_thread, input_thread, output_thread, memtable_thread;

    auto qvec = generate_queue_list(2);
    Config cfg;

    oe_enclave_setting_context_switchless_t switchless_setting = {
        1,  // number of host worker threads
        1}; // number of enclave worker threads.
    oe_enclave_setting_t settings[] = {{
        .setting_type = OE_ENCLAVE_SETTING_CONTEXT_SWITCHLESS,
        .u.context_switchless_setting = &switchless_setting,
    }};

    uint32_t flags = OE_ENCLAVE_FLAG_DEBUG;
    if (check_simulate_opt(&argc, argv))
    {
        flags |= OE_ENCLAVE_FLAG_SIMULATE;
    }

    if (check_load_opt(&argc, argv))
    {
        load_db = true; 
    }

    if (argc < 3)
    {
        fprintf(
            stderr, "Usage: %s enclave_image_path config_path [ --simulate --load  ]\n", argv[0]);
        goto exit;
    }
    end_signal.store(false);
    cfg.key_maxsize = 100;
    cfg.val_maxsize = 1000;
    cfg.memtable_size = 1100000;

    std::cerr << "Creating Enclave" << std::endl;
    result = oe_create_cdb_enclave(
        argv[1], OE_ENCLAVE_TYPE_AUTO, flags, settings, OE_COUNTOF(settings), &enclave);
    if (result != OE_OK)
    {
        fprintf(
            stderr,
            "oe_create_cdb_enclave(): result=%u (%s)\n",
            result,
            oe_result_str(result));
        goto exit;
    }

    ret = enclave_generate_queues(enclave, 4);
    if (ret != OE_OK){
        goto exit;
    }

    rx_thread = std::thread(init_net_rx, &cfg, std::ref(end_signal), (uint16_t)3001, qvec->at(0));
    input_thread = std::thread(input_worker, enclave, &cfg, &end_signal, qvec->at(0), 0);
    memtable_thread = std::thread(memtable_worker, enclave, &cfg, &end_signal, 0, 1, 2, 3);
    output_thread = std::thread(output_worker, enclave, &cfg, &end_signal, 1, qvec->at(1));
    tx_thread = std::thread(init_net_tx, enclave, &cfg, std::ref(end_signal), qvec->at(1));

    rx_thread.join();
    tx_thread.join();
    input_thread.join();
    output_thread.join();
    memtable_thread.join();

exit:
    // Clean up the enclave if we created one
    if (enclave)
        oe_terminate_enclave(enclave);

    return ret;
}