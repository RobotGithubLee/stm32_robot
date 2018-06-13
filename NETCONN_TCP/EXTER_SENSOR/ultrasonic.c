#include "includes.h"
#include "ultrasonic.h"												    
#include "usart6.h"	
#include "Queue.h"
#include "ConvertData.h"			


#define ULQUEUECOUNT 10

//�ͻ����������ȼ�
#define ULTRASONIC_SERVER_PRIO		20
//�����ջ��С	
#define ULTRASONIC_STK_SIZE 		128
//�����ջ	
__align(8) OS_STK ULTRASONIC_TASK_STK[ULTRASONIC_STK_SIZE];
//������

const u8 ultrasonic_Count=4;

short Dis_ultrasonic[4];






static void ultrasonic_thread(void *arg)
{
	u8 i,j=0,count,buf[3];
	short d[40];

	
	buf[0]=0xE8;
	buf[1]=0x02;
	while(1)
	{
		for(i=0;i<ultrasonic_Count;i++)
		{
			buf[2]=0x30+i*8;
			u6_Send(buf,3);
			delay_ms(5);
			count=0;
			while(1)
			{
				if(USART6_RX_STA&0X8000)		                            //���յ�һ��������
				{ 
					if(j>ULQUEUECOUNT-1)
						j=0;
				  d[j+i*ULQUEUECOUNT]=hexbyte2short(USART6_RX_BUF);
					Dis_ultrasonic[i]=GetMedianNum(d+i*ULQUEUECOUNT,ULQUEUECOUNT);	
					printf("COM%d: d=%d \r\r",i+1,Dis_ultrasonic[i]);
					j++;
				  USART6_RX_STA=0;	
				  break;			
				}				
				delay_ms(5);
				count++;
				if(count>20)
					break;
			}		
		}	
		printf("\n");
		delay_ms(20);
	}
	  
}




//����ULTRASONIC�߳�
//����ֵ:0 ULTRASONIC�����ɹ�
//		���� ULTRASONIC����ʧ��
u8 ultrasonic_task_init(void)
{
  INT8U res;
	OS_CPU_SR cpu_sr;
	
	OS_ENTER_CRITICAL();	//���ж�
	res = OSTaskCreate(ultrasonic_thread,(void*)0,(OS_STK*)&ULTRASONIC_TASK_STK[ULTRASONIC_STK_SIZE-1],ULTRASONIC_SERVER_PRIO); //����TCP�������߳�
	OS_EXIT_CRITICAL();		//���ж�
	
	return res;
}

















		  






