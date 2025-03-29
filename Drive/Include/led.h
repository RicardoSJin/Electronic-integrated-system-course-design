#ifndef __LED_H
#define __LED_H

#include "stm32f4xx.h"

/*---------------------- LED���ú� ------------------------*/

#define LED1_PIN             GPIO_Pin_7       		 // LED1 ����      
#define LED1_PORT            GPIOG                  // LED1 GPIO�˿�     
#define LED1_CLK             RCC_AHB1Periph_GPIOG	 // LED1 GPIO�˿�ʱ��

/*---------------------- LED���ƺ� ------------------------*/
					
#define LED1_OFF 	  GPIO_ResetBits(LED1_PORT,LED1_PIN)			// ����͵�ƽ������LED1	
#define LED1_ON 	  GPIO_SetBits(LED1_PORT,LED1_PIN)				// ����ߵ�ƽ���ر�LED1	
#define LED1_Toggle	GPIO_ToggleBits(LED1_PORT, LED1_PIN)		// LED״̬��ת


/*---------------------- �������� ----------------------------*/

void LED_Init(void);	//LED��ʼ������

#endif //__LED_H

