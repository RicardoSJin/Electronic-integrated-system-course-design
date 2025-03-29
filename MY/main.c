#include "stm32f4xx.h"
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
#include "red_control.h"
#define STOP 15
//假设酒店一共有10间房间
uint16_t pwm_value;
int16_t Enocder_SpeedR;
int16_t Encoder_SpeedL;
int Encoder_Left;
int Encoder_Right;
int i=0;

int Speed_Set;
float Position_Kp=0.01,Position_Ki,Position_Kd=0.01;
uint8_t RxData;			//定义用于接收串口数据的变量 USART3 -> RX
uint8_t point = 0;
int PS2_LX,PS2_LY,PS2_RX,PS2_RY,PS2_KEY; 
int32_t distance = 0;



int ENCODER_PID(int encoder , int velocity)//measure在前面，target在后面
{
    int bias = encoder - velocity ; 
    static int last_bias_velocity;
    float pid = bias*Position_Kp + (bias - last_bias_velocity)*Position_Kd;
    last_bias_velocity = bias;
    return pid ; 
}

//***************************轨迹前进函数应用***************************
void TRAKER_GOING(void)
{
		if((IR_Data_number[3]&&IR_Data_number[4]&&IR_Data_number[5]&&IR_Data_number[2])==1)//全白
		{
			Motor_R_F_SPEED(10+ENCODER_PID((Encoder_Left-Encoder_Right),0),0);
		    Motor_L_F_SPEED(10-ENCODER_PID((Encoder_Left-Encoder_Right),0),0);
			point++;
			Serial_SendByte(point);//发送数据给板子2，让板子2处理数据
			Delay_ms(500);
		}
		if((IR_Data_number[4]&&IR_Data_number[3]) == 0)//其中有一个黑
		{
			Motor_R_F_SPEED(20+ENCODER_PID((Encoder_Left-Encoder_Right),0),0);
		    Motor_L_F_SPEED(20-ENCODER_PID((Encoder_Left-Encoder_Right),0),0);
		}
		if((IR_Data_number[1]&&IR_Data_number[6]) == 0)//两侧有一个黑
		{
			if(IR_Data_number[1]==0)//左侧黑要左转，右轮速度要快
			{
			Motor_R_F_SPEED(20,0);
		    Motor_L_F_SPEED(10,0);	
			}
			if(IR_Data_number[6]==0)//右侧黑要右转，左轮速度要快
			{
			Motor_R_F_SPEED(10,0);
		    Motor_L_F_SPEED(20,0);	
			}
		}
		if(point == 10)
		{
			point=0;
		}
		
}
//***********************************************************************************
void Key_Scan(u8 key)
{	 
	switch(key)
	{		
		 case 162://1号按键负责基本功能
				 
		 //***********************状况1，自动巡线，并且会播报到达酒店房间****************************
			//Motor_R_F_SPEED(20,0); //右轮pwm波生成 PF8 棕线  0->控制方向引脚 PA8(白)->HIGH PA7(紫)->LOW
			//Motor_L_F_SPEED(20,0);
			TRAKER_SERVICE();
			TRAKER_GOING();//这个最好放在while的最后面，因为里面有延时函数。
			control_if_move();//前方有障碍物就停下
		//***************************************************************************************
            break;
         case 98:
	//***********************状况2，自动巡线，并且会播报到达酒店房间,并且会有led显示提醒路上行人****************************		 
			TRAKER_SERVICE();
			TRAKER_GOING();//这个最好放在while的最后面，因为里面有延时函数。
			control_if_move();//前方有障碍物就停下
			display_traffic_lights();
		 break;
		 case 0x18://笔直前行
			 Motor_R_F_SPEED(20+ENCODER_PID(Encoder_Right,20),0);
		     Motor_L_F_SPEED(20-ENCODER_PID(Encoder_Left,20),0); 
			 break;
		 case 0x4a://后退
			 Motor_R_F_SPEED(20+ENCODER_PID(Encoder_Right,20),1);
		     Motor_L_F_SPEED(20-ENCODER_PID(Encoder_Left,20),1); 
			 break;
		 case 0x10://左转
			 Motor_R_F_SPEED(20,0);
		     Motor_L_F_SPEED(10,0); 
			 break;
		 case 0x5a://右转
			 Motor_R_F_SPEED(10,0);
		     Motor_L_F_SPEED(20,0); 
			 break;		 
        default:
            break;
	}	
}

int main(void)
{
	Remote_Init();
	USART3_Init(115200);
	MOTOR_Init();
	TIM134_PWM_Init();
	Delay_Init();		//延时函数初始化
	LED_Init();			//LED初始化
	Usart_Config ();	// USART初始化函数
	Encoder_Init();
	TIM5_Init(10000,84000);
	Motor_Stop();
	//超声波配置
	Sr04_Init();
	//PS2手柄配置
	PS2_Init();									    //=====ps2驱动端口初始化
	PS2_SetInit();		 					    //=====ps2配置初始化,配置“红绿灯模式”，并选择是否可以修改
	//LCD配置
	SPI_LCD_Init();	// LCD初始化
	LCD_SetDirection(Direction_V);		
	LCD_SetTextFont(&CH_Font24);			// 设置2424中文字体,ASCII字体对应为2412
	LCD_SetBackColor(LCD_WHITE);			// 设置背景颜色
	LCD_SetColor(LIGHT_RED);					// 设置画笔颜色
	LCD_Clear();		// 清屏
	LCD_DisplayText(23, 10,"Smart Guide Car");
	LCD_SetColor(LCD_BLACK);					// 设置画笔颜色
	LCD_SetTextFont(&CH_Font12);		
	LCD_DisplayText(0,35,"distance:");
	LCD_DisplayText(0,50,"PS2KEY:");
	LCD_DisplayText(0,65,"LX:");
	LCD_DisplayText(100,65,"LY:");
	LCD_DisplayText(0,80,"RX:");
	LCD_DisplayText(100,80,"RY:");
	TRAKER_Init();
	
	while (1)
	{	
		Key_Scan(Remote_Scan());
		
		
		//***********************状况1，自动巡线，并且会播报到达酒店房间****************************
		Motor_R_F_SPEED(20,0); //右轮pwm波生成 PF8 棕线  0->控制方向引脚 PA8(白)->HIGH PA7(紫)->LOW
        Motor_L_F_SPEED(20,0);
		TRAKER_SERVICE();
		TRAKER_GOING();//这个最好放在while的最后面，因为里面有延时函数。
		control_if_move();//前方有障碍物就停下
		//***************************************************************************************
		
		/*i++;
//	Motor_R_F_SPEED(ENCODER_PID(Encoder_Right,20),0); //右轮pwm波生成 PF8 棕线  0->控制方向引脚 PA8(白)->HIGH PA7(紫)->LOW
//	Motor_L_F_SPEED(ENCODER_PID(Encoder_Left,20),0); //左轮pwm波生成 FP9 红线  0->控制方向引脚 PA4(白)->HIGH PA5(黑)->LOW
		Motor_R_F_SPEED(20+ENCODER_PID((Encoder_Left-Encoder_Right),0),0); //右轮pwm波生成 FP8 棕线  0->控制方向引脚 PA8(白)->HIGH PA9(紫)->LOW
		Motor_L_F_SPEED(20-ENCODER_PID((Encoder_Left-Encoder_Right),0),0); //左轮pwm波生成 FP9 红线  0->控制方向引脚 PA4
		//显示超声波距离
//		distance = Get_Sr04_Value();
		LCD_DisplayDecimals(130,35,distance,  6,3);		// 显示小数	
		//显示手柄键位信息和摇杆坐标
		PS2_LX=PS2_AnologData(PSS_LX);    
		PS2_LY=PS2_AnologData(PSS_LY);
		PS2_RX=PS2_AnologData(PSS_RX);
		PS2_RY=PS2_AnologData(PSS_RY);
		PS2_KEY=PS2_DataKey();	
		LCD_DisplayNumber(30,65,PS2_LX,3);
		LCD_DisplayNumber(130,65,PS2_LY,3);
		LCD_DisplayNumber(30,80,PS2_RX,3);
		LCD_DisplayNumber(130,80,PS2_RY,3);
		LCD_DisplayNumber(130,50,PS2_KEY,3);

		switch(PS2_KEY)
		{
			case 0: LCD_ClearRect( 100, 180, 80, 100); break;
			case 1: 
			display_attention(); 
			Speed_Set =20;//速度初始值
			while(PS2_DataKey()!=STOP&&(!Serial_GetStop()))
			{
				Motor_R_F_SPEED(Speed_Set+ENCODER_PID((Encoder_Left-Encoder_Right),0),0); //右轮pwm波生成 FP8 棕线  0->控制方向引脚 PA8(白)->HIGH PA9(紫)->LOW
				Motor_L_F_SPEED(Speed_Set-ENCODER_PID((Encoder_Left-Encoder_Right),0),0); //左轮pwm波生成 FP9 红线  0->控制方向引脚 PA4(白)->HIGH PA5(黑)->LOW
				control_if_move();
				Delay_ms(100);
			}
			Motor_Stop();
			break;
			case 2: display_no(); break;
			case 3: display_traffic_lights(); break;
			case 4: display_aotu(); break;
			case 14: display_cars(); LED1_Toggle;break;
			case STOP: display_no(); 
			Motor_Stop();
			break;
		};
		
		Delay_ms(100);
*/
	}	
}


void TIM5_IRQHandler(void)
{
	
	if(TIM_GetITStatus(TIM5,TIM_IT_Update)==SET) //溢出中断
	{
	
	Encoder_Right=Encoder_Get_Right();
	Encoder_Left= Encoder_Get_Left();
	}	
	
	TIM_ClearITPendingBit(TIM5,TIM_IT_Update);  //清除中断标志位
}



/* ---------------------------引脚使用情况--------------------------------
	定时器：TIM13\TIM14\TIM4\TIM2\TIM3\TIM5\TIM6   使用引脚：F8\F9\A2\A3\A6\A7\E0~E7\C2\C3
	GPIO: 									使用引脚：A4\A5\A8\A9*/
