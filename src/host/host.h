#pragma once

#include <iostream>
#include <unistd.h>
#include <sched.h>
#include <string>
#include <sstream>
#include <openenclave/host.h>
#include <thread>
#include <vector>
#include <mutex>
#include <deque>
#include <condition_variable>
#include <shared_mutex>
#include <errno.h>
#include <cstdio>
#include <cstring>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "cdb_u.h"
#include "../common.h"
#include "../util/blockingconcurrentqueue.h"
#include "../util/bufpool.h"
#include "../dc_config.h"
#include "../config.h"