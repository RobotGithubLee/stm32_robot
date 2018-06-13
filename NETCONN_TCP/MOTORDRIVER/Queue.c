#include "includes.h"
#include "Queue.h"




//初始化队列
int InitQueueinter(struct interQueue *Q)
{
	Q->front = Q->rear= 0;
	Q->QueueCount=0;
	return 0;
}

//入队
int EnQueueinter(int size, void *pe, struct interQueue *Q)
{	
	if(Q->QueueCount>=Q->MaxCount)
		InitQueueinter(Q);

	memcpy((void*)((int)(Q->QNode)+size*Q->rear),pe,size);
	Q->rear++;
	if(Q->rear>=Q->MaxCount)
	{	
		Q->rear=0;
	}
	Q->QueueCount++;

	return 0;
Err:
	return -1;
}

//出队并删除队头
int GetAndDelQueueinter(int size, void *pe, struct interQueue *Q)			
{
	if(Q->QueueCount<=0)goto Err;

	memcpy(pe,(void*)((int)(Q->QNode)+size*Q->front),size);
	
	Q->front++;
	if(Q->front>=Q->MaxCount)
	{	
		Q->front=0;
	}
	Q->QueueCount--;

	return 0;
Err:

	return -1;	
}

//出队并删除队头
int GetQueueinter(int size, void *pe, struct interQueue *Q)			
{
	if(Q->QueueCount<=0)goto Err;

	memcpy(pe,(void*)((int)(Q->QNode)+size*Q->front),size);

	return 0;
Err:

	return -1;	
}



//?????
int DelQueueinter( struct interQueue *Q)					
{
	if(Q->QueueCount<=0)goto Err;

	Q->front++;
	if(Q->front>=Q->MaxCount)
	{	
		Q->front=0;
	}
	Q->QueueCount--;

	return 0;
Err:

	return -1;	
}


int Check_interqueue(struct interQueue *Q)
{
	if (Q->QueueCount==1)
		return -1;		//??-1?????
	else
		return 0;	    //??0??????
}






