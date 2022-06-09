#ifndef __STATS_H__
#define __STATS_H__
#include "sys/time.h"
struct stats{
    int connfd;
    struct timeval arrival_time;
    struct timeval dispatch_time;
    int thread_id;
    int request_count;
    int static_request_count;
    int dynamic_request_count;
};
#endif
