#include <stdbool.h>
typedef struct queue_t* Queue;

void CreateQueue(int queue_max_size, char* policy);
bool isEmpty();
void addToWaitingQueue(int new_fd);
int addToWorkingQueue();
void dequeueFromWorkingQueue(int connfd);
void queueRemove(Queue queue,int connfd); //not in use at the moment
void destroyQueue(Queue queue);
