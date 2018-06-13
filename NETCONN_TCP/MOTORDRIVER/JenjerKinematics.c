#include "includes.h"
#include "ConvertData.h"
#include "JenjerKinematics.h"


#define PI 3.1415926

const float R=235.0;       //The radius of chassis
const float r=80.0;       //The radius of wheel

const float SIN60=0.888;

int getExpAngleVel(float w,float v,int* AngleVle)
{
	AngleVle[0]=w*R/r;
	AngleVle[1]=v*SIN60-w*R/r;
	AngleVle[2]=v*SIN60+w*R/r;
	return 0;
}

