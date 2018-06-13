#ifndef __QUEUE_H
#define __QUEUE_H	
#include "sys.h"	

struct interQueue
{
	int front;
	int QueueCount;		
	int rear;			
	void *QNode;
	int MaxCount;
};


int InitQueueinter(struct interQueue *Q);
int EnQueueinter(int size, void *pe, struct interQueue *Q);
int GetAndDelQueueinter(int size, void *pe, struct interQueue *Q);
int GetQueueinter(int size, void *pe, struct interQueue *Q);
int DelQueueinter( struct interQueue *Q);

#endif
