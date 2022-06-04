
typedef struct queue_t* Queue;

Queue bool isEmpty(List list);
void insert(Queue list, int new_fd);
void remove(Queue list,int to_remove_fd);
void destroyList(Queue list);
