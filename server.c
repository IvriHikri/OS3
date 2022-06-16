#include "segel.h"
#include "request.h"
#include <pthread.h>
#include "queue.h"
#include "stats.h"
// 
// server.c: A very, very simple web server
//
// To run:
//  ./server <portnum (above 2000)>
//
// Repeatedly handles HTTP requests sent to this port number.
// Most of the work is done within routines written in request.c
//


void getargs(int *port,int* pool_size, int* queue_size, char** policy, int argc, char *argv[])
{
    if (argc < 2) {
	fprintf(stderr, "Usage: %s <port>\n", argv[0]);
	exit(1);
    }
    *port = atoi(argv[1]);
    *pool_size = atoi(argv[2]);
    *queue_size= atoi(argv[3]);
    *policy=argv[4];
}

void* worker(void* id){

    int thread_id= *(int*)id;
    free(id);
    int request_count = 0;
    int static_request_count = 0;
    int dynamic_request_count=0;
    while(1){
	request_count++;
    struct stats current_request = addToWorkingQueue();
    current_request.thread_id=thread_id;
    current_request.request_count = request_count;
    current_request.static_request_count = static_request_count;
    current_request.dynamic_request_count = dynamic_request_count;

    //Calculate dispatch time

    requestHandle(current_request,&static_request_count,&dynamic_request_count);
    Close(current_request.connfd);
    dequeueFromWorkingQueue(current_request.connfd);
    }
}

int main(int argc, char *argv[])
{
    int listenfd, connfd, port, clientlen, pool_size,queue_size;
    struct sockaddr_in clientaddr;
    char* policy;

 getargs(&port,&pool_size,&queue_size, &policy, argc, argv);
   /* port = 8000;
    * policy = "random";
    queue_size=10;
    pool_size=1;
    */
    //Craete threads
    CreateQueue(queue_size, policy);
    pthread_t *thread_pool = malloc(sizeof(*thread_pool)*pool_size);
    for(int i=0;i<pool_size;i++){
        int* tmp = malloc(sizeof(*tmp));
        *tmp=i;
         pthread_create(&thread_pool[i],NULL,worker,tmp);
    }
    //Create queues

    listenfd = Open_listenfd(port);
    while (1) {
    struct timeval arrival_time_tv;
	clientlen = sizeof(clientaddr);
	connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *) &clientlen);
    gettimeofday(&arrival_time_tv,NULL);
    struct stats thread_stat= { connfd, arrival_time_tv,{0,0},-1,-1,-1,-1 };
    addToWaitingQueue(thread_stat);
    
	// 
	// HW3: In general, don't handle the request in the main thread.
	// Save the relevant info in a buffer and have one of the worker threads 
	// do the work. 
	// 
    }

}


    


 
