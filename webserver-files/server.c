#include "segel.h"
#include "request.h"
#include <pthread.h>
#include <queue.h>
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
cond_t c;
mutex_t m; 
int waiting_list_size = 0;
void getargs(int *port,int* pool_size, int argc, char *argv[])
{
    if (argc < 2) {
	fprintf(stderr, "Usage: %s <port>\n", argv[0]);
	exit(1);
    }
    *port = atoi(argv[1]);\
    *pool_size = atoi(argv[2]);
}

void* worker(void* ...){
    
    int connfd = dequeue(waiting_list);
    enqueue(working_list,connfd);
    requestHandle(connfd);
    dequeue(working_list,connfd);
    /*
    mutex_lock(&m);
    while(waiting_list_size==0){
        cond_wait(&c,&m);
    }
    int connfd = remove(waiting_list);
    waiting_list_size--;
    mutex_unlock(&m);
    requestHandle(connfd);
    */
}
struct Queues{
    Queue waiting_list;
    Queue working_list;
}
int main(int argc, char *argv[])
{
    int listenfd, connfd, port, clientlen, pool_size;
    struct sockaddr_in clientaddr;

    getargs(&port,&pool_size, argc, argv);

    //Create threads
    pthread_cond_init(&c,NULL);
    pthread_mutex_init(&m,NULL);
    Queue waiting_queue = QueueCreate();
    Queue working_queue= QueueCreate();
    struct Queues = {waiting_queue,working_queue};
    pthread_t *thread_pool = malloc(sizeof(*thread_pool)*pool_size);
    for(int i=0;i<pool_size;i++){
         pthread_create(&thread_pool[i],NULL,worker,(void*)&Queues);
    }
    //Create queues

    listenfd = Open_listenfd(port);
    while (1) {
	clientlen = sizeof(clientaddr);
	connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *) &clientlen);
    mutex_lock(&m);
    insert(waiting_queue,connfd);
    waiting_list_size++;
    cond_signal(&c);
    mutex_unlock(&m);
    
	// 
	// HW3: In general, don't handle the request in the main thread.
	// Save the relevant info in a buffer and have one of the worker threads 
	// do the work. 
	// 
	requestHandle(connfd);

	Close(connfd);
    }

}


    


 
