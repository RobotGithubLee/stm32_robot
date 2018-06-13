#ifndef __MESSAGE_H
#define __MESSAGE_H	
#include "sys.h"	


#define QUEUEMAXCOUNT 40




typedef struct CanMessge
{
	int ID;
	u8 data[8];
	u8 len;
}SCM;

typedef struct TcpMessge
{
	u8 Head;
	u8 Seq;
	u8 Cmd;
	u8 state;
	u8 len;
	u8 data[126];
	u8 Bcc;
}STM;


int InitDriverCmdQueue(void);
	
int MessageHandle(u8 *msg,int len);

int MotionCmdHandle(u8 Seq,u8* data, u8 len);

int AngleMotionCmdHandle(u8 Seq,u8* data, u8 len);

int MotionFeedbackHandle(u8 Seq);

int EnDriverCmdQueue(int ID,u8* sendFramData,u8 len);

int EnFeedBackMsgQueue(u8 Cmd,u8 state,u8 Seq,u8 len,u8 *data);

int InitDriver(void);
#endif
