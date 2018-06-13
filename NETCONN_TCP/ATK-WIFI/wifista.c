#include "common.h"
#include "stdlib.h"
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//ATK-ESP8266 WIFIģ�� WIFI STA��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2015/4/3
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 

//ATK-ESP8266 WIFI STA����
//���ڲ���TCP/UDP����
//����ֵ:0,����
//    ����,�������
u8 netpro=0;	//����ģʽ
u8 atk_8266_wifista_test(void)
{
	//u8 netpro=0;	//����ģʽ
	u8 key;
	u8 ipbuf[16]; 	//IP����
	u8 *p;
	u16 t=999;		//���ٵ�һ�λ�ȡ����״̬
	u8 res=0;
	u16 rlen=0;
	u8 constate=0;	//����״̬
	p=mymalloc(SRAMIN,32);							//����32�ֽ��ڴ�
	atk_8266_send_cmd("AT+CWMODE=1","OK",50);		//����WIFI STAģʽ
	atk_8266_send_cmd("AT+RST","OK",20);		//DHCP�������ر�(��APģʽ��Ч) 
	delay_ms(1000);         //��ʱ3S�ȴ������ɹ�
	delay_ms(1000);
	delay_ms(1000);
	delay_ms(1000);
	//�������ӵ���WIFI��������/���ܷ�ʽ/����,�⼸��������Ҫ�������Լ���·�������ý����޸�!! 
	sprintf((char*)p,"AT+CWJAP=\"%s\",\"%s\"",wifista_ssid,wifista_password);//�������߲���:ssid,����
	while(atk_8266_send_cmd(p,"WIFI GOT IP",300));					//����Ŀ��·����,���һ��IP
PRESTA:
	netpro|=atk_8266_netpro_sel(50,30,(u8*)ATK_ESP8266_CWMODE_TBL[0]);	//ѡ������ģʽ
	if(netpro&0X02)   //UDP
	{
				sprintf((char*)p,"AT+CIPSTART=\"UDP\",\"%s\",%s",ipbuf,(u8*)portnum);    //����Ŀ��UDP������
				delay_ms(200);
				atk_8266_send_cmd("AT+CIPMUX=0","OK",20);  //������ģʽ
				delay_ms(200);
		
				while(atk_8266_send_cmd(p,"OK",500));
	}
	else     //TCP
	{
		if(netpro&0X01)     //TCP Client    ͸��ģʽ����
		{
			
		
			sprintf((char*)p,"AT+CIPSTART=\"TCP\",\"%s\",%s",ipbuf,(u8*)portnum);    //����Ŀ��TCP������
			while(atk_8266_send_cmd(p,"OK",200))
			{ 
					key=KEY_Scan(0);
					if(key==WKUP_PRES)goto PRESTA;
			}	
			atk_8266_send_cmd("AT+CIPMODE=1","OK",200);      //����ģʽΪ��͸��			
		}
		else					//TCP Server
		{
				//Show_Str(0,30,"ATK-ESP WIFI-STA ����",16,240); 
				//Show_Str(30,50,200,16,"��������ATK-ESPģ��,���Ե�...",12,0);
				atk_8266_send_cmd("AT+CIPMUX=1","OK",20);   //0�������ӣ�1��������
				sprintf((char*)p,"AT+CIPSERVER=1,%s",(u8*)portnum);    //����Serverģʽ(0���رգ�1����)���˿ں�Ϊportnum
				atk_8266_send_cmd(p,"OK",50);    
		}
	}
			
			atk_8266_get_wanip(ipbuf);//������ģʽ,��ȡWAN IP
			sprintf((char*)p,"IP��ַ:%s �˿�:%s",ipbuf,(u8*)portnum);
			USART3_RX_STA=0;
			while(1)
			{
				key=KEY_Scan(0);
				if(key==WKUP_PRES)			//WK_UP �˳�����		 
				{ 
					res=0;					
					atk_8266_quit_trans();	//�˳�͸��
					atk_8266_send_cmd("AT+CIPMODE=0","OK",20);   //�ر�͸��ģʽ
					break;												 
				}
				else if(key==KEY0_PRES)	//KEY0 �������� 
				{
				
					if((netpro==3)||(netpro==2))   //UDP
					{
						sprintf((char*)p,"ATK-8266%s����%02d\r\n",ATK_ESP8266_WORKMODE_TBL[netpro],t/10);//��������
						//Show_Str(30+54,100,200,12,p,12,0);
						atk_8266_send_cmd("AT+CIPSEND=25","OK",200);  //����ָ�����ȵ�����
						delay_ms(200);
						atk_8266_send_data(p,"OK",100);  //����ָ�����ȵ�����
					}
					else if((netpro==1))   //TCP Client
					{
						atk_8266_quit_trans();
						atk_8266_send_cmd("AT+CIPSEND","OK",20);         //��ʼ͸��           
						sprintf((char*)p,"ATK-8266%s����%d\r\n",ATK_ESP8266_WORKMODE_TBL[netpro],t/10);//��������
						//Show_Str(30+54,100,200,12,p,12,0);
						u3_printf("%s",p);
					}
					else    //TCP Server
					{
						sprintf((char*)p,"ATK-8266%s����%02d\r\n",ATK_ESP8266_WORKMODE_TBL[netpro],t/10);//��������
						//Show_Str(30+54,100,200,12,p,12,0);
						atk_8266_send_cmd("AT+CIPSEND=0,25","OK",200);  //����ָ�����ȵ�����
						delay_ms(200);
						atk_8266_send_data(p,"OK",100);  //����ָ�����ȵ�����
					}
				}else;
			
				
				t++;
				delay_ms(10);
				if(USART3_RX_STA&0X8000)		//���յ�һ��������
				{ 
					rlen=USART3_RX_STA&0X7FFF;	//�õ����ν��յ������ݳ���
					USART3_RX_BUF[rlen]=0;		//��ӽ����� 
					printf("%s",USART3_RX_BUF);	//���͵�����   
					sprintf((char*)p,"�յ�%d�ֽ�,��������",rlen);//���յ����ֽ��� 
					
					USART3_RX_STA=0;
					if(constate!='+')t=1000;		//״̬Ϊ��δ����,������������״̬
					else t=0;                   //״̬Ϊ�Ѿ�������,10����ټ��
				}  
				if(t==1000)//����10����û���յ��κ�����,��������ǲ��ǻ�����.
				{

					constate=atk_8266_consta_check();//�õ�����״̬
					if(constate!='+')
						printf("WIFI TCP ����ʧ��");
					t=0;
				}
				if((t%20)==0)LED0=!LED0;
				atk_8266_at_response(1);
			}
	myfree(SRAMIN,p);		//�ͷ��ڴ� 
	return res;		
} 




























