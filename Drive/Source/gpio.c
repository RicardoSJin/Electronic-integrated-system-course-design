#include "stm32f4xx.h"

void MOTOR_Init(void)
{
RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);	//开启GPIOA的时钟
RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	
															//使用各个外设前必须开启时钟，否则对外设的操作无	
	/*GPIO初始化*/
GPIO_InitTypeDef GPIO_InitStructure;					//定义结构体变量
	
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		//GPIO模式，赋值为推挽输出模式           						左				右                      
GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_8|GPIO_Pin_7;		//GPIO引脚，赋值为第4.5号引脚;4脚反向，5脚正向，6脚反向，7脚正向
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//GPIO速度，赋值为50MHz
GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;

GPIO_Init(GPIOA, &GPIO_InitStructure);					//将赋值后的构体变量传递给GPIO_Init函数
															//函数内部会自动根据结构体的参数配置相应寄存器
															//实现GPIOA的初始化
	
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		//GPIO模式，赋值为推挽输出模式           						左				右                      
GPIO_InitStructure.GPIO_Pin =GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_14;		//GPIO引脚，赋值为第4.5号引脚;4脚反向，5脚正向，6脚反向，7脚正向
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//GPIO速度，赋值为50MHz
GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;

GPIO_Init(GPIOF, &GPIO_InitStructure);					//将赋值后的构体变量传递给GPIO_Init函数
															//函数内部会自动根据结构体的参数配置相应寄存器
															//实现GPIOA的初始化
	
	

}

void Set_Dir_R(uint8_t dir_R)//0代表反转，1代表正转
{
	
	if(dir_R == 0 )//右侧，反
	{
	GPIO_SetBits(GPIOA, GPIO_Pin_8);
	GPIO_ResetBits(GPIOA, GPIO_Pin_7);
		
	GPIO_SetBits(GPIOF,GPIO_Pin_5); //后轮
	GPIO_ResetBits(GPIOF,GPIO_Pin_6);
	}
	if(dir_R == 1 )//右侧，反
	{
	GPIO_SetBits(GPIOA, GPIO_Pin_7);
	GPIO_ResetBits(GPIOA, GPIO_Pin_8);
		
	GPIO_SetBits(GPIOF, GPIO_Pin_6); //后轮
	GPIO_ResetBits(GPIOF, GPIO_Pin_5);
	}
}

void Set_Dir_L(uint8_t dir_L)
{
	if(dir_L == 0 )//左侧反
	{
	GPIO_SetBits(GPIOA, GPIO_Pin_4);
	GPIO_ResetBits(GPIOA, GPIO_Pin_5);
		
	GPIO_SetBits(GPIOF,GPIO_Pin_14); //后轮
	GPIO_ResetBits(GPIOA,GPIO_Pin_2);
	}
	if(dir_L == 1 )
	{
	GPIO_SetBits(GPIOA, GPIO_Pin_5);
	GPIO_ResetBits(GPIOA, GPIO_Pin_4);
		
	GPIO_SetBits(GPIOA, GPIO_Pin_2); //后轮
	GPIO_ResetBits(GPIOF, GPIO_Pin_14);
	}
}

