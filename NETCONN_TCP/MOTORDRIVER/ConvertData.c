
#include "ConvertData.h"

u8 getXorCheck(u8* data, u8 len)
{
	u8 i,Bcc=0;
  for(i=0;i<len;i++)
	{
		Bcc^=data[i];
	}
	return Bcc;
}



short  GetMedianNum(short * bArray, int iFilterLen)  
{  
    int i,j;
    short bTemp;  
       
    for (j = 0; j < iFilterLen - 1; j ++)  
    {  
        for (i = 0; i < iFilterLen - j - 1; i ++)  
        {  
            if (bArray[i] > bArray[i + 1])  
            {  
                bTemp = bArray[i];  
                bArray[i] = bArray[i + 1];  
                bArray[i + 1] = bTemp;  
            }  
        }  
    }  
      

		//ÆæÊý
    if ((iFilterLen & 1) > 0)  
    {  
        bTemp = bArray[(iFilterLen - 1) / 2];  
    }  
    else //Å¼Êý
    {   
        bTemp = (bArray[iFilterLen / 2] + bArray[iFilterLen / 2 -1]) / 2;  
    }  
  
    return bTemp;  
}  




int hexbyte2int(u8* hex){
	int d=0;
	d=hex[0];
	d<<=8;
	d+=hex[1];
	d<<=8;
	d+=hex[2];
	d<<=8;
	d+=hex[3];
	return d;
}

short hexbyte2short(u8* hex){
	short d=0;
	d=hex[0];
	d<<=8;
	d+=hex[1];
	return d;
}


void short2hexbyte(short data,u8* hex){
	u8 *p = (u8 *)&data;
	hex[0]=*(p+1);
	hex[1]=*(p+0);
}

void int2hexbyte(short data,u8* hex){
	u8 *p = (u8 *)&data;
	hex[0]=*(p+0);
	hex[1]=*(p+1);
	hex[2]=*(p+2);
	hex[3]=*(p+3);
}
