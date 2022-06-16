#include "queue.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include "segel.h"
#include <math.h>
pthread_cond_t c;
pthread_mutex_t m; 
int queue_size=0;
int waiting_size=0;
int working_size=0;
Queue waiting_queue=NULL;
Queue working_queue=NULL;
struct queue_t{
    struct queue_t* next;
    struct stats request;
};

int max_queue_size =0;
char* overload_policy=NULL; 

int random_num(int min, int max){
   return min + rand() / (RAND_MAX / (max - min + 1) + 1);
}
int our_ceil(double num){
    int a= (int)num;
    if(num-(double)a<=1e-6)
    return a;
    return a+1;
}
void CreateQueue(int queue_max_size, char* policy){
    pthread_cond_init(&c,NULL);
    pthread_mutex_init(&m,NULL);
    waiting_queue = malloc(sizeof(*waiting_queue));
    waiting_queue->request.connfd=-1;
    waiting_queue->next=NULL;

    working_queue = malloc(sizeof(*working_queue));
    working_queue->request.connfd=-1;
    working_queue->next=NULL;
    max_queue_size=queue_max_size;
    overload_policy=policy;
}
bool isEmpty(){
    return queue_size==0;
}
void addToWaitingQueue(struct stats new_req){
    pthread_mutex_lock(&m);
    if(strcmp(overload_policy,"block")==0){
        while(queue_size==max_queue_size){
            pthread_cond_wait(&c, &m);
        }
    }
    if(strcmp(overload_policy,"dt")==0){
        if(queue_size==max_queue_size){
            Close(new_req.connfd);
            pthread_mutex_unlock(&m);
            return;
        }
    }
    if(strcmp(overload_policy,"dh")==0){
        if(queue_size==max_queue_size){
            //Queue before_oldest = waiting_queue->next;
            //Queue temp = waiting_queue;
            //while(temp->next!=NULL){
                //if(timercmp(&temp->next->request.arrival_time,&before_oldest->request.arrival_time,<)){
                   // before_oldest=temp;
                //}
                //temp=temp->next;
            //}
    //Queue oldest =  before_oldest->next;
    //before_oldest->next=oldest->next;
    Queue oldest =  waiting_queue->next;
    if(oldest==NULL){
	close(new_req.connfd);
        pthread_mutex_unlock(&m);
	return;
	}
    waiting_queue->next=oldest->next;
    Close(oldest->request.connfd);
    free(oldest);
    queue_size--;
    waiting_size--;
        }
    }
    if(strcmp(overload_policy,"random")==0){
        if(queue_size==max_queue_size){
            int to_drop=our_ceil((double)waiting_size*0.3);
            if(waiting_size==0){
                Close(new_req.connfd);
                pthread_mutex_unlock(&m);
                return;
            }
            int i=waiting_size;
            while(to_drop>0 && i>0){
                if(waiting_queue->next==NULL)
                    break;
                int to_delete=random_num(1,i);
                int j=1;    
                Queue temp=waiting_queue;
                while(j<to_delete){
                temp=temp->next;
                j++;
                }
           Queue to_remove=temp->next;
           temp->next = to_remove->next;
            Close(to_remove->request.connfd);
            free(to_remove);
            to_drop--;
            i--;
            waiting_size--;
            queue_size--;     
            }
        }
    }
    Queue tmp = waiting_queue;
    while(tmp->next!=NULL)
    {
        tmp=tmp->next;
    }
    Queue new_node = malloc(sizeof(*new_node));
    new_node->next=NULL;
    new_node->request=new_req;
    tmp->next=new_node;
    queue_size++;
    waiting_size++;
    pthread_cond_signal(&c);
    pthread_mutex_unlock(&m);
}

struct stats addToWorkingQueue(){
    pthread_mutex_lock(&m);
    while(waiting_size==0){
        pthread_cond_wait(&c,&m);
    }
    Queue to_remove= waiting_queue->next;
    struct stats request_to_return = to_remove->request;
    struct timeval pickup_time;
    gettimeofday(&pickup_time,NULL);
    timersub(&pickup_time,&request_to_return.arrival_time,&request_to_return.dispatch_time);
    waiting_queue->next=to_remove->next;
    free(to_remove);
    waiting_size--;
    Queue tmp = working_queue;
    while(tmp->next!=NULL)
    {
        tmp=tmp->next;
    }
    Queue new_node = malloc(sizeof(*new_node));
    new_node->next=NULL;
    new_node->request=request_to_return;
    tmp->next=new_node;
    working_size++;
    pthread_cond_signal(&c);
    pthread_mutex_unlock(&m);
    return request_to_return;
}

void dequeueFromWorkingQueue(int connfd){
    pthread_mutex_lock(&m);
    while(working_size==0){
        pthread_cond_wait(&c,&m);
    }
    Queue temp=working_queue;
    while(temp->next->request.connfd != connfd){
        temp=temp->next;
    }
    Queue to_remove = temp->next;
    temp->next=to_remove->next;
    free(to_remove);
    queue_size--;
    working_size--;
    pthread_cond_broadcast(&c);
    pthread_mutex_unlock(&m);
}

void queueRemove(Queue queue, int connfd){

}
