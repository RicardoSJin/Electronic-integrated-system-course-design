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
//����Ƶ�һ����10�䷿��
uint16_t pwm_value;
int16_t Enocder_SpeedR;
int16_t Encoder_SpeedL;
int Encoder_Left;
int Encoder_Right;
int i=0;

int Speed_Set;
float Position_Kp=0.01,Position_Ki,Position_Kd=0.01;
uint8_t RxData;			//�������ڽ��մ������ݵı��� USART3 -> RX
uint8_t point = 0;
int PS2_LX,PS2_LY,PS2_RX,PS2_RY,PS2_KEY; 
int32_t distance = 0;



int ENCODER_PID(int encoder , int velocity)//measure��ǰ�棬target�ں���
{
    int bias = encoder - velocity ; 
    static int last_bias_velocity;
    float pid = bias*Position_Kp + (bias - last_bias_velocity)*Position_Kd;
    last_bias_velocity = bias;
    return pid ; 
}

//***************************�켣ǰ������Ӧ��***************************
void TRAKER_GOING(void)
{
		if((IR_Data_number[3]&&IR_Data_number[4]&&IR_Data_number[5]&&IR_Data_number[2])==1)//ȫ��
		{
			Motor_R_F_SPEED(10+ENCODER_PID((Encoder_Left-Encoder_Right),0),0);
		    Motor_L_F_SPEED(10-ENCODER_PID((Encoder_Left-Encoder_Right),0),0);
			point++;
			Serial_SendByte(point);//�������ݸ�����2���ð���2��������
			Delay_ms(500);
		}
		if((IR_Data_number[4]&&IR_Data_number[3]) == 0)//������һ����
		{
			Motor_R_F_SPEED(20+ENCODER_PID((Encoder_Left-Encoder_Right),0),0);
		    Motor_L_F_SPEED(20-ENCODER_PID((Encoder_Left-Encoder_Right),0),0);
		}
		if((IR_Data_number[1]&&IR_Data_number[6]) == 0)//������һ����
		{
			if(IR_Data_number[1]==0)//����Ҫ��ת�������ٶ�Ҫ��
			{
			Motor_R_F_SPEED(20,0);
		    Motor_L_F_SPEED(10,0);	
			}
			if(IR_Data_number[6]==0)//�Ҳ��Ҫ��ת�������ٶ�Ҫ��
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
		 case 162://1�Ű��������������
				 
		 //***********************״��1���Զ�Ѳ�ߣ����һᲥ������Ƶ귿��****************************
			//Motor_R_F_SPEED(20,0); //����pwm������ PF8 ����  0->���Ʒ������� PA8(��)->HIGH PA7(��)->LOW
			//Motor_L_F_SPEED(20,0);
			TRAKER_SERVICE();
			TRAKER_GOING();//�����÷���while������棬��Ϊ��������ʱ������
			control_if_move();//ǰ�����ϰ����ͣ��
		//***************************************************************************************
            break;
         case 98:
	//***********************״��2���Զ�Ѳ�ߣ����һᲥ������Ƶ귿��,���һ���led��ʾ����·������****************************		 
			TRAKER_SERVICE();
			TRAKER_GOING();//�����÷���while������棬��Ϊ��������ʱ������
			control_if_move();//ǰ�����ϰ����ͣ��
			display_traffic_lights();
		 break;
		 case 0x18://��ֱǰ��
			 Motor_R_F_SPEED(20+ENCODER_PID(Encoder_Right,20),0);
		     Motor_L_F_SPEED(20-ENCODER_PID(Encoder_Left,20),0); 
			 break;
		 case 0x4a://����
			 Motor_R_F_SPEED(20+ENCODER_PID(Encoder_Right,20),1);
		     Motor_L_F_SPEED(20-ENCODER_PID(Encoder_Left,20),1); 
			 break;
		 case 0x10://��ת
			 Motor_R_F_SPEED(20,0);
		     Motor_L_F_SPEED(10,0); 
			 break;
		 case 0x5a://��ת
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
	Delay_Init();		//��ʱ������ʼ��
	LED_Init();			//LED��ʼ��
	Usart_Config ();	// USART��ʼ������
	Encoder_Init();
	TIM5_Init(10000,84000);
	Motor_Stop();
	//����������
	Sr04_Init();
	//PS2�ֱ�����
	PS2_Init();									    //=====ps2�����˿ڳ�ʼ��
	PS2_SetInit();		 					    //=====ps2���ó�ʼ��,���á����̵�ģʽ������ѡ���Ƿ�����޸�
	//LCD����
	SPI_LCD_Init();	// LCD��ʼ��
	LCD_SetDirection(Direction_V);		
	LCD_SetTextFont(&CH_Font24);			// ����2424��������,ASCII�����ӦΪ2412
	LCD_SetBackColor(LCD_WHITE);			// ���ñ�����ɫ
	LCD_SetColor(LIGHT_RED);					// ���û�����ɫ
	LCD_Clear();		// ����
	LCD_DisplayText(23, 10,"Smart Guide Car");
	LCD_SetColor(LCD_BLACK);					// ���û�����ɫ
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
		
		
		//***********************״��1���Զ�Ѳ�ߣ����һᲥ������Ƶ귿��****************************
		Motor_R_F_SPEED(20,0); //����pwm������ PF8 ����  0->���Ʒ������� PA8(��)->HIGH PA7(��)->LOW
        Motor_L_F_SPEED(20,0);
		TRAKER_SERVICE();
		TRAKER_GOING();//�����÷���while������棬��Ϊ��������ʱ������
		control_if_move();//ǰ�����ϰ����ͣ��
		//***************************************************************************************
		
		/*i++;
//	Motor_R_F_SPEED(ENCODER_PID(Encoder_Right,20),0); //����pwm������ PF8 ����  0->���Ʒ������� PA8(��)->HIGH PA7(��)->LOW
//	Motor_L_F_SPEED(ENCODER_PID(Encoder_Left,20),0); //����pwm������ FP9 ����  0->���Ʒ������� PA4(��)->HIGH PA5(��)->LOW
		Motor_R_F_SPEED(20+ENCODER_PID((Encoder_Left-Encoder_Right),0),0); //����pwm������ FP8 ����  0->���Ʒ������� PA8(��)->HIGH PA9(��)->LOW
		Motor_L_F_SPEED(20-ENCODER_PID((Encoder_Left-Encoder_Right),0),0); //����pwm������ FP9 ����  0->���Ʒ������� PA4
		//��ʾ����������
//		distance = Get_Sr04_Value();
		LCD_DisplayDecimals(130,35,distance,  6,3);		// ��ʾС��	
		//��ʾ�ֱ���λ��Ϣ��ҡ������
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
			Speed_Set =20;//�ٶȳ�ʼֵ
			while(PS2_DataKey()!=STOP&&(!Serial_GetStop()))
			{
				Motor_R_F_SPEED(Speed_Set+ENCODER_PID((Encoder_Left-Encoder_Right),0),0); //����pwm������ FP8 ����  0->���Ʒ������� PA8(��)->HIGH PA9(��)->LOW
				Motor_L_F_SPEED(Speed_Set-ENCODER_PID((Encoder_Left-Encoder_Right),0),0); //����pwm������ FP9 ����  0->���Ʒ������� PA4(��)->HIGH PA5(��)->LOW
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
	
	if(TIM_GetITStatus(TIM5,TIM_IT_Update)==SET) //����ж�
	{
	
	Encoder_Right=Encoder_Get_Right();
	Encoder_Left= Encoder_Get_Left();
	}	
	
	TIM_ClearITPendingBit(TIM5,TIM_IT_Update);  //����жϱ�־λ
}



/* ---------------------------����ʹ�����--------------------------------
	��ʱ����TIM13\TIM14\TIM4\TIM2\TIM3\TIM5\TIM6   ʹ�����ţ�F8\F9\A2\A3\A6\A7\E0~E7\C2\C3
	GPIO: 									ʹ�����ţ�A4\A5\A8\A9*/
