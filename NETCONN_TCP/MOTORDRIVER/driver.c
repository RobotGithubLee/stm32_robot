#include "driver.h"
#include "delay.h"
#include "usart.h"

#define ID_LEFT 0x581
#define ID_RIGHT 0x582

#define P2P  0x0100
#define CHANGSTATUS  0x0480

const int DEC=2731;


int CruPosition=0;
int CruLeftVelocity=0;
int CruRightVelocity=0;

void setCruPosition(int pos){
	CruPosition=pos;
}

int getPosition(){
	return CruPosition;
}

void setCruVelocity(int Vel,int ID){
	if(ID==ID_LEFT)
	{
		CruLeftVelocity=Vel;
	}
	else if(ID==ID_RIGHT)
	{
		CruRightVelocity=Vel;
	}
}


int getLeftVelocity(void){
	return CruLeftVelocity;
}

int getRightVelocity(void){
	return CruRightVelocity;
}


int getVelocity(void){
	return (int)((CruLeftVelocity-CruRightVelocity)/2/DEC);
}




void FeedbackMessge(int ID, u8* data,u8 len){
	int size,cmd,msg=0;
	char * p = (char *)&msg;
	size=data[0];
	
	cmd=data[2];
	cmd<<=8;
	cmd+=data[1];
	cmd<<=8;
	cmd+=data[3];
	
	* (p+0) = data[4];
  * (p+1) = data[5];
  * (p+2) = data[6];
  * (p+3) = data[7];
	
	if(size==0x60){    //????
		//printf("driver.c:  set Sucess!\n\n");
	}
	else if((size==RBYTE1)||(size==RBYTE2)||(size==RBYTE4)){	 
        printf("driver.c:  CMD=%x\r\n",cmd);	
		switch(cmd){
			case GetCruPos:   //????
				setCruPosition(msg);
				printf("driver.c:  ID=%x,Position=%d\r\n",ID,msg);
				break;
			case GetVelocity:  //????
				setCruVelocity(msg,ID);
				printf("driver.c:  ID=%x,Velocity=%d\r\n",ID,msg);
				break;
			case GetCurrent: //????
				printf("driver.c:  ID=%x,Current=%d\r\n",ID,msg);
				break;				
			case GetVoltage:   //??????
				printf("driver.c:  ID=%x,Voltage=%d\r\n",ID,msg);
				break;
			case GetError:
				printf("ID=%x,Error=%d\r\n",ID,msg);
				break;
			case GetError2:
				printf("driver.c:  ID=%x,Error2=%d\r\n",ID,msg);
				break;
			case GetIniTCtrlMode:
				printf("driver.c:  ID=%x,Mode=%d\r\n",ID,msg);
				break;
			case GetCmdVelocity:
				printf("driver.c:  ID=%x,CmdVelocity=%d\r\n",ID,msg);
				break;
			case GetDevTemp:
				printf("driver.c:  ID=%x,DevTemp=%d\r\n",ID,msg);
				break;
			case GetDevName:
				printf("driver.c:  ID=%x,DevName=%d\r\n",ID,msg);
				break;
			case GetDevserialN:
				printf("driver.c:  ID=%x,DevserialN=%d\r\n",ID,msg);
				break;
			case GetDinStatues:
				printf("driver.c:  ID=%x,DinStatues=%d\r\n",ID,data[5]);
				break;
			case SetAVelocity:
				printf("driver.c:  ID=%x,SetAVelocity=%d\r\n",ID,msg);
				break;
			case SetDVelocity:
				printf("driver.c:  ID=%x,SetDVelocity=%d\r\n",ID,msg);
				break;
			case GetStauts:
				printf("driver.c:  ID=%x,Stauts=%d\r\n",ID,msg);
				break;
			default :
				break;
			}
		printf("\r\n\n");	
		}
	else{
		printf("driver.c:  error command!!!!, data=%s\n",data);
	}
}





void setSendFramData(unsigned char cmd, unsigned int data,int param,unsigned char*sendData){
	unsigned char *p = (unsigned char *)&data;
	char *pa = (char *)&param;
	
	sendData[0]=cmd;
	sendData[1]=*(p+1);
	sendData[2]=*(p+2);
	sendData[3]=*(p+0);
	
	sendData[4]=*(pa+0);
	sendData[5]=*(pa+1);
	sendData[6]=*(pa+2);
	sendData[7]=*(pa+3);
}



//Set Dev PowerOn
int MotorPowerOnFram(u8* sendFramData){
	setSendFramData(WBYTE2,SetPSW,0x0F,sendFramData);
	return 0;
}

//Set Dev is DisEnable
int DisEnableFram(u8* sendFramData){
	setSendFramData(WBYTE2,SetPSW,DISEnSMD,sendFramData);
	return 0;
}

//Set Dev is Enable
int EnableFram(u8* sendFramData){
	setSendFramData(WBYTE2,SetPSW,EnSMD,sendFramData);
	return 0;
}


//Get Dev's Voltage
int getVoltageFram(u8* sendFramData){
  setSendFramData(GETKEY,GetVoltage,0,sendFramData);
	return 0;
}

//Get Dev's Current
int getCurrentFram(u8* sendFramData){
  setSendFramData(GETKEY,GetCurrent,0,sendFramData);
	return 0;
}

//Get Dev's Error
int getErrorFram(u8* sendFramData){
  setSendFramData(GETKEY,GetError,0,sendFramData);
	return 0;
}

//Get Dev's Mode
int getModeFram(u8* sendFramData){
  setSendFramData(GETKEY,GetIniTCtrlMode,0,sendFramData);
	return 0;
}

//Get Dev's Stauts
int getStautsFram(u8* sendFramData){
  setSendFramData(GETKEY,GetStauts,0,sendFramData);
	return 0;
}


//Get Dev's Velocity
int getVelocityFram(u8* sendFramData){
  setSendFramData(GETKEY,GetVelocity,0,sendFramData);
	return 0;
}

//Get Dev's Position
int getPositionFram(u8* sendFramData){
  setSendFramData(GETKEY,GetCruPos,0,sendFramData);
	return 0;
}


//Get Dev's Temperature
int getTemperatureFram(u8* sendFramData){
  setSendFramData(GETKEY,GetDevTemp,0,sendFramData);
	return 0;
}


//Get Dev's Name:
int getDevNameFram(u8* sendFramData){
  setSendFramData(GETKEY,GetDevName,0,sendFramData);
	return 0;
}


//Get Dev's Num
int getDevNumFram(u8* sendFramData){
  setSendFramData(GETKEY,GetDevserialN,0,sendFramData);
	return 0;
}

//Get Dev's DinStatues
int getDinStatuesFram(u8* sendFramData){
  setSendFramData(GETKEY,GetDinStatues,0,sendFramData);
	return 0;
}




//Set Dev Stop:
int StopFram(u8* sendFramData){
  setSendFramData(SETKEY,GetDinStatues,0,sendFramData);
	return 0;
}

//Set Dev EmergencyStop
int EmergencyStopFram(u8* sendFramData){
  setSendFramData(WBYTE1,SetPSW,MOTOR_EMGC_STOP,sendFramData);
	return 0;
}

//Clear Dev's Error:
int ClearErrorFram(u8* sendFramData){
  setSendFramData(WBYTE1,SetPSW,CLEAR_ERROR,sendFramData);
	return 0;
}

//Set Dev's Mode
int setModeFram(u8* sendFramData,int Mode){
	setSendFramData(WBYTE1,SetMode,Mode,sendFramData);//???????
	return 0;
}

//Set Dev's Velocity
int setVelocityFram(int v,u8* sendFramData){
  setSendFramData(WBYTE4,SetVelocity,v*DEC,sendFramData);
	return 0;
}


//Set Dev's Position
int setPositionFram(int Pos,u8* sendFramData){
  setSendFramData(WBYTE4,DTargetPostion,Pos,sendFramData);
	return 0;
}


//Set Dev is TAcc
int setTVelocityFram(int Acc,u8* sendFramData){
  setSendFramData(WBYTE4,DTrapezoidvelocity,Acc*2731,sendFramData);
	return 0;
}

//Set Absolute Position Mode: 必须先后调用这两个函数
int setAbsolutePositionMode1(u8* sendFramData){;
  setSendFramData(WBYTE2,SetPSW,0x2F,sendFramData);
	return 0;		
}
int setAbsolutePositionMode2(u8* sendFramData){;
	setSendFramData(WBYTE2,SetPSW,0x3F,sendFramData);
	return 0;		
}

//Set Relative Position Mode
int setRelativePositionMode1(u8* sendFramData){
  setSendFramData(WBYTE2,SetPSW,0x4F,sendFramData);
	return 0;		
}
int setRelativePositionMode2(u8* sendFramData){	
	setSendFramData(WBYTE2,SetPSW,0x5F,sendFramData);
	return 0;		
}

