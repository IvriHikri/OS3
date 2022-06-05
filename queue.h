#include <stdbool.h>
typedef struct queue_t* Queue;

Queue CreateQueue();
bool isEmpty(Queue queue);
void enqueue(Queue queue, int new_fd);
int dequeue(Queue queue);
void queueRemove(Queue queue,int connfd);
void destroyQueue(Queue queu);
