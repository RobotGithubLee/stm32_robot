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




//KEY任务
#define KEY_TASK_PRIO 		8
//任务堆栈大小
#define KEY_STK_SIZE		128	
//任务堆栈
OS_STK KEY_TASK_STK[KEY_STK_SIZE];
//任务函数
void key_task(void *pdata);   

//LED任务
//任务优先级
#define LED_TASK_PRIO		9
//任务堆栈大小
#define LED_STK_SIZE		64
//任务堆栈
OS_STK	LED_TASK_STK[LED_STK_SIZE];
//任务函数
void led_task(void *pdata);  


//任务优先级
#define DISPLAY_TASK_PRIO	10
//任务堆栈大小
#define DISPLAY_STK_SIZE	128
//任务堆栈
OS_STK	DISPLAY_TASK_STK[DISPLAY_STK_SIZE];
//任务函数
void display_task(void *pdata);

//START任务
//任务优先级
#define START_TASK_PRIO		11
//任务堆栈大小
#define START_STK_SIZE		128
//任务堆栈
OS_STK START_TASK_STK[START_STK_SIZE];
//任务函数
void start_task(void *pdata); 



int main(void)
{
	delay_init(168);       	//延时初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//中断分组配置
	uart_init(115200);   	//串口波特率设置
	usart6_init(115200);  //初始化串口6波特率为9600
	usart3_init(115200);  //初始化串口3波特率为115200
	usart2_init(115200);
	usmart_dev.init(84);	//初始化USMART
	LED_Init(); 			//LED初始化
	KEY_Init();  			//按键初始化
	
	
  CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_7tq,6,CAN_Mode_Normal);//CAN初始化环回模式,波特率500Kbps 
	
	InitDriverCmdQueue();
		
	FSMC_SRAM_Init(); 		//SRAM初始化
	mymem_init(SRAMIN);  	//初始化内部内存池
	mymem_init(SRAMEX);  	//初始化外部内存池
	mymem_init(SRAMCCM); 	//初始化CCM内存池
	
	
	
	OSInit(); 					//UCOS初始化
	
	
	
	if(can_task_init())//Can初始化
	{
		printf("main.c  Can init fail!!!!\r\n");
	}	
   
	InitDriver();
	printf("main.c  Init driver,Wait a moment......\r\n");

	
	
	//if(ultrasonic_task_init())//ultrasonic初始化
	{
		printf("main.c  Ultrasonic init fail!!!!\r\n");	
	}	
	
	if(MPU6050_task_init())//MPU6050_task_init初始化
	{
		printf("main.c  MPU6050_task_init init fail!!!!\r\n");	
	}	
	
	if(wifi_task_init())
	{
		printf("main.c  Wifi init fail!!!!\r\n");
	}
	
	
	if(lwip_comm_init()) 	//lwip初始化
	{
		printf("main.c  Lwip init fail!!!!\r\n");	
	}
	else{
	
	 
		if(tcp_client_init()) 									//初始化tcp_client(创建tcp_client线程)
	  {
	 		printf("main.c  tcp_client init fail!!!!\r\n");	
	  }
		 if(tcp_server_init()) 									//初始化tcp_server(创建tcp_server线程)
	  {
	 		printf("main.c  tcp_server init fail!!!!\r\n");	
	  }
  }
	
	OSTaskCreate(start_task,(void*)0,(OS_STK*)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO);
	OSStart(); //开启UCOS
}

//start任务
void start_task(void *pdata)
{
	OS_CPU_SR cpu_sr;
	pdata = pdata ;
	
	OSStatInit();  			//初始化统计任务
	OS_ENTER_CRITICAL();  	//关中断
	
	
#if	LWIP_DHCP
	lwip_comm_dhcp_creat();	//创建DHCP任务
#endif
	OSTaskCreate(led_task,(void*)0,(OS_STK*)&LED_TASK_STK[LED_STK_SIZE-1],LED_TASK_PRIO); 	//创建LED任务
	OSTaskCreate(key_task,(void*)0,(OS_STK*)&KEY_TASK_STK[KEY_STK_SIZE-1],KEY_TASK_PRIO); 	//创建KEY任务
	OSTaskCreate(display_task,(void*)0,(OS_STK*)&DISPLAY_TASK_STK[DISPLAY_STK_SIZE-1],DISPLAY_TASK_PRIO); //显示任务
	OSTaskSuspend(OS_PRIO_SELF); //挂起start_task任务
	OS_EXIT_CRITICAL();  //开中断
}

//显示地址等信息的任务函数
void display_task(void *pdata)
{
	while(1)
	{ 
#if LWIP_DHCP									//当开启DHCP的时候
		if(lwipdev.dhcpstatus != 0) 			//开启DHCP
		{
			show_address(lwipdev.dhcpstatus );	//显示地址信息
			OSTaskSuspend(OS_PRIO_SELF); 		//显示完地址信息后挂起自身任务
		}
#else
		OSTaskSuspend(OS_PRIO_SELF); 			//显示完地址信息后挂起自身任务
#endif //LWIP_DHCP
		OSTimeDlyHMSM(0,0,0,500);
	}
}

//key任务
void key_task(void *pdata)
{
	u8 key; 
	u8 canbuf[8];
	while(1)
	{
		key = KEY_Scan(0);
		if(key==KEY0_PRES) //发送数据
		{
			tcp_server_flag |= LWIP_SEND_DATA; //标记LWIP有数据要发送
		}else if(key==KEY1_PRES)
		{
			tcp_client_flag |= LWIP_SEND_DATA; //标记LWIP有数据要发送;
		}
		key=CAN1_Receive_Msg(canbuf);
		if(key)//接收到有数据
		{				
			printf("收到：%s\r\n",canbuf);
		}
		
		OSTimeDlyHMSM(0,0,0,10);  //延时10ms
	}
}

//led任务
void led_task(void *pdata)
{
	while(1)
	{
		LED0 = !LED0;
		OSTimeDlyHMSM(0,0,0,500);  //延时500ms
 	}
}


