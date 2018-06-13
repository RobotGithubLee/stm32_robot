#ifndef __USART6_H
#define __USART6_H	 
#include "sys.h"  
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//����3��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2014/3/29
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	   

#define USART6_MAX_RECV_LEN		400					//�����ջ����ֽ���
#define USART6_MAX_SEND_LEN		400					//����ͻ����ֽ���
#define USART6_RX_EN 			1					//0,������;1,����.

extern u8  USART6_RX_BUF[USART6_MAX_RECV_LEN]; 		//���ջ���,���USART6_MAX_RECV_LEN�ֽ�
extern u8  USART6_TX_BUF[USART6_MAX_SEND_LEN]; 		//���ͻ���,���USART6_MAX_SEND_LEN�ֽ�
extern u16 USART6_RX_STA;   						//��������״̬

void usart6_init(u32 bound);				//����2��ʼ�� 
void TIM6_Int_Init(u16 arr,u16 psc);
void u6_Send(uint8_t* buf,u16 len);
int UsartRcvRobotDataHandle(u8 Res);
	
#endif













