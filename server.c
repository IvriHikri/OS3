#include "segel.h"
#include "request.h"
#include <pthread.h>
#include "queue.h"
// 
// server.c: A very, very simple web server
//
// To run:
//  ./server <portnum (above 2000)>
//
// Repeatedly handles HTTP requests sent to this port number.
// Most of the work is done within routines written in request.c
//

// HW3: Parse the new arguments too
struct Queues{
    Queue waiting_queue;
    Queue working_queue;
};

void getargs(int *port,int* pool_size, int argc, char *argv[])
{
    if (argc < 2) {
	fprintf(stderr, "Usage: %s <port>\n", argv[0]);
	exit(1);
    }
    *port = atoi(argv[1]);
    *pool_size = atoi(argv[2]);
}

void* worker(void* temp){

    while(1){
    int connfd = addToWorkingQueue();
    requestHandle(connfd);
    Close(connfd);
    dequeueFromWorkingQueue(connfd);
    }
}

int main(int argc, char *argv[])
{
    int listenfd, connfd, port, clientlen, pool_size;
    struct sockaddr_in clientaddr;

    getargs(&port,&pool_size, argc, argv);
    printf("got args");
    //Craete threads
    CreateQueue();
    pthread_t *thread_pool = malloc(sizeof(*thread_pool)*pool_size);
    for(int i=0;i<pool_size;i++){
         pthread_create(&thread_pool[i],NULL,worker,NULL);
    }
    printf("finished initialized threads");
    //Create queues

    listenfd = Open_listenfd(port);
    while (1) {
	clientlen = sizeof(clientaddr);
	connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *) &clientlen);
    printf("accept client");
    addToWaitingQueue(connfd);
    
	// 
	// HW3: In general, don't handle the request in the main thread.
	// Save the relevant info in a buffer and have one of the worker threads 
	// do the work. 
	// 
    }

}


    


 
