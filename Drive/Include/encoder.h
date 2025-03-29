#ifndef __ENCODER_H
#define __ENCODER_H

#include "stdint.h"

void Encoder_Init(void);
int16_t Encoder_Get_Right(void);
int16_t Encoder_Get_Left(void);
void Motor_Stop(void);

#endif //__PWM_H
