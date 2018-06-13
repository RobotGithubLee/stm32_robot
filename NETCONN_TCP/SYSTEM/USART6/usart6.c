#include "delay.h"
#include "usart6.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	  
#include "timer.h"
#include "includes.h"
#include "message.h"
//////////////////////////////////////////////////////////////////////////////////	 
					  
////////////////////////////////////////////////////////////////////////////////// 	   
int UsartRcvIMUDataHandle(u8 res);
//串口发送缓存区 	
__align(8) u8 USART6_TX_BUF[USART6_MAX_SEND_LEN]; 	//发送缓冲,最大USART6_MAX_SEND_LEN字节
#ifdef USART6_RX_EN   								//如果使能了接收   	  
//串口接收缓存区 	
u8 USART6_RX_BUF[USART6_MAX_RECV_LEN]; 				//接收缓冲,最大USART6_MAX_RECV_LEN个字节.

u16 USART6_RX_STA=0;   	 
void USART6_IRQHandler(void)
{
	u8 res;	    
	if(USART_GetITStatus(USART6, USART_IT_RXNE) != RESET)//接收到数据
	{	 
		res =USART_ReceiveData(USART6);	
		
		//UsartRcvRobotDataHandle(res);
		UsartRcvIMUDataHandle(res);
		//printf("%2x  ",res);
 }										 
}  
#endif	
//初始化IO 串口6
//bound:波特率	  
void usart6_init(u32 bound)
{  

	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE); //使能GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6,ENABLE);//使能USART6时钟

 	USART_DeInit(USART6);  //复位串口6
	
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_USART6); //GPIOA6复用为USART6
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_USART6); //GPIOA7复用为USART6	
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; //GPIOA6和GPIOA7初始化
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOC,&GPIO_InitStructure); //初始化GPIOA6，和GPIOA7
	
	USART_InitStructure.USART_BaudRate = bound;//波特率 
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  
	USART_Init(USART6, &USART_InitStructure); //初始化串口6
 
	USART_Cmd(USART6, ENABLE);               //使能串口 
	
  USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);//开启中断   
	
	NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	TIM5_Int_Init(40-1,8400-1);		//10ms中断
	USART6_RX_STA=0;		//清零
	TIM_Cmd(TIM5, DISABLE); //关闭定时器5
  	

}



//串口6,wifiSend 函数
//确保一次发送数据不超过USART6_MAX_SEND_LEN字节
void u6_Send(u8* buf,u16 len)  
{  
	u16 i;

	for(i=0;i<len;i++)//循环发送数据
	{
	  while(USART_GetFlagStatus(USART6,USART_FLAG_TC)==RESET);  //等待上次传输完成 
		USART_SendData(USART6,buf[i]); 	 //发送数据到串口6 
	}
}

/*
int UsartRcvRobotDataHandle(u8 Res){
	
	if((USART6_RX_STA&0x8000)==0)//接收未完成
	{
		USART6_RX_BUF[USART6_RX_STA&0X3FFF]=Res ;
		printf("Buf[%d]=%2x",USART6_RX_STA,USART6_RX_BUF[USART6_RX_STA]);
		USART6_RX_STA++;
		if(USART6_RX_STA>EXTERN_BUFSIZE-2 && USART6_RX_BUF[EXTERN_BUFSIZE-2]+EXTERN_BUFSIZE==USART6_RX_STA)
		{
			MessageHandle(USART6_RX_BUF,USART6_RX_STA);
			USART6_RX_STA|=0x8000;	//接收完成了 
			printf("接收完成\r\n");
		}
	}
	else
	{
		if(Res==0xAA)
		{
			printf("开始接收\r\n");
			USART6_RX_STA=0;
			USART6_RX_BUF[USART6_RX_STA&0X3FFF]=Res ;
			printf("Buf%d=%2x",USART6_RX_STA,USART6_RX_BUF[USART6_RX_STA]);
			USART6_RX_STA++;
			USART6_RX_STA&=0x8000;	//开始接收
		}
	}
	return 0;
}
*/


int UsartRcvIMUDataHandle(u8 res){
	if((USART6_RX_STA&(1<<15))==0)//接收完的一批数据,还没有被处理,则不再接收其他数据
	{ 
		if(USART6_RX_STA<USART6_MAX_SEND_LEN)		//还可以接收数据
		{
			if(res==0x55||USART6_RX_STA>0)
			{
				USART6_RX_BUF[USART6_RX_STA++]=res;		//记录接收到的值	
			}
			if(USART6_RX_STA>32)
			{
				USART6_RX_STA|=1<<15;	//接收完成
				
			}				
		}else 
		{
			USART6_RX_STA|=1<<15;					//强制标记接收完成
		} 
	} 
}








