#ifndef __RED_CONTROL_H
#define __RED_CONTROL_H

#include <stm32f4xx.h>
#include "sys.h" 

#define REMOTE_ID 0
#define RDATA 	PAin(3)	

void Remote_Init(void);
extern u16 Dval;
extern	u8 	RmtSta;	  	  
extern	u32 RmtRec;	//红外接收到的数据	   		    
extern	u8  RmtCnt;	//按键按下的次数	
u8 Remote_Scan(void);


#endif

