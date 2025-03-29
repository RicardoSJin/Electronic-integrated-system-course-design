#ifndef __PWM_H
#define __PWM_H

#include "stdint.h"

void TIM134_PWM_Init(void);
void PWM_SetCompare3(uint16_t Compare);
void Motor_R_F_SPEED(uint16_t Speed,uint8_t dir);
void Motor_L_F_SPEED(uint16_t Speed2,uint8_t dir2);
void Motor_Stop(void);

#endif //__PWM_H
