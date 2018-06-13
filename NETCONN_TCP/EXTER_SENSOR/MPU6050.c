#include "includes.h"
#include "MPU6050.h"												    
#include "usart6.h"	
#include "Queue.h"
#include "ConvertData.h"			


#define MPUQUEUECOUNT 10

//客户端任务优先级
#define MPU6050_SERVER_PRIO		20
//任务堆栈大小	
#define MPU6050_STK_SIZE 		128
//任务堆栈	
__align(8) OS_STK MPU6050_TASK_STK[MPU6050_STK_SIZE];
//任务函数

short Yaw;



static void MPU6050_thread(void *arg)
{
	float a[3],w[3],Angle[3],T;
	while(1)
	{
		if(USART6_RX_STA&0X8000 )		                            //接收到一次数据了 && USART6_RX_BUF[0]==0x55
		{ 
					if(USART6_RX_BUF[1]==0x51)
					{
						a[0]=(short)(USART6_RX_BUF[3]<<8|USART6_RX_BUF[2])/32768.0*16;
						a[1]=(short)(USART6_RX_BUF[5]<<8|USART6_RX_BUF[4])/32768.0*16;
						a[2]=(short)(USART6_RX_BUF[7]<<8|USART6_RX_BUF[6])/32768.0*16;
					}
					if(USART6_RX_BUF[12]==0x52)
					{
						w[0]=(short)(USART6_RX_BUF[3+11]<<8|USART6_RX_BUF[2+11])/32768.0*2000;
						w[1]=(short)(USART6_RX_BUF[5+11]<<8|USART6_RX_BUF[4+11])/32768.0*2000;
					  w[2]=(short)(USART6_RX_BUF[7+11]<<8|USART6_RX_BUF[6+11])/32768.0*2000;
					}
					if(USART6_RX_BUF[23]==0x53)
					{
						Angle[0]=(short)(USART6_RX_BUF[3+22]<<8|USART6_RX_BUF[2+22])/32768.0*180;
						Angle[1]=(short)(USART6_RX_BUF[5+22]<<8|USART6_RX_BUF[4+22])/32768.0*180;
					  Angle[2]=(short)(USART6_RX_BUF[7+22]<<8|USART6_RX_BUF[6+22])/32768.0*180;
						Yaw=(short)(Angle[2]*100);
						printf("%d\r\n",(short)Angle[2]);
					}
					
		   	//printf("len=%d,USART6_RX_BUF[1]=%d \r\n",USART6_RX_BUF[0],USART6_RX_BUF[1]);
				  // 	printf("%c%c%c",0xAA,0xAA,0xAA);
					//	printf("%s",USART6_RX_BUF);
					 /* 
					switch(USART6_RX_BUF[1])
					{
						case 0x51:
							a[0]=(short)(USART6_RX_BUF[3]<<1|USART6_RX_BUF[2])/32768.0*16;
						  a[1]=(short)(USART6_RX_BUF[5]<<1|USART6_RX_BUF[4])/32768.0*16;
					  	a[2]=(short)(USART6_RX_BUF[7]<<1|USART6_RX_BUF[6])/32768.0*16;
					   //	printf("a[0]=%f,a[1]=%f,a[2]=%f\r\n",a[0],a[1],a[2]);
						break;
						case 0x52:
							w[0]=(short)(USART6_RX_BUF[3]<<1|USART6_RX_BUF[2])/32768.0*2000;
						  w[1]=(short)(USART6_RX_BUF[5]<<1|USART6_RX_BUF[4])/32768.0*2000;
					  	w[2]=(short)(USART6_RX_BUF[7]<<1|USART6_RX_BUF[6])/32768.0*2000;
						 // printf("w[0]=%f,e[1]=%f,w[2]=%f\r\n",w[0],w[1],w[2]);
						break;
						case 0x53:
							Angle[0]=(short)(USART6_RX_BUF[3+22]<<1|USART6_RX_BUF[2+22])/32768.0*180;
						  Angle[1]=(short)(USART6_RX_BUF[5+22]<<1|USART6_RX_BUF[4+22])/32768.0*180;
					  	Angle[2]=(short)(USART6_RX_BUF[7+22]<<1|USART6_RX_BUF[6+22])/32768.0*180;
						//  printf("Angle[0]=%f,Angle[1]=%f,Angle[2]=%f\r\n",Angle[0],Angle[1],Angle[2]);
						printf("%s",USART6_RX_BUF);
						break;					
					}	*/	
				  USART6_RX_STA=0;			
				}				
				delay_ms(3);
	}
	  
}




//创建MPU6050线程
//返回值:0 MPU6050创建成功
//		其他 MPU6050创建失败
u8 MPU6050_task_init(void)
{
  INT8U res;
	OS_CPU_SR cpu_sr;
	
	OS_ENTER_CRITICAL();	//关中断
	res = OSTaskCreate(MPU6050_thread,(void*)0,(OS_STK*)&MPU6050_TASK_STK[MPU6050_STK_SIZE-1],MPU6050_SERVER_PRIO); //创建TCP服务器线程
	OS_EXIT_CRITICAL();		//开中断
	
	return res;
}

















		  






