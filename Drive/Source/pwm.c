#include "stm32f4xx.h"
#include "gpio.h"

void TIM134_PWM_Init(void)
{		 					 
	//此部分需手动修改IO口设置
	
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM13,ENABLE);  	//TIM14时钟使能
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14,ENABLE);  	//TIM14时钟使能    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE); 	//使能PORTF时钟	
	    
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource8,GPIO_AF_TIM13); //GPIOF9复用为定时器12
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource9,GPIO_AF_TIM14);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9;           //GPIOF8->TIM13 GPIOF9->TIM14  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //上拉
	GPIO_Init(GPIOF,&GPIO_InitStructure);              //初始化PF9
	TIM13->CNT =0;
	TIM14->CNT =0;
	  
	TIM_TimeBaseStructure.TIM_Prescaler=168-1;  //定时器分频
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseStructure.TIM_Period=1000-1;   //自动重装载值
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM13,&TIM_TimeBaseStructure);//初始化定时器14,13
	TIM_TimeBaseInit(TIM14,&TIM_TimeBaseStructure);
	//初始化TIM14 Channel1 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性低
	TIM_OC1Init(TIM13, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM12 4OC1
	TIM_OC1Init(TIM14, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM12 4OC2

	TIM_OC1PreloadConfig(TIM13, TIM_OCPreload_Enable);  //使能TIM14在CCR1上的预装载寄存器
	TIM_OC1PreloadConfig(TIM14, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM13,ENABLE);//ARPE使能 
	TIM_ARRPreloadConfig(TIM14,ENABLE);//ARPE使能 
	TIM_Cmd(TIM13, ENABLE);  //使能TIM13
	TIM_Cmd(TIM14, ENABLE);  //使能TIM14
 
										  
}  
/**
  * 函    数：PWM设置CCR
  * 参    数：Compare 要写入的CCR的值，范围：0~100
  * 返 回 值：无
  * 注意事项：CCR和ARR共同决定占空比，此函数仅设置CCR的值，并不直接是占空比
  *           占空比Duty = CCR / (ARR + 1)
  */
void PWM_SetCompare3(uint16_t Compare)
{
	TIM_SetCompare3(TIM13, Compare);		//设置CCR3的值
}

void Motor_R_F_SPEED(uint16_t Speed,uint8_t dir)
{
	TIM_SetCompare1(TIM13 , 10*Speed);
	Set_Dir_R(dir);
}

void Motor_L_F_SPEED(uint16_t Speed2,uint8_t dir2)
{
	TIM_SetCompare1(TIM14 , 10*Speed2);
	Set_Dir_L(dir2);
}

void Motor_Stop(void)
{
	Motor_R_F_SPEED(0,0);
	Motor_L_F_SPEED(0,0);
}