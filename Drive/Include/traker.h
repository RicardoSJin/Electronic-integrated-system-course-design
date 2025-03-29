#ifndef __APP_USART2_H_
#define __APP_USART2_H_


#include <stdlib.h>
#include "stdio.h"
#include "string.h"
#include "stm32f4xx.h"

extern u8 IR_Data_number[];
extern u16 IR_Data_Anglo[];
extern u8 g_Amode_Data ;//模拟型标志
extern u8 g_Dmode_Data ;//数字型标志

void Deal_IR_Usart(u8 rxtemp);
void Deal_Usart_Data(void);
void Deal_Usart_AData(void);
void send_control_data(u8 adjust,u8 aData,u8 dData);
void splitString(char* mystrArray[],char *str, const char *delimiter) ;

void UART4_init(u32 baudrate);
void UART4_Send_U8(uint8_t ch);
void UART4_Send_ArrayU8(uint8_t *BufferPtr, uint16_t Length);
void TRAKER_Init(void);
void TRAKER_SERVICE(void);
#endif
