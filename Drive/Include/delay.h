#ifndef __DELAY_H
#define __DELAY_H

#include "stm32f4xx.h"

void Delay_Init(void);				//延时函数初始化
void Delay_ms(uint32_t nTime);	//毫秒延时函数
void Delay_us(uint32_t nTime);
void delay_us(uint32_t nTime);

#endif //__DELAY_H

