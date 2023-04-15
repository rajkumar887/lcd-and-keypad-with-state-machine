#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Queue.h"

int enqueue(Queue *queue, unsigned char *cmd) {
    int tmp=0;
    if (queue->count >= MSG_QUEUE_DEPTH) {
        printf("enqueue: Queue depth exceeded\n");
        return 0;
    }
    memcpy(queue->cmdList[queue->rear], cmd, YY_BUF_SIZE);       //memcpy(dest, src, strlen(src)+1)
    //for(tmp=0;tmp<10;tmp++);
    tmp = queue->rear+1;										//rear is then incremented by one
    queue->rear = tmp % MSG_QUEUE_DEPTH;						//at the location rear add the new item
    queue->count++;
    return 1;
}


int dequeue(Queue *queue, unsigned char *cmd) {
	if (queue->count <= 0) {
		printf("dequeue: The queue is empty\n\r");
		return 0;
	}
	memcpy(cmd, queue->cmdList[queue->front], YY_BUF_SIZE);
	queue->front = (queue->front+1)%MSG_QUEUE_DEPTH;
	queue->count--;
	return 1;
}

int getQueueCount(Queue *queue) {
	return (queue->count);
}
