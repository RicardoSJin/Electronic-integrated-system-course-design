#include <stm32f4xx.h>
#include "red_control.h"
#include "led.h"   
#include "delay.h"
#include "pwm.h"
#include "encoder.h"
#include "timer.h"
#include "gpio.h"
#include "usart.h"
#include "lcd_spi_169.h"
#include "lcd_test.h"
#include "sr04.h"
#include "inf_image.h"
#include "pstwo.h"
#include "serial.h"
#include "traker.h"

void Remote_Init(void)   
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);  // GPIOAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9, ENABLE);    // TIM9ʱ��
	
	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_TIM9); 
	
	
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;        // ����ģʽ
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;      // �������
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;    // ��������
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct = {0};
	TIM_TimeBaseStruct.TIM_Prescaler = 168-1;       // ��Ƶϵ����84MHzϵͳʱ�ӡ�1MHz����Ƶ�ʣ�
	TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStruct.TIM_Period = 10000-1;          // ARR���ֵ
	TIM_TimeBaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM9, &TIM_TimeBaseStruct);
	
	TIM_ICInitTypeDef TIM_ICInitStruct = {0};
	TIM_ICInitStruct.TIM_Channel = TIM_Channel_2;    // CH2��ӦPA3
	TIM_ICInitStruct.TIM_ICPolarity = TIM_ICPolarity_Rising;  // �����ش���
	TIM_ICInitStruct.TIM_ICSelection = TIM_ICSelection_DirectTI; // ֱ��ӳ�䵽TI2
	TIM_ICInitStruct.TIM_ICPrescaler = TIM_ICPSC_DIV1;        // ����Ƶ
	TIM_ICInitStruct.TIM_ICFilter = 0x0F;                      // ���˲�
	TIM_ICInit(TIM9, &TIM_ICInitStruct);
	
	
	NVIC_InitTypeDef NVIC_InitStruct = {0};
	NVIC_InitStruct.NVIC_IRQChannel = TIM1_BRK_TIM9_IRQn;  // TIM9�ж�ͨ��
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	

	TIM_ITConfig(TIM9, TIM_IT_Update|TIM_IT_CC2, ENABLE);          // ����CH2�����ж�
	TIM_ClearITPendingBit(TIM9,TIM_IT_Update | TIM_IT_CC2);
	TIM_Cmd(TIM9, ENABLE);
}

u16 Dval;		//�½���ʱ��������ֵ
u8 	RmtSta=0;	  	  
u32 RmtRec=0;	//������յ�������	   		    
u8  RmtCnt=0;	//�������µĴ���	
uint8_t a=0;
u8 Remote_Scan(void)
{        
	u8 sta=0;       
    u8 t1,t2;  
	if(RmtSta&(1<<6))//�õ�һ��������������Ϣ��
	{ 
	    t1=RmtRec>>24;			// �õ���ַ��
	    t2=(RmtRec>>16)&0xff;	// �õ���ַ���� 
 	    if((t1==(u8)~t2)&&t1==REMOTE_ID)// ����ң��ʶ����(ID)����ַ 
	    { 
	        t1=RmtRec>>8;
	        t2=RmtRec; 	
	        if(t1==(u8)~t2)
			{	
				sta = t1;// ��ֵ��ȷ
				RmtSta = 0;
				RmtRec = 0;
			}
		}   
		if((sta==0)||((RmtSta&0X80)==0))// �������ݴ���/ң���Ѿ�û�а�����
		{
		 	RmtSta&=~(1<<6);// ������յ���Ч������ʶ
			RmtCnt=0;		// �����������������
		}
	}  
    return sta;
}


void TIM1_BRK_TIM9_IRQHandler(void) {
	
	if(TIM_GetITStatus(TIM9,TIM_IT_Update)!=RESET)
	{
		if(RmtSta&0x80)								// �ϴ������ݱ����յ���
		{	
			RmtSta&=~0X10;							// ȡ���������Ѿ���������
			if((RmtSta&0X0F)==0X00)RmtSta|=1<<6;	// ����Ѿ����һ�ΰ����ļ�ֵ��Ϣ�ɼ�
			if((RmtSta&0X0F)<14)RmtSta++;
			else
			{
				RmtSta&=~(1<<7);					// ���������ʶ
				RmtSta&=0XF0;						// ��ռ�����	
			}								 	   	
		}							    
	}
    // �������벶���ж�
    if (TIM_GetITStatus(TIM9, TIM_IT_CC2) != RESET)
	{
		if(RDATA)// �����ز���
		{
			TIM_OC2PolarityConfig(TIM9, TIM_ICPolarity_Falling); // �л�Ϊ�½��ز���
            TIM_SetCounter(TIM9, 0);     // ���ü�����
            RmtSta |= 0x10;              // ����������Ѳ���
		}
		
		else
		{
			Dval = TIM_GetCapture2(TIM9);   
			TIM_OC2PolarityConfig(TIM9, TIM_ICPolarity_Rising);
			if(RmtSta&0X10)
			{
				if (RmtSta & 0x80) 
				{  
					if (Dval > 300 && Dval < 800)
					{
						RmtRec <<= 1;
						RmtRec |= 0;
					} 
                // �����߼�1��1680us �� ��
					else if (Dval > 1400 && Dval < 1800) 
					{
						RmtRec <<= 1;
						RmtRec |= 1;
					} 
                // �ظ��루2500us �� ��
					else if (Dval > 2200 && Dval < 2600) 
					{
						RmtCnt++;
						RmtSta &= 0xF0;  // ��ռ�ʱ��
					}
				}
				else if(Dval>4200&&Dval<4700)		// 4500Ϊ��׼ֵ4.5ms
				{
					RmtSta|=1<<7;					// ��ǳɹ����յ���������
					RmtCnt=0;						// �����������������
				}	
			}
			RmtSta&=~(1<<4);
		
		}

			
	}
	
	TIM_ClearITPendingBit(TIM9, TIM_IT_Update | TIM_IT_CC2);  // ��������жϱ�־
          
}


