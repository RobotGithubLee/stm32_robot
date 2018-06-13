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
//���ڷ��ͻ����� 	
__align(8) u8 USART6_TX_BUF[USART6_MAX_SEND_LEN]; 	//���ͻ���,���USART6_MAX_SEND_LEN�ֽ�
#ifdef USART6_RX_EN   								//���ʹ���˽���   	  
//���ڽ��ջ����� 	
u8 USART6_RX_BUF[USART6_MAX_RECV_LEN]; 				//���ջ���,���USART6_MAX_RECV_LEN���ֽ�.

u16 USART6_RX_STA=0;   	 
void USART6_IRQHandler(void)
{
	u8 res;	    
	if(USART_GetITStatus(USART6, USART_IT_RXNE) != RESET)//���յ�����
	{	 
		res =USART_ReceiveData(USART6);	
		
		//UsartRcvRobotDataHandle(res);
		UsartRcvIMUDataHandle(res);
		//printf("%2x  ",res);
 }										 
}  
#endif	
//��ʼ��IO ����6
//bound:������	  
void usart6_init(u32 bound)
{  

	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE); //ʹ��GPIOAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6,ENABLE);//ʹ��USART6ʱ��

 	USART_DeInit(USART6);  //��λ����6
	
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_USART6); //GPIOA6����ΪUSART6
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_USART6); //GPIOA7����ΪUSART6	
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; //GPIOA6��GPIOA7��ʼ��
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOC,&GPIO_InitStructure); //��ʼ��GPIOA6����GPIOA7
	
	USART_InitStructure.USART_BaudRate = bound;//������ 
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  
	USART_Init(USART6, &USART_InitStructure); //��ʼ������6
 
	USART_Cmd(USART6, ENABLE);               //ʹ�ܴ��� 
	
  USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);//�����ж�   
	
	NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//��ռ���ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	TIM5_Int_Init(40-1,8400-1);		//10ms�ж�
	USART6_RX_STA=0;		//����
	TIM_Cmd(TIM5, DISABLE); //�رն�ʱ��5
  	

}



//����6,wifiSend ����
//ȷ��һ�η������ݲ�����USART6_MAX_SEND_LEN�ֽ�
void u6_Send(u8* buf,u16 len)  
{  
	u16 i;

	for(i=0;i<len;i++)//ѭ����������
	{
	  while(USART_GetFlagStatus(USART6,USART_FLAG_TC)==RESET);  //�ȴ��ϴδ������ 
		USART_SendData(USART6,buf[i]); 	 //�������ݵ�����6 
	}
}

/*
int UsartRcvRobotDataHandle(u8 Res){
	
	if((USART6_RX_STA&0x8000)==0)//����δ���
	{
		USART6_RX_BUF[USART6_RX_STA&0X3FFF]=Res ;
		printf("Buf[%d]=%2x",USART6_RX_STA,USART6_RX_BUF[USART6_RX_STA]);
		USART6_RX_STA++;
		if(USART6_RX_STA>EXTERN_BUFSIZE-2 && USART6_RX_BUF[EXTERN_BUFSIZE-2]+EXTERN_BUFSIZE==USART6_RX_STA)
		{
			MessageHandle(USART6_RX_BUF,USART6_RX_STA);
			USART6_RX_STA|=0x8000;	//��������� 
			printf("�������\r\n");
		}
	}
	else
	{
		if(Res==0xAA)
		{
			printf("��ʼ����\r\n");
			USART6_RX_STA=0;
			USART6_RX_BUF[USART6_RX_STA&0X3FFF]=Res ;
			printf("Buf%d=%2x",USART6_RX_STA,USART6_RX_BUF[USART6_RX_STA]);
			USART6_RX_STA++;
			USART6_RX_STA&=0x8000;	//��ʼ����
		}
	}
	return 0;
}
*/


int UsartRcvIMUDataHandle(u8 res){
	if((USART6_RX_STA&(1<<15))==0)//�������һ������,��û�б�����,���ٽ�����������
	{ 
		if(USART6_RX_STA<USART6_MAX_SEND_LEN)		//�����Խ�������
		{
			if(res==0x55||USART6_RX_STA>0)
			{
				USART6_RX_BUF[USART6_RX_STA++]=res;		//��¼���յ���ֵ	
			}
			if(USART6_RX_STA>32)
			{
				USART6_RX_STA|=1<<15;	//�������
				
			}				
		}else 
		{
			USART6_RX_STA|=1<<15;					//ǿ�Ʊ�ǽ������
		} 
	} 
}








