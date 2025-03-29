#include "sr04.h"
#include "pwm.h"
/*******************************
����˵����
PA2  -- TRIG(���)
PA3  -- ECHO(����)
*******************************/
void Sr04_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	TIM_TimeBaseInitTypeDef  	TIM_TimeBaseInitStruct;
	
	//ʹ��GPIOA��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	//1���ܶ�ʱ��ʱ�ӡ�
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
	
	GPIO_InitStruct.GPIO_Pin	= GPIO_Pin_2;			//����2
	GPIO_InitStruct.GPIO_Mode	= GPIO_Mode_OUT;		//�������
	GPIO_InitStruct.GPIO_OType	= GPIO_OType_PP;		//�������
	GPIO_InitStruct.GPIO_Speed	= GPIO_Speed_50MHz;		//�ٶ�
	GPIO_InitStruct.GPIO_PuPd	= GPIO_PuPd_UP;			//����
	GPIO_Init(GPIOC, &GPIO_InitStruct);	
	
	
	GPIO_InitStruct.GPIO_Pin	= GPIO_Pin_3;				//����3
	GPIO_InitStruct.GPIO_Mode	= GPIO_Mode_IN;				//����
	GPIO_InitStruct.GPIO_PuPd	= GPIO_PuPd_NOPULL;			//����
	GPIO_Init(GPIOC, &GPIO_InitStruct);	

	TIM_TimeBaseInitStruct.TIM_Period		= 50000-1; 				//���ؼĴ�����
	TIM_TimeBaseInitStruct.TIM_Prescaler	= 83;   				//84��Ƶ 84MHZ/84 = 1MHZ ��һ��������1us
	TIM_TimeBaseInitStruct.TIM_CounterMode	= TIM_CounterMode_Up;	//���ϼ���
	TIM_TimeBaseInitStruct.TIM_ClockDivision= TIM_CKD_DIV1;			//��Ƶ����
	//2����ʼ����ʱ��������ARR,PSC��
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseInitStruct);

	//5����ʹ�ܶ�ʱ����
	TIM_Cmd(TIM6, DISABLE);	

}

u16 Get_Sr04_Value(void)
{
	u16 t = 0;
	u16 distance;
	u16 count = 0;
	
	
	
	//�����ź�
	TRIG  = 0;
	Delay_us(5);
	TRIG  = 1;
	Delay_us(20);
	TRIG  = 0;

	//����CNTΪ0
	TIM6->CNT = 0;
	
	//�ȴ��ߵ�ƽ����,������ʱ����
//	while( PAin(3) == 0)
	while( PCin(3) == 0)
	{
		t++;
		Delay_us(10);
	
		//����ȴ�5msδ�ܵȴ��ߵ�ƽ���򷵻�
		if(t >= 500)
			return 0;
	}

	//������ʱ��
	TIM_Cmd(TIM6, ENABLE);	
	
	t = 0;
	//�ȴ��͵�ƽ������������ʱ����
//	while( PAin(3) == 1 )
		while( PCin(3) == 1 )
	{
		t++;
		Delay_us(20);
	
		//����ȴ�24ms��24ms�����ľ���Ϊ413cm�ˣ��ֲ���˵��ԶΪ40cmm����ȡ��ֵ��δ�ܵȴ��͵�ƽ���򷵻�
		if(t >= 1200)
			return 0;		
	}

	//��ȡ��ʱ����CNTֵ
	//count = TIM_GetCounter(TIM3);
	count = TIM6->CNT;
	
	//�رն�ʱ��
	TIM_Cmd(TIM6, DISABLE);	
	
	//ͨ����ʱ����CNTֵ���������

	distance = count/58;

	return distance;
}

//����ֵΪ0ʱΪֹͣ״̬
int control_if_move(void)
{
	distance = Get_Sr04_Value();
	if(distance<SAFE_DISTANCE){
		Motor_Stop();
		//���������ڷ���0X18
		
		return 0;
	}
	return 1;
}
