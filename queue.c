#include "queue.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
pthread_cond_t c;
pthread_mutex_t m; 
int queue_size=0;
struct queue_t{
    struct queue_t* next;
    int connfd;
};
Queue CreateQueue(){
    pthread_cond_init(&c,NULL);
    pthread_mutex_init(&m,NULL);
    Queue queue = malloc(sizeof(*queue));
    queue->connfd=-1;
    queue->next=NULL;
    return queue;
}
bool isEmpty(Queue queue){
    return queue_size==0;
}
void enqueue(Queue queue, int new_fd){
    printf("reached enqueue");
    pthread_mutex_lock(&m);
    Queue tmp = queue;
    while(tmp->next!=NULL)
    {
        tmp=tmp->next;
    }
    Queue new_node = malloc(sizeof(*queue));
    new_node->next=NULL;
    new_node->connfd=new_fd;
    tmp->next=new_node;
    queue_size++;
    pthread_cond_signal(&c);
    pthread_mutex_unlock(&m);
}
int dequeue(Queue queue){
    printf("reached dequeue");
    pthread_mutex_lock(&m);
    while(queue_size==0){
        pthread_cond_wait(&c,&m);
    }
    Queue to_remove= queue->next;
    int fd_to_return=to_remove->connfd;
    if(to_remove->next==NULL){
        free(to_remove);
        queue->next=NULL;
    }
    else{
        queue->next=to_remove->next;
        free(to_remove);
    }
    queue_size--;
    pthread_mutex_unlock(&m);
    return fd_to_return;
}

void queueRemove(Queue queue, int connfd){
    pthread_mutex_lock(&m);
    while(queue_size==0){
        pthread_cond_wait(&c,&m);
    }
    Queue to_remove;
    Queue temp=queue;
    while(temp->next!=NULL){
        if(temp->next->connfd==connfd){
            to_remove=temp;
            break;
        }
        temp=temp->next;
    }
    temp->next=to_remove->next->next;
    free(to_remove->next);
    queue_size--;
    pthread_mutex_unlock(&m);
}