#ifndef __QUEUE_H__
#define __QUEUE_H__
#include <stdbool.h>
#include "segel.h"
#include "stats.h"
typedef struct queue_t* Queue;

void CreateQueue(int queue_max_size, char* policy);
bool isEmpty();
void addToWaitingQueue(struct stats thread_stats);
struct stats addToWorkingQueue();
void dequeueFromWorkingQueue(int connfd);
void queueRemove(Queue queue,int connfd); //not in use at the moment
void destroyQueue(Queue queue);
#endif