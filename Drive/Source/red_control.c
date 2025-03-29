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
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);  // GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9, ENABLE);    // TIM9时钟
	
	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_TIM9); 
	
	
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;        // 复用模式
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;      // 推挽输出
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;    // 无上下拉
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct = {0};
	TIM_TimeBaseStruct.TIM_Prescaler = 168-1;       // 分频系数（84MHz系统时钟→1MHz计数频率）
	TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStruct.TIM_Period = 10000-1;          // ARR最大值
	TIM_TimeBaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM9, &TIM_TimeBaseStruct);
	
	TIM_ICInitTypeDef TIM_ICInitStruct = {0};
	TIM_ICInitStruct.TIM_Channel = TIM_Channel_2;    // CH2对应PA3
	TIM_ICInitStruct.TIM_ICPolarity = TIM_ICPolarity_Rising;  // 上升沿触发
	TIM_ICInitStruct.TIM_ICSelection = TIM_ICSelection_DirectTI; // 直接映射到TI2
	TIM_ICInitStruct.TIM_ICPrescaler = TIM_ICPSC_DIV1;        // 不分频
	TIM_ICInitStruct.TIM_ICFilter = 0x0F;                      // 无滤波
	TIM_ICInit(TIM9, &TIM_ICInitStruct);
	
	
	NVIC_InitTypeDef NVIC_InitStruct = {0};
	NVIC_InitStruct.NVIC_IRQChannel = TIM1_BRK_TIM9_IRQn;  // TIM9中断通道
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	

	TIM_ITConfig(TIM9, TIM_IT_Update|TIM_IT_CC2, ENABLE);          // 允许CH2捕获中断
	TIM_ClearITPendingBit(TIM9,TIM_IT_Update | TIM_IT_CC2);
	TIM_Cmd(TIM9, ENABLE);
}

u16 Dval;		//下降沿时计数器的值
u8 	RmtSta=0;	  	  
u32 RmtRec=0;	//红外接收到的数据	   		    
u8  RmtCnt=0;	//按键按下的次数	
uint8_t a=0;
u8 Remote_Scan(void)
{        
	u8 sta=0;       
    u8 t1,t2;  
	if(RmtSta&(1<<6))//得到一个按键的所有信息了
	{ 
	    t1=RmtRec>>24;			// 得到地址码
	    t2=(RmtRec>>16)&0xff;	// 得到地址反码 
 	    if((t1==(u8)~t2)&&t1==REMOTE_ID)// 检验遥控识别码(ID)及地址 
	    { 
	        t1=RmtRec>>8;
	        t2=RmtRec; 	
	        if(t1==(u8)~t2)
			{	
				sta = t1;// 键值正确
				RmtSta = 0;
				RmtRec = 0;
			}
		}   
		if((sta==0)||((RmtSta&0X80)==0))// 按键数据错误/遥控已经没有按下了
		{
		 	RmtSta&=~(1<<6);// 清除接收到有效按键标识
			RmtCnt=0;		// 清除按键次数计数器
		}
	}  
    return sta;
}


void TIM1_BRK_TIM9_IRQHandler(void) {
	
	if(TIM_GetITStatus(TIM9,TIM_IT_Update)!=RESET)
	{
		if(RmtSta&0x80)								// 上次有数据被接收到了
		{	
			RmtSta&=~0X10;							// 取消上升沿已经被捕获标记
			if((RmtSta&0X0F)==0X00)RmtSta|=1<<6;	// 标记已经完成一次按键的键值信息采集
			if((RmtSta&0X0F)<14)RmtSta++;
			else
			{
				RmtSta&=~(1<<7);					// 清空引导标识
				RmtSta&=0XF0;						// 清空计数器	
			}								 	   	
		}							    
	}
    // 处理输入捕获中断
    if (TIM_GetITStatus(TIM9, TIM_IT_CC2) != RESET)
	{
		if(RDATA)// 上升沿捕获
		{
			TIM_OC2PolarityConfig(TIM9, TIM_ICPolarity_Falling); // 切换为下降沿捕获
            TIM_SetCounter(TIM9, 0);     // 重置计数器
            RmtSta |= 0x10;              // 标记上升沿已捕获
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
                // 解码逻辑1（1680us ± 误差）
					else if (Dval > 1400 && Dval < 1800) 
					{
						RmtRec <<= 1;
						RmtRec |= 1;
					} 
                // 重复码（2500us ± 误差）
					else if (Dval > 2200 && Dval < 2600) 
					{
						RmtCnt++;
						RmtSta &= 0xF0;  // 清空计时器
					}
				}
				else if(Dval>4200&&Dval<4700)		// 4500为标准值4.5ms
				{
					RmtSta|=1<<7;					// 标记成功接收到了引导码
					RmtCnt=0;						// 清除按键次数计数器
				}	
			}
			RmtSta&=~(1<<4);
		
		}

			
	}
	
	TIM_ClearITPendingBit(TIM9, TIM_IT_Update | TIM_IT_CC2);  // 清除捕获中断标志
          
}


