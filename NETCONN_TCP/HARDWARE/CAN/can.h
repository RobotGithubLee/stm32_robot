#ifndef __CAN_H
#define __CAN_H	 
#include "sys.h"	 

#include "message.h"
#include "Queue.h"
#include "driver.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//CAN���� ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/7
//�汾��V1.0 
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 

	
//CAN1����RX0�ж�ʹ��
#define CAN1_RX0_INT_ENABLE	0		//0,��ʹ��;1,ʹ��.

#define LEFT_ID  0x01 
#define RIGHT_ID 0x02
#define FRONT_ID 0x03
#define DRIVER_ID 0x0600

extern void FeedbackMessge(int readID, u8 *data,u8 len);

u8 can_task_init(void);
	
u8 CAN1_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode);//CAN��ʼ��



u8 CAN1_Send_Msg(u8* msg,u8 len);						//��������

u8 CAN1_Receive_Msg(u8 *buf);							//��������

u8 SendCan(SCM sendMsg);

u8 ReadCan(u8 *buf,u32 *ID);
#endif

















