#ifndef __KINEMATICS_H
#define __KINEMATICS_H	 
#include "sys.h"

float getCurAngle();

float getCurVelocity();

float getCurAngleVel();
	
int Angle2Position(float Angle);

int getExpPosition(float w,float v);

int getExpRightVelocity(float v);

int getExpLeftVelocity(float v);



#endif 













