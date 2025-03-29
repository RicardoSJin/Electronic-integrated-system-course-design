#include "sr04.h"
#include "pwm.h"
/*******************************
引脚说明：
PA2  -- TRIG(输出)
PA3  -- ECHO(输入)
*******************************/
void Sr04_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	TIM_TimeBaseInitTypeDef  	TIM_TimeBaseInitStruct;
	
	//使能GPIOA组时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	//1、能定时器时钟。
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
	
	GPIO_InitStruct.GPIO_Pin	= GPIO_Pin_2;			//引脚2
	GPIO_InitStruct.GPIO_Mode	= GPIO_Mode_OUT;		//输出类型
	GPIO_InitStruct.GPIO_OType	= GPIO_OType_PP;		//输出推挽
	GPIO_InitStruct.GPIO_Speed	= GPIO_Speed_50MHz;		//速度
	GPIO_InitStruct.GPIO_PuPd	= GPIO_PuPd_UP;			//上拉
	GPIO_Init(GPIOC, &GPIO_InitStruct);	
	
	
	GPIO_InitStruct.GPIO_Pin	= GPIO_Pin_3;				//引脚3
	GPIO_InitStruct.GPIO_Mode	= GPIO_Mode_IN;				//输入
	GPIO_InitStruct.GPIO_PuPd	= GPIO_PuPd_NOPULL;			//浮空
	GPIO_Init(GPIOC, &GPIO_InitStruct);	

	TIM_TimeBaseInitStruct.TIM_Period		= 50000-1; 				//重载寄存器。
	TIM_TimeBaseInitStruct.TIM_Prescaler	= 83;   				//84分频 84MHZ/84 = 1MHZ 计一个数，用1us
	TIM_TimeBaseInitStruct.TIM_CounterMode	= TIM_CounterMode_Up;	//向上计数
	TIM_TimeBaseInitStruct.TIM_ClockDivision= TIM_CKD_DIV1;			//分频因子
	//2、初始化定时器，配置ARR,PSC。
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseInitStruct);

	//5、不使能定时器。
	TIM_Cmd(TIM6, DISABLE);	

}

u16 Get_Sr04_Value(void)
{
	u16 t = 0;
	u16 distance;
	u16 count = 0;
	
	
	
	//触发信号
	TRIG  = 0;
	Delay_us(5);
	TRIG  = 1;
	Delay_us(20);
	TRIG  = 0;

	//设置CNT为0
	TIM6->CNT = 0;
	
	//等待高电平到来,并做超时处理
//	while( PAin(3) == 0)
	while( PCin(3) == 0)
	{
		t++;
		Delay_us(10);
	
		//如果等待5ms未能等待高电平，则返回
		if(t >= 500)
			return 0;
	}

	//开启定时器
	TIM_Cmd(TIM6, ENABLE);	
	
	t = 0;
	//等待低电平到来，并做超时处理
//	while( PAin(3) == 1 )
		while( PCin(3) == 1 )
	{
		t++;
		Delay_us(20);
	
		//如果等待24ms（24ms测量的距离为413cm了，手册中说最远为40cmm，故取此值）未能等待低电平，则返回
		if(t >= 1200)
			return 0;		
	}

	//获取定时器的CNT值
	//count = TIM_GetCounter(TIM3);
	count = TIM6->CNT;
	
	//关闭定时器
	TIM_Cmd(TIM6, DISABLE);	
	
	//通过定时器的CNT值计算出距离

	distance = count/58;

	return distance;
}

//返回值为0时为停止状态
int control_if_move(void)
{
	distance = Get_Sr04_Value();
	if(distance<SAFE_DISTANCE){
		Motor_Stop();
		//接下来串口发送0X18
		
		return 0;
	}
	return 1;
}
