#include "queue.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
pthread_cond_t c;
pthread_mutex_t m; 
int queue_size=0;
int waiting_size=0;
int working_size=0;
Queue waiting_queue=NULL;
Queue working_queue=NULL;
struct queue_t{
    struct queue_t* next;
    int connfd;
};

void CreateQueue(){
    pthread_cond_init(&c,NULL);
    pthread_mutex_init(&m,NULL);
    waiting_queue = malloc(sizeof(*waiting_queue));
    waiting_queue->connfd=-1;
    waiting_queue->next=NULL;

    working_queue = malloc(sizeof(*working_queue));
    working_queue->connfd=-1;
    working_queue->next=NULL;
}
bool isEmpty(){
    return queue_size==0;
}
void addToWaitingQueue(int new_fd){
    printf("reached enqueue");
    pthread_mutex_lock(&m);
    Queue tmp = waiting_queue;
    while(tmp->next!=NULL)
    {
        tmp=tmp->next;
    }
    Queue new_node = malloc(sizeof(*new_node));
    new_node->next=NULL;
    new_node->connfd=new_fd;
    tmp->next=new_node;
    queue_size++;
    waiting_size++;
    pthread_cond_signal(&c);
    pthread_mutex_unlock(&m);
}

int addToWorkingQueue(){
    pthread_mutex_lock(&m);
    while(waiting_size==0){
        pthread_cond_wait(&c,&m);
    }
    Queue to_remove= waiting_queue->next;
    int fd_to_return=to_remove->connfd;
    if(to_remove->next==NULL){
        free(to_remove);
        waiting_queue->next=NULL;
    }
    else{
        waiting_queue->next=to_remove->next;
        free(to_remove);
    }
    waiting_size--;
    Queue tmp = working_queue;
    while(tmp->next!=NULL)
    {
        tmp=tmp->next;
    }
    Queue new_node = malloc(sizeof(*new_node));
    new_node->next=NULL;
    new_node->connfd=fd_to_return;
    tmp->next=new_node;
    working_size++;
    pthread_cond_signal(&c);
    pthread_mutex_unlock(&m);
    return fd_to_return;
}

void dequeueFromWorkingQueue(int connfd){
    pthread_mutex_lock(&m);
    while(working_size==0){
        pthread_cond_wait(&c,&m);
    }
    Queue to_remove;
    Queue temp=working_queue;
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
    working_size--;
    pthread_mutex_unlock(&m);
}

void queueRemove(Queue queue, int connfd){

}