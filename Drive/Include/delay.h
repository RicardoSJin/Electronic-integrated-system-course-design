#ifndef __DELAY_H
#define __DELAY_H

#include "stm32f4xx.h"

void Delay_Init(void);				//��ʱ������ʼ��
void Delay_ms(uint32_t nTime);	//������ʱ����
void Delay_us(uint32_t nTime);
void delay_us(uint32_t nTime);

#endif //__DELAY_H

