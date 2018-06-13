#ifndef __DRIVER_H
#define __DRIVER_H	 
#include "sys.h"	 


#define LEFT_ID  0x01 
#define RIGHT_ID 0x02
#define FRONT_ID 0x03
#define DRIVER_ID 0x0600


enum KINCOCMD{
		GetCruPos = 0x606300,    //????
		GetVelocity = 0x606c00,
		GetCurrent = 0x607800,
		GetVoltage = 0x60f712,
		GetError  = 0x260100,    //??????1
		GetError2  = 0x260200, //??????2
		GetIniTCtrlMode = 0x606000,
		GetStauts = 0x604100,
		GetCmdVelocity = 0x60ff00,
		GetDevTemp = 0x60f70b,
		GetDevName = 0x100800,
		GetDevserialN = 0x2ff800,
		GetDinStatues = 0x60fD00,
		
		SetPSW=0x604000,
		SetMode=0x606000,		
		SetVelocity = 0x60fF00,
		SetAVelocity = 0x608300,  //?????????
		SetDVelocity = 0x608400,  //?????????
		DTargetPostion = 0x607A00,   //????
		DTrapezoidvelocity  = 0x608100,   //????
};

		
enum POSBOUNDARY{
	POS_MAX=450000,
	POS_MIN=-250000,
	POS_MID = 50000,
	POS_MAX_SPEED = 6000,
};
		
		
 enum WRITE_CMD{
  SETKEY  = 0x08,  	
	WBYTE1  = 0x2f,  //?????1????
	WBYTE2  = 0x2b,  //?????2????
	WBYTE4  = 0x23,  //?????4????	
 };
 enum READ_CMD{
	GETKEY  = 0x40,	
	RBYTE1  = 0x4f,  //?????1????
	RBYTE2  = 0x4b,  //?????2????
	RBYTE4  = 0x43,  //?????4????	
 };
 
enum MODE_CMD{
	POSITION_MODE=0x01,
	VELOCITY_MODE=0x03,
};	

enum PSW{
	MOTOR_POWER_OFF=0x06,
	MOTOR_POWER_ON=0x0F,
	MOTOR_EMGC_STOP=0xB,
	CLEAR_ERROR=0x80,	
};	
		
enum ENCMD{	
	EnSMD =0x00103F,
	DISEnSMD= 0x000006,	
};

int getPosition(void);

int getLeftVelocity(void);
int getRightVelocity(void);
int getVelocity(void);
	

void FeedbackMessge(int ID, u8* data,u8 len);
	
int MotorPowerOnFram(u8* sendFramData);
int DisEnableFram(u8* sendFramData);
int EnableFram(u8* sendFramData);
int StopFram(u8* sendFramData);
int EmergencyStopFram(u8* sendFramData);
int ClearErrorFram(u8* sendFramData);

int getVoltageFram(u8* sendFramData);
int getCurrentFram(u8* sendFramData);
int getErrorFram(u8* sendFramData);
int getModeFram(u8* sendFramData);
int getStautsFram(u8* sendFramData);
int getVelocityFram(u8* sendFramData);
int getPositionFram(u8* sendFramData);
int getTemperatureFram(u8* sendFramData);




int setModeFram(u8* sendFramData,int Mode);
int setVelocityFram(int v,u8* sendFramData);
int setPositionFram(int Pos,u8* sendFramData);
int setTVelocityFram(int Acc,u8* sendFramData);

#endif 













