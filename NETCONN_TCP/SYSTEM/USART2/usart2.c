#include "delay.h"
#include "usart2.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	  
#include "timer.h"
#include "includes.h"
#include "message.h"
//////////////////////////////////////////////////////////////////////////////////	 
					  
////////////////////////////////////////////////////////////////////////////////// 	   

//���ڷ��ͻ����� 	
__align(8) u8 USART2_TX_BUF[USART2_MAX_SEND_LEN]; 	//���ͻ���,���USART2_MAX_SEND_LEN�ֽ�
#ifdef USART2_RX_EN   								//���ʹ���˽���   	  
//���ڽ��ջ����� 	
u8 USART2_RX_BUF[USART2_MAX_RECV_LEN]; 				//���ջ���,���USART2_MAX_RECV_LEN���ֽ�.
int UsartRcvIMUDataHandle2(u8 res);
u16 USART2_RX_STA=0;   	 
int UsartRcvRobotDataHandle(u8 Res);
void USART2_IRQHandler(void)
{
	u8 res;	    
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)//���յ�����
	{	 
 
	res =USART_ReceiveData(USART2);	
		//UsartRcvIMUDataHandle2(res);
	//printf("%2x\r\n",res);
	//	u2_Send(&res,1);
/*	if((USART2_RX_STA&(1<<15))==0)//�������һ������,��û�б�����,���ٽ�����������
	{ 
		if(USART2_RX_STA<USART2_MAX_RECV_LEN)		//�����Խ�������
		{
			TIM_SetCounter(TIM5,0);//���������        				 
			if(USART2_RX_STA==0)		
				TIM_Cmd(TIM5, ENABLE);  //ʹ�ܶ�ʱ��5
			USART2_RX_BUF[USART2_RX_STA++]=res;		//��¼���յ���ֵ	 
		}else 
		{
			USART2_RX_STA|=1<<15;					//ǿ�Ʊ�ǽ������
		} 
	}  	*/
		UsartRcvRobotDataHandle(res);
 }										 
}  
#endif	
//��ʼ��IO ����2
//bound:������	  
void usart2_init(u32 bound)
{  

	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE); //ʹ��GPIOAʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//ʹ��USART2ʱ��

 	USART_DeInit(USART2);  //��λ����2
	
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource5,GPIO_AF_USART2); //GPIOA2����ΪUSART2
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource6,GPIO_AF_USART2); //GPIOA3����ΪUSART2	
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6; //GPIOA2��GPIOA3��ʼ��
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOD,&GPIO_InitStructure); //��ʼ��GPIOA2����GPIOA3
	
	USART_InitStructure.USART_BaudRate = bound;//������ 
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  
	USART_Init(USART2, &USART_InitStructure); //��ʼ������2
 
	USART_Cmd(USART2, ENABLE);               //ʹ�ܴ��� 
	
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�����ж�   
	
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//��ռ���ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	//TIM5_Int_Init(100-1,8400-1);		//10ms�ж�
	USART2_RX_STA=0;		//����
	//TIM_Cmd(TIM5, DISABLE); //�رն�ʱ��6
  	

}



//����2,wifiSend ����
//ȷ��һ�η������ݲ�����USART2_MAX_SEND_LEN�ֽ�
void u2_Send(u8* buf,u16 len)  
{  
	u16 i;

	for(i=0;i<len;i++)//ѭ����������
	{
	  while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET);  //�ȴ��ϴδ������ 
		USART_SendData(USART2,buf[i]); 	 //�������ݵ�����2 
	}
}

int UsartRcvRobotDataHandle(u8 Res){
	
	if((USART2_RX_STA&0x8000)==0)//����δ���
	{
		USART2_RX_BUF[USART2_RX_STA&0X3FFF]=Res ;
		
		USART2_RX_STA++;
		if(USART2_RX_STA>EXTERN_BUFSIZE-2 && USART2_RX_BUF[EXTERN_BUFSIZE-2]+EXTERN_BUFSIZE==USART2_RX_STA)
		{
			MessageHandle(USART2_RX_BUF,USART2_RX_STA);
			USART2_RX_STA|=0x8000;	//��������� 
		}
	}
	else
	{
		if(Res==0xAA)
		{
			USART2_RX_STA=0;
			USART2_RX_BUF[USART2_RX_STA&0X3FFF]=Res ;
			USART2_RX_STA++;
		}
	}
	return 0;
}


int UsartRcvIMUDataHandle2(u8 res){
	if((USART2_RX_STA&(1<<15))==0)//�������һ������,��û�б�����,���ٽ�����������
	{ 
		if(USART2_RX_STA<USART2_MAX_SEND_LEN)		//�����Խ�������
		{
			if(res==0xAA||USART2_RX_STA>0)
			{
				USART2_RX_BUF[USART2_RX_STA++]=res;		//��¼���յ���ֵ	
			}
			if(USART2_RX_STA>9)
			{
				USART2_RX_STA|=1<<15;	//�������
				USART2_RX_STA = 0;
			}				
		}else 
		{
			USART2_RX_STA|=1<<15;					//ǿ�Ʊ�ǽ������
		} 
	} 
	return 0;
}










