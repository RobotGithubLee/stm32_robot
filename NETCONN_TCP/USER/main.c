#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "can.h"
#include "key.h"
#include "lwip_comm.h"
#include "LAN8720.h"
#include "usmart.h"
#include "timer.h"


#include "sram.h"
#include "malloc.h"
#include "lwip_comm.h"
#include "includes.h"
#include "lwipopts.h"
#include "tcp_server_demo.h"
#include "tcp_client_demo.h"


#include "MPU6050.h"		
#include "ultrasonic.h"		
#include "driver.h"
#include "message.h"
 
#include "usart6.h"
#include "usart3.h"
#include "usart2.h"
#include "common.h" 




//KEY����
#define KEY_TASK_PRIO 		8
//�����ջ��С
#define KEY_STK_SIZE		128	
//�����ջ
OS_STK KEY_TASK_STK[KEY_STK_SIZE];
//������
void key_task(void *pdata);   

//LED����
//�������ȼ�
#define LED_TASK_PRIO		9
//�����ջ��С
#define LED_STK_SIZE		64
//�����ջ
OS_STK	LED_TASK_STK[LED_STK_SIZE];
//������
void led_task(void *pdata);  


//�������ȼ�
#define DISPLAY_TASK_PRIO	10
//�����ջ��С
#define DISPLAY_STK_SIZE	128
//�����ջ
OS_STK	DISPLAY_TASK_STK[DISPLAY_STK_SIZE];
//������
void display_task(void *pdata);

//START����
//�������ȼ�
#define START_TASK_PRIO		11
//�����ջ��С
#define START_STK_SIZE		128
//�����ջ
OS_STK START_TASK_STK[START_STK_SIZE];
//������
void start_task(void *pdata); 



int main(void)
{
	delay_init(168);       	//��ʱ��ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//�жϷ�������
	uart_init(115200);   	//���ڲ���������
	usart6_init(115200);  //��ʼ������6������Ϊ9600
	usart3_init(115200);  //��ʼ������3������Ϊ115200
	usart2_init(115200);
	usmart_dev.init(84);	//��ʼ��USMART
	LED_Init(); 			//LED��ʼ��
	KEY_Init();  			//������ʼ��
	
	
  CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_7tq,6,CAN_Mode_Normal);//CAN��ʼ������ģʽ,������500Kbps 
	
	InitDriverCmdQueue();
		
	FSMC_SRAM_Init(); 		//SRAM��ʼ��
	mymem_init(SRAMIN);  	//��ʼ���ڲ��ڴ��
	mymem_init(SRAMEX);  	//��ʼ���ⲿ�ڴ��
	mymem_init(SRAMCCM); 	//��ʼ��CCM�ڴ��
	
	
	
	OSInit(); 					//UCOS��ʼ��
	
	
	
	if(can_task_init())//Can��ʼ��
	{
		printf("main.c  Can init fail!!!!\r\n");
	}	
   
	InitDriver();
	printf("main.c  Init driver,Wait a moment......\r\n");

	
	
	//if(ultrasonic_task_init())//ultrasonic��ʼ��
	{
		printf("main.c  Ultrasonic init fail!!!!\r\n");	
	}	
	
	if(MPU6050_task_init())//MPU6050_task_init��ʼ��
	{
		printf("main.c  MPU6050_task_init init fail!!!!\r\n");	
	}	
	
	if(wifi_task_init())
	{
		printf("main.c  Wifi init fail!!!!\r\n");
	}
	
	
	if(lwip_comm_init()) 	//lwip��ʼ��
	{
		printf("main.c  Lwip init fail!!!!\r\n");	
	}
	else{
	
	 
		if(tcp_client_init()) 									//��ʼ��tcp_client(����tcp_client�߳�)
	  {
	 		printf("main.c  tcp_client init fail!!!!\r\n");	
	  }
		 if(tcp_server_init()) 									//��ʼ��tcp_server(����tcp_server�߳�)
	  {
	 		printf("main.c  tcp_server init fail!!!!\r\n");	
	  }
  }
	
	OSTaskCreate(start_task,(void*)0,(OS_STK*)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO);
	OSStart(); //����UCOS
}

//start����
void start_task(void *pdata)
{
	OS_CPU_SR cpu_sr;
	pdata = pdata ;
	
	OSStatInit();  			//��ʼ��ͳ������
	OS_ENTER_CRITICAL();  	//���ж�
	
	
#if	LWIP_DHCP
	lwip_comm_dhcp_creat();	//����DHCP����
#endif
	OSTaskCreate(led_task,(void*)0,(OS_STK*)&LED_TASK_STK[LED_STK_SIZE-1],LED_TASK_PRIO); 	//����LED����
	OSTaskCreate(key_task,(void*)0,(OS_STK*)&KEY_TASK_STK[KEY_STK_SIZE-1],KEY_TASK_PRIO); 	//����KEY����
	OSTaskCreate(display_task,(void*)0,(OS_STK*)&DISPLAY_TASK_STK[DISPLAY_STK_SIZE-1],DISPLAY_TASK_PRIO); //��ʾ����
	OSTaskSuspend(OS_PRIO_SELF); //����start_task����
	OS_EXIT_CRITICAL();  //���ж�
}

//��ʾ��ַ����Ϣ��������
void display_task(void *pdata)
{
	while(1)
	{ 
#if LWIP_DHCP									//������DHCP��ʱ��
		if(lwipdev.dhcpstatus != 0) 			//����DHCP
		{
			show_address(lwipdev.dhcpstatus );	//��ʾ��ַ��Ϣ
			OSTaskSuspend(OS_PRIO_SELF); 		//��ʾ���ַ��Ϣ�������������
		}
#else
		OSTaskSuspend(OS_PRIO_SELF); 			//��ʾ���ַ��Ϣ�������������
#endif //LWIP_DHCP
		OSTimeDlyHMSM(0,0,0,500);
	}
}

//key����
void key_task(void *pdata)
{
	u8 key; 
	u8 canbuf[8];
	while(1)
	{
		key = KEY_Scan(0);
		if(key==KEY0_PRES) //��������
		{
			tcp_server_flag |= LWIP_SEND_DATA; //���LWIP������Ҫ����
		}else if(key==KEY1_PRES)
		{
			tcp_client_flag |= LWIP_SEND_DATA; //���LWIP������Ҫ����;
		}
		key=CAN1_Receive_Msg(canbuf);
		if(key)//���յ�������
		{				
			printf("�յ���%s\r\n",canbuf);
		}
		
		OSTimeDlyHMSM(0,0,0,10);  //��ʱ10ms
	}
}

//led����
void led_task(void *pdata)
{
	while(1)
	{
		LED0 = !LED0;
		OSTimeDlyHMSM(0,0,0,500);  //��ʱ500ms
 	}
}


