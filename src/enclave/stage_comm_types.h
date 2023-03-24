#pragma once

#include <string>
#include "cdb_t.h"

enum CmdType
{
    CMD_READ,
    CMD_WRITE
};

struct Cmd
{
    CmdType type;
    std::string ops[2];
};

struct CmdResult
{
    KV_Status status;
    std::string result;
};