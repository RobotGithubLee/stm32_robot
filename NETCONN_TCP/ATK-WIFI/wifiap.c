#include "includes.h"
#include "common.h"
#include "Queue.h"
#include "message.h"
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
							  
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 


extern struct interQueue *FMQ;


u8 atk_8266_wifiap_test(void)
{
	u8 ipbuf[16]; 	//IP缓存
	u8 *p;
	u16 t=999;		//加速第一次获取链接状态
	u16 rlen=0;
	u8 constate=0;	//连接状态
	STM sendMsg;
	u8 sendBuf[125];
	p=mymalloc(SRAMIN,32);							//申请32字节内存
	
	
	//服务器模式
	atk_8266_send_cmd("AT+CIPMUX=1","OK",20);   //0：单连接，1：多连接
	sprintf((char*)p,"AT+CIPSERVER=1,%s",(u8*)portnum);
	atk_8266_send_cmd(p,"OK",20);     //开启Server模式，端口号为8088

  delay_ms(200);
	atk_8266_get_wanip(ipbuf);//服务器模式,获取WAN IP
	sprintf((char*)p,"IP地址:%s 端口:%s",ipbuf,(u8*)portnum);
  printf("IP地址:%s 端口:%s",ipbuf,(u8*)portnum);
  USART3_RX_STA=0;
	
	while(1)
	{
		//Wifi发送数据
		if(FMQ->QueueCount>10&&(constate=='+'))
		{
			GetAndDelQueueinter(sizeof(STM), &sendMsg, FMQ);             //出队
			
			memset(sendBuf,0,MAX_RX_BUFSIZE);  	                   //数据清零

			memcpy(sendBuf,&sendMsg,sendMsg.len+EXTERN_BUFSIZE-1); //复制数据
				
			sendBuf[sendMsg.len+EXTERN_BUFSIZE-1]=sendMsg.Bcc;     //复制校验位
			
			u3_wifiSend(sendBuf,sendMsg.len+EXTERN_BUFSIZE);                               //发送WiFi数据               
		}					
				
	  delay_ms(10);	
		
		//接受数据
		if(USART3_RX_STA&0X8000)		                            //接收到一次数据了
		{ 
			rlen=USART3_RX_STA&0X7FFF;	                          //得到本次接收到的数据长度
			USART3_RX_BUF[rlen]=0;	                             	//添加结束符 
		
			if(strstr((const char*)USART3_RX_BUF, "IPD"))
			{
				p=(u8*)strchr((const char*)USART3_RX_BUF, 0xAA);

				if(p)
					MessageHandle(p,*(p+4)+EXTERN_BUFSIZE);
			}
			
			USART3_RX_STA=0;
			
			if(constate!='+')
				t=1000;		//状态为还未连接,立即更新连接状态
			else 
				t=0;                   //状态为已经连接了,10秒后再检查
		}  

		if(t==1000)//连续10秒钟没有收到任何数据,检查连接是不是还存在.
		{
			constate=atk_8266_consta_check();//得到连接状态
			if(constate!='+')
				printf("wifiap.c: wifi connection failed");//连接状态
			t=0;
		}

		t++;
		atk_8266_at_response(1);
	}
	
	myfree(SRAMIN,p);		//释放内存 
	return 0;		
} 







