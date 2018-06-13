#include "common.h"

//ATK-ESP8266 AP+STA模式测试
//用于测试TCP/UDP连接
//返回值:0,正常
//    其他,错误代码
u8 atk_8266_apsta_test(void)
{
	u8 netpro;
	u8 key=0;
	u8 ipbuf[16]; 	//IP缓存
	u8 *p;
	u16 t=999;		//加速第一次获取链接状态
	u8 res=0;
	u16 rlen=0;
	u8 constate=0;	//连接状态
	p=mymalloc(SRAMIN,100);							//申请32字节内存
	atk_8266_send_cmd("AT+CWMODE=3","OK",50);		//设置WIFI AP+STA模式
//	atk_8266_send_cmd("AT+RST","OK",20);		//重启模块 
//	delay_ms(1000);         //延时2S等待重启成功
//	delay_ms(1000);
//	delay_ms(1000);
	//设置模块AP模式的WIFI网络名称/加密方式/密码，这几个参数看自己喜好设置
	sprintf((char*)p,"AT+CWSAP=\"%s\",\"%s\",1,4",wifiap_ssid,wifiap_password);//设置无线参数:ssid,密码
	atk_8266_send_cmd(p,"OK",1000);					//设置AP模式参数
	//设置连接到的WIFI网络名称/加密方式/密码,这几个参数需要根据您自己的路由器设置进行修改!! 
	sprintf((char*)p,"AT+CWJAP=\"%s\",\"%s\"",wifista_ssid,wifista_password);//设置无线参数:ssid,密码
	atk_8266_send_cmd(p,"WIFI GOT IP",1000);						//连接目标路由器，并获得IP
	while(atk_8266_send_cmd("AT+CIFSR","STAIP",20));   //检测是否获得STA IP
	while(atk_8266_send_cmd("AT+CIFSR","APIP",20));   //检测是否获得AP IP
//	while((atk_8266_consta_check()-'2'));   //获得IP

	
	atk_8266_send_cmd("AT+CIPMUX=1","OK",50);   //0：单连接，1：多连接
	delay_ms(500);
	sprintf((char*)p,"AT+CIPSERVER=1,%s",(u8*)portnum);
	atk_8266_send_cmd(p,"OK",50);     //开启Server模式，端口号为8088
	delay_ms(500);
	atk_8266_send_cmd("AT+CIPSTO=1200","OK",50);     //设置服务器超时时间
	
PRESTA:
	netpro=atk_8266_netpro_sel(50,30,(u8*)ATK_ESP8266_CWMODE_TBL[0]);	//AP+STA模式网络模式选择
	if(netpro&0X02)   //STA UDP
	{
			
				sprintf((char*)p,"AT+CIPSTART=0,\"UDP\",\"%s\",%s",ipbuf,(u8*)portnum);    //配置目标UDP服务器,及ID号，STA模式下为0
				
				
				atk_8266_send_cmd(p,"OK",200);
				netpro=atk_8266_mode_cofig(netpro);     //AP模式网络模式配置			
	}
	else     //TCP
	{
		if(netpro&0X01)     //STA TCP Client  
		{
			
			sprintf((char*)p,"AT+CIPSTART=0,\"TCP\",\"%s\",%s",ipbuf,(u8*)portnum);    //配置目标TCP服务器,及ID号，STA模式下为0
			while(atk_8266_send_cmd(p,"OK",200))
			{
				key=KEY_Scan(0);
				if(key==WKUP_PRES)goto PRESTA;
			}	
			netpro=atk_8266_mode_cofig(netpro);     //AP模式网络模式配置	
		}
		else netpro=atk_8266_mode_cofig(netpro);     //TCP SERVER不用配置
	}
	
	

	atk_8266_get_ip(15,65);//STA+AP模式,获取 IP,并显示	
	
	

	USART3_RX_STA=0;
	while(1)
	{
		key=KEY_Scan(0);
		if(key==WKUP_PRES)			//WK_UP 退出测试		 
		{ 
			res=0;	
			break;												 
		}
		else if(key==KEY0_PRES)	//KEY0 向ID0发送数据 
		{
			  sprintf((char*)p,"ATK-8266模块ID0发数据%02d\r\n",t/10);//测试数据
				atk_8266_send_cmd("AT+CIPSEND=0,25","OK",200);  //发送指定长度的数据
				delay_ms(200);
				atk_8266_send_data(p,"OK",100);  //发送指定长度的数据
		}
		else if(key==KEY1_PRES)	//KEY1 向ID1发送数据 
		{
			  sprintf((char*)p,"ATK-8266模块ID1发数据%02d\r\n",t/10);//测试数据
				atk_8266_send_cmd("AT+CIPSEND=1,25","OK",200);  //发送指定长度的数据
				delay_ms(200);
				atk_8266_send_data(p,"OK",100);  //发送指定长度的数据
		}
			
	 t++;
	 delay_ms(10);
	 if(USART3_RX_STA&0X8000)		//接收到一次数据了
	{ 
		rlen=USART3_RX_STA&0X7FFF;	//得到本次接收到的数据长度
		USART3_RX_BUF[rlen]=0;		//添加结束符 
		printf("%s",USART3_RX_BUF);	//发送到串口   
		sprintf((char*)p,"收到%d字节,内容如下",rlen);//接收到的字节数 

		USART3_RX_STA=0;
		if(constate!='+')t=1000;		//状态为还未连接,立即更新连接状态
		else t=0;                   //状态为已经连接了,10秒后再检查
	}  
	if(t==1000)//连续10秒钟没有收到任何数据,检查连接是不是还存在.
	{
		constate=atk_8266_consta_check();//得到连接状态
		t=0;
	}
	if((t%20)==0)LED0=!LED0;
	atk_8266_at_response(1);
}
	myfree(SRAMIN,p);		//释放内存 
	return res;		
} 


























