#include "includes.h"
#include "ConvertData.h"
#include "kinematics.h"
#include "driver.h"




#define PI 3.1415926

const int T=1;              //Control cycle

const float Length=556.0;   //The length between front wheel and back wheel
const float Width=610.0/2;    //The Width between the two back wheel



const float L0=111.51;     /*      /\        */
const float L1=125.858;    /*     /   \L2    */
const float L2=65.0;       /* L1 /     |     */
const float L3=75.0;       /*   /      |L3   */
                           /*  /_ __ __|     */
													 /*     L0         */

const float theta0=62.61*PI/180; 

const float  Millimeter2Pulse=8545.175;

const float  vRatioa=30*60/2/PI/150;     //mm/s->r/min,  radio=30,  1 minute=60s,  2PI=1 roud  R=160



float getCurAngle(){
	float Lx,sL5,CurAngle,theta1,theta2;
	Lx=L0-getPosition()/Millimeter2Pulse;
	sL5=Lx*Lx+L3*L3;
	theta1=atan(L3/Lx);
	theta2=acos((L1*L1+sL5-L2*L2)/(2*L1*sqrt(sL5)));
	CurAngle=theta1+theta2-theta0;
	return CurAngle;
}


float getCurVelocity()
{
	return getVelocity()/vRatioa;
}



int Angle2Position(float Angle)
{
	float Lx1,Lx2,tempL;
	int pos=0;
	if(Angle>0.279)
		Angle=0.279;
	else if(Angle<-0.279)
		Angle=-0.279;
	Lx1=L1*cos(Angle+theta0);
	tempL=L1*sin(Angle+theta0)-L3;
	Lx2=sqrt(L2*L2-tempL*tempL);
	pos=(int)((L0-Lx1-Lx2)*Millimeter2Pulse);	
	return pos;
}


int getExpPosition(float w,float v)
{
	int pos;
	if(fabs(v)<0.0001)
	{	
		pos=0;
	}
	else
	{
		pos=Angle2Position(asin(w*Length/v));
	}
	return pos;
}



int getExpLeftVelocity(float v)
{
	float CurAngle=getCurAngle();
	return (int)(v*vRatioa*(cos(CurAngle)-Width*sin(CurAngle)/Length));
}


int getExpRightVelocity(float v)
{
	float CurAngle=getCurAngle();
	return (int)(-v*vRatioa*(cos(CurAngle)+Width*sin(CurAngle)/Length));
}



float getCurAngleVel()
{
	float w=getCurVelocity()*tan(getCurAngle())/Length;
	return w;
}



