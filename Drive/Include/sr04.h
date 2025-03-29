#ifndef __SR04_H
#define __SR04_H

#include "stm32f4xx.h"
#include "sys.h"
#include "delay.h"
#include "pwm.h"

/*******************************
����˵����
PA2  -- TRIG(���)
PA3  -- ECHO(����)
*******************************/

//#define TRIG   PAout(2)
//#define ECHO   PAin(3)
#define TRIG   PCout(2)
#define ECHO   PCin(3)

void Sr04_Init(void);
u16 Get_Sr04_Value(void);
int control_if_move(void);

#endif
