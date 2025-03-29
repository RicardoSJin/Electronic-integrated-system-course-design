#ifndef __SERIAL_H
#define __SERIAL_H

#include <stdint.h>

void USART3_Init(uint32_t bound);
void Serial_SendByte(uint8_t Byte);
void Serial_SendArray(uint8_t *Array, uint16_t Length);
void Serial_SendString(char *String);
uint8_t Serial_GetRxFlag(void);
uint8_t Serial_GetRxData(void);
uint8_t Serial_GetStop(void);
extern uint8_t Serial_Stop;
#endif
