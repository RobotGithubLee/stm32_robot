#ifndef __CONVERDATA_H
#define __CONVERDATA_H	
#include "sys.h"	  

u8 getXorCheck(u8* data, u8 len);

int hexbyte2int(u8* hex);

short hexbyte2short(u8* hex);


void short2hexbyte(short data,u8* hex);

void int2hexbyte(short data,u8* hex);

short  GetMedianNum(short * bArray, int iFilterLen);

#endif
