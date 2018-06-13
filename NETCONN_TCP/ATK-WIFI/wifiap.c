#include "includes.h"
#include "common.h"
#include "Queue.h"
#include "message.h"
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
							  
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 


extern struct interQueue *FMQ;


u8 atk_8266_wifiap_test(void)
{
	u8 ipbuf[16]; 	//IP����
	u8 *p;
	u16 t=999;		//���ٵ�һ�λ�ȡ����״̬
	u16 rlen=0;
	u8 constate=0;	//����״̬
	STM sendMsg;
	u8 sendBuf[125];
	p=mymalloc(SRAMIN,32);							//����32�ֽ��ڴ�
	
	
	//������ģʽ
	atk_8266_send_cmd("AT+CIPMUX=1","OK",20);   //0�������ӣ�1��������
	sprintf((char*)p,"AT+CIPSERVER=1,%s",(u8*)portnum);
	atk_8266_send_cmd(p,"OK",20);     //����Serverģʽ���˿ں�Ϊ8088

  delay_ms(200);
	atk_8266_get_wanip(ipbuf);//������ģʽ,��ȡWAN IP
	sprintf((char*)p,"IP��ַ:%s �˿�:%s",ipbuf,(u8*)portnum);
  printf("IP��ַ:%s �˿�:%s",ipbuf,(u8*)portnum);
  USART3_RX_STA=0;
	
	while(1)
	{
		//Wifi��������
		if(FMQ->QueueCount>10&&(constate=='+'))
		{
			GetAndDelQueueinter(sizeof(STM), &sendMsg, FMQ);             //����
			
			memset(sendBuf,0,MAX_RX_BUFSIZE);  	                   //��������

			memcpy(sendBuf,&sendMsg,sendMsg.len+EXTERN_BUFSIZE-1); //��������
				
			sendBuf[sendMsg.len+EXTERN_BUFSIZE-1]=sendMsg.Bcc;     //����У��λ
			
			u3_wifiSend(sendBuf,sendMsg.len+EXTERN_BUFSIZE);                               //����WiFi����               
		}					
				
	  delay_ms(10);	
		
		//��������
		if(USART3_RX_STA&0X8000)		                            //���յ�һ��������
		{ 
			rlen=USART3_RX_STA&0X7FFF;	                          //�õ����ν��յ������ݳ���
			USART3_RX_BUF[rlen]=0;	                             	//��ӽ����� 
		
			if(strstr((const char*)USART3_RX_BUF, "IPD"))
			{
				p=(u8*)strchr((const char*)USART3_RX_BUF, 0xAA);

				if(p)
					MessageHandle(p,*(p+4)+EXTERN_BUFSIZE);
			}
			
			USART3_RX_STA=0;
			
			if(constate!='+')
				t=1000;		//״̬Ϊ��δ����,������������״̬
			else 
				t=0;                   //״̬Ϊ�Ѿ�������,10����ټ��
		}  

		if(t==1000)//����10����û���յ��κ�����,��������ǲ��ǻ�����.
		{
			constate=atk_8266_consta_check();//�õ�����״̬
			if(constate!='+')
				printf("wifiap.c: wifi connection failed");//����״̬
			t=0;
		}

		t++;
		atk_8266_at_response(1);
	}
	
	myfree(SRAMIN,p);		//�ͷ��ڴ� 
	return 0;		
} 







