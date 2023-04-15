#ifndef QUEUE_H
#define QUEUE_H
#define YY_BUF_SIZE 3
#define MSG_QUEUE_DEPTH 20

typedef struct queue_ {
	int count;
	int front;
	int rear;
	unsigned char cmdList[MSG_QUEUE_DEPTH][YY_BUF_SIZE];
} Queue;


int enqueue(Queue *, unsigned char *);
int dequeue(Queue *, unsigned char *);
int getQueueCount(Queue *);

#endif 
