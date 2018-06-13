#include "includes.h"
#include "message.h"
#include "delay.h"

#include "Queue.h"
#include "ConvertData.h"
#include "kinematics.h"
#include "driver.h"
#include "can.h"
#include "usart6.h"

#define PI 3.1415926


const float  aRatio=PI*16/34/180;   //max Angle=16°，UP max=34;
const float  wRatio=0.0001;
const float  vRatio=1000*1/340;    //Max v=1.0 m/s, *1000->mm/s, 340->max data 
static u8 isAuto=0;
extern u8 wifiSendBuf(u8 *buf,u8 len);
extern short Yaw;
//信息反馈队列
struct interQueue *FMQ,FeedbackMsgQueue;

STM FeedbackMsgArrry[QUEUEMAXCOUNT];



//接收命令队列
struct interQueue *DCQ,DriverCmdQueue;
SCM  DriverCmdArray[QUEUEMAXCOUNT];


void * MsgGrp[256];			//消息队列存储地址,最大支持256个消息

int InitDriverCmdQueue(void)
{
	u8 sendFramData[8];
	//Init FeedBack MsgQueue
	memset((void*)FeedbackMsgArrry,0x00,sizeof(FeedbackMsgArrry));

	FeedbackMsgQueue.QNode=FeedbackMsgArrry;

	FMQ=&FeedbackMsgQueue;

	FMQ->MaxCount=QUEUEMAXCOUNT;

	
	//Init Fornt Drive CmdQueue
	memset((void*)DriverCmdArray,0x00,sizeof(DriverCmdArray));

	DriverCmdQueue.QNode=DriverCmdArray;

	DCQ=&DriverCmdQueue;

	DCQ->MaxCount=QUEUEMAXCOUNT;

  InitQueueinter(DCQ);
	
	
	//Init  Drive: setTacc and Enable Drive
	setTVelocityFram(POS_MAX_SPEED,sendFramData);
	EnDriverCmdQueue(FRONT_ID,sendFramData,0x8);
	EnDriverCmdQueue(LEFT_ID,sendFramData,0x8);
	EnDriverCmdQueue(RIGHT_ID,sendFramData,0x8);
	
	//Enable Drive
	EnableFram(sendFramData);
	EnDriverCmdQueue(FRONT_ID,sendFramData,0x8);
	EnDriverCmdQueue(LEFT_ID,sendFramData,0x8);
	EnDriverCmdQueue(RIGHT_ID,sendFramData,0x8);

	
	return 0;
}



int MessageHandle(u8 *msg,int len)
{
	
	STM RxMsg;
	
	memcpy(&RxMsg,msg,len-1);
	RxMsg.Bcc=msg[len-1];
	
	if(RxMsg.Head!=0xAA  || RxMsg.len+EXTERN_BUFSIZE!=len)
	{
		u8 i;
		for(i=0;i<len;i++)
		{
			printf("%2x  ",*(msg+i));
		}
		printf("\r\n");
		printf("message.c:  msg is not available\r\n");
		return -1;
	}
	
  //XOR check	
	if(getXorCheck(RxMsg.data,RxMsg.len)!=RxMsg.Bcc)
	{
		printf("message.c:  XOR check error\r\n");
		//return -1;
	}
	
	MotionFeedbackHandle(RxMsg.Seq);
	
	switch(RxMsg.Cmd) 
	{
		case 0x01:
			isAuto=RxMsg.data[0];
		printf("message.c:  isAuto=%d\r\n",isAuto);
		break;
		case 0x60:
			if(!isAuto)
				AngleMotionCmdHandle(RxMsg.Seq,RxMsg.data,RxMsg.len);
		break;
		
		case 0x80:
			if(isAuto)
				MotionCmdHandle(RxMsg.Seq,RxMsg.data,RxMsg.len);
		break;	
		
		case 0x81:
			MotionFeedbackHandle(RxMsg.Seq);
		break;	
	}
	return 0;
}



int MotionCmdHandle(u8 Seq,u8* data, u8 len)
{
	int ExpPos=0,ExpRightV=0,ExpLeftV=0;
	u8 sendFramData[8];
	float w,v,radio;
	if(len!=0x4)
	{
		printf("message.c: MotionCmdHandle check error\r\n");
		return -1;
	}
	
	w=hexbyte2short(data)*wRatio;
	if(w>0.5)
		w=0.5;
	else if(w<-0.5)
		w=-0.5;	
	v=hexbyte2short(data+2);
	if(v>1000)
		v=1000;
	else if(v<-1000)
		v=-1000;
	ExpPos=getExpPosition(w,v);
	ExpLeftV=getExpLeftVelocity(v);
	ExpRightV=getExpRightVelocity(v);
  printf("message.c: w=%f v=%f\r\n",w,v);
	 
	setPositionFram(ExpPos,sendFramData);
	EnDriverCmdQueue(FRONT_ID,sendFramData,0x8);
	
	setVelocityFram(ExpLeftV,sendFramData);
	EnDriverCmdQueue(LEFT_ID,sendFramData,0x8);
	
	setVelocityFram(ExpRightV,sendFramData);
	EnDriverCmdQueue(RIGHT_ID,sendFramData,0x8);
	
	return 0;
}


 
int AngleMotionCmdHandle(u8 Seq,u8* data, u8 len)
{
	int ExpPos=0,ExpRightV=0,ExpLeftV=0;
	u8 sendFramData[8];
	float Angle,v;
	if(len!=0x4)
	{
		printf("message.c: MotionCmdHandle check error\r\n");
		return -1;
	}
	
	Angle=hexbyte2short(data)*aRatio;
	
	v=hexbyte2short(data+2)*vRatio;
	
	ExpPos=Angle2Position(Angle);
	ExpLeftV=getExpLeftVelocity(v);
	ExpRightV=getExpRightVelocity(v);
	
	printf("Angle=%f v=%f ExpPos=%d  ExpLeftV=%d  ExpRightV=%d\r\n",Angle,v,ExpPos,ExpLeftV,ExpRightV);
	
	setPositionFram(ExpPos,sendFramData);
	EnDriverCmdQueue(FRONT_ID,sendFramData,0x8);
	
	setVelocityFram(ExpLeftV,sendFramData);
	EnDriverCmdQueue(LEFT_ID,sendFramData,0x8);
	
	setVelocityFram(ExpRightV,sendFramData);
	EnDriverCmdQueue(RIGHT_ID,sendFramData,0x8);
	
	return 0;
}


int MotionFeedbackHandle(u8 Seq)
{
	u8 sendFramData[8];
	u8 sendData[6];
	short w;

	getPositionFram(sendFramData);
	EnDriverCmdQueue(FRONT_ID,sendFramData,0x8);
	
	getVelocityFram(sendFramData);
	EnDriverCmdQueue(LEFT_ID,sendFramData,0x8);
	
	getVelocityFram(sendFramData);
	EnDriverCmdQueue(RIGHT_ID,sendFramData,0x8);
	
	delay_ms(10);
                  
	short2hexbyte((short)(getCurAngleVel()/wRatio),sendData);    //w
	short2hexbyte((short)(getCurVelocity()),sendData+2);    //v
	short2hexbyte(Yaw,sendData+4);    // IMU angle
  
	EnFeedBackMsgQueue(0x81,0x00,Seq,0x06,sendData);
	
	return 0;
}


int EnDriverCmdQueue(int ID,u8* sendFramData,u8 len)
{
	int i=0;
	SCM sendCanmsg;
	sendCanmsg.ID=ID;
	for(i=0;i<len;i++)
	{
		sendCanmsg.data[i]=sendFramData[i];
	}
	sendCanmsg.len=len;
	
	EnQueueinter(sizeof(SCM),&sendCanmsg,DCQ);

	return 0;
}


int EnFeedBackMsgQueue(u8 Cmd,u8 state,u8 Seq,u8 len,u8 *data)
{
	STM feedBackMsg;
	feedBackMsg.Head=0xCC;
	feedBackMsg.Cmd=Cmd;
	feedBackMsg.Seq=Seq;
	feedBackMsg.state=state;
	feedBackMsg.len=len;
	memcpy(feedBackMsg.data,data,len);
	feedBackMsg.Bcc=getXorCheck(data,len);
	EnQueueinter(sizeof(STM),&feedBackMsg,FMQ);
	u6_Send(&feedBackMsg.Head,len+EXTERN_BUFSIZE);
	return 0;
}

int InitDriver(void)
{
	int i=0;
	SCM sendCanmsg;
	for(i=0;i<3;i++)
	{
		sendCanmsg.ID=i+0x604;
		setTVelocityFram(POS_MAX_SPEED,sendCanmsg.data);
		sendCanmsg.len=0x8;
		EnQueueinter(sizeof(SCM),&sendCanmsg,DCQ);
		delay_ms(2);
		
		EnableFram(sendCanmsg.data);
		EnQueueinter(sizeof(SCM),&sendCanmsg,DCQ);
		
		delay_ms(2);
		getStautsFram(sendCanmsg.data);
		EnQueueinter(sizeof(SCM),&sendCanmsg,DCQ);
	}

	return 0;
}
