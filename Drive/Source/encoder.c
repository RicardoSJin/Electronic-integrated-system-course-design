#include "stm32f4xx.h"
#include "encoder.h"

/**************************************************************************
�������ܣ���λʱ���ȡ����������
��ڲ�������ʱ��
����  ֵ���ٶ�ֵ
**************************************************************************/



int16_t Encoder_Count_Right;					//ȫ�ֱ��������ڼ�����ת������������ֵ
int16_t Encoder_Count_Left;


void Encoder_Init(void)
{
	
	GPIO_InitTypeDef  GPIO_InitStructure; //GPIO������
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOE, ENABLE);//ʹ��GPIOA,GPIOEʱ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5; //KEY0 KEY1 KEY2��Ӧ����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//��ͨ����ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	GPIO_Init(GPIOE, &GPIO_InitStructure);//��ʼ��GPIOE2,3,4,5
	
	
	NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//ʹ��SYSCFGʱ��
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource2);//PE2 ���ӵ��ж���2
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource3);//PE3 ���ӵ��ж�
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource4);//PE4 ���ӵ��ж���4
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource5);//PE5 ���ӵ��ж�
	

	
	EXTI_InitStructure.EXTI_Line = EXTI_Line2 | EXTI_Line3 |EXTI_Line4|EXTI_Line5 ;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//�ж��¼�
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //�½��ش���
   	EXTI_InitStructure.EXTI_LineCmd = ENABLE;//�ж���ʹ��
	EXTI_Init(&EXTI_InitStructure);//����
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;//�ⲿ�ж�2
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x03;//��ռ���ȼ�3
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//�����ȼ�2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
  NVIC_Init(&NVIC_InitStructure);//����
	
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;//�ⲿ�ж�3
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;//��ռ���ȼ�2
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//�����ȼ�2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
  NVIC_Init(&NVIC_InitStructure);//����
  
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;//�ⲿ�ж�2
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x03;//��ռ���ȼ�3
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//�����ȼ�2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
  NVIC_Init(&NVIC_InitStructure);//����
	
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;//�ⲿ�ж�3
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;//��ռ���ȼ�2
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//�����ȼ�2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
  NVIC_Init(&NVIC_InitStructure);//����
	
	
	
} 

int16_t Encoder_Get_Right(void)
{
	/*ʹ��Temp������Ϊ�м̣�Ŀ���Ƿ���Encoder_Count��������*/
	/*�����Ҳ����ֱ�ӷ���Encoder_Count
	  �������Ͳ��ǻ�ȡ����ֵ�Ĳ���������
	  Ҳ����ʵ�ֹ��ܣ�ֻ��˼·��һ��*/
	int16_t Temp;
	Temp = Encoder_Count_Right;
	Encoder_Count_Right = 0;
	return Temp;
}

int16_t Encoder_Get_Left(void)
{
	/*ʹ��Temp������Ϊ�м̣�Ŀ���Ƿ���Encoder_Count��������*/
	/*�����Ҳ����ֱ�ӷ���Encoder_Count
	  �������Ͳ��ǻ�ȡ����ֵ�Ĳ���������
	  Ҳ����ʵ�ֹ��ܣ�ֻ��˼·��һ��*/
	int16_t Temp;
	Temp = Encoder_Count_Left;
	Encoder_Count_Left = 0;
	return Temp;
}





void EXTI2_IRQHandler(void) //PE2�ⲿ�ж�
{
	if (EXTI_GetITStatus(EXTI_Line2) == SET)		//�ж��Ƿ����ⲿ�жϺ��ߴ������ж�
	{

		if (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_2) == 0)
		{
			if (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_3) == 0)		//PE2���½��ش����жϣ���ʱ�����һ��PE3�ĵ�ƽ��Ŀ�����ж���ת����
			{
				Encoder_Count_Right --;					//�˷�����Ϊ��ת�����������Լ�
			}
		}
		EXTI_ClearITPendingBit(EXTI_Line2);			//����ⲿ�ж�2���ߵ��жϱ�־λ
													//�жϱ�־λ�������
													//�����жϽ��������ϵش�����������������
	}
}

void EXTI3_IRQHandler(void) //PE3�ⲿ�ж�
{
	
	if (EXTI_GetITStatus(EXTI_Line3) == SET)		//�ж��Ƿ����ⲿ�ж�1���ߴ������ж�
	{
		/*��������������������󣬿��ٴ��ж����ŵ�ƽ���Ա��ⶶ��*/
		if (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_3) == 0)
		{
			if (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_2) == 0)		//PB1���½��ش����жϣ���ʱ�����һ��PB0�ĵ�ƽ��Ŀ�����ж���ת����
			{
				Encoder_Count_Right ++;					//�˷�����Ϊ��ת��������������
			}
		}
		EXTI_ClearITPendingBit(EXTI_Line3);			//����ⲿ�ж�1���ߵ��жϱ�־λ
													//�жϱ�־λ�������
													//�����жϽ��������ϵش�����������������
	}
}

void EXTI4_IRQHandler(void) //PE4�ⲿ�ж�
{
	
	if (EXTI_GetITStatus(EXTI_Line4) == SET)		//�ж��Ƿ����ⲿ�ж�4���ߴ������ж�
	{
		/*��������������������󣬿��ٴ��ж����ŵ�ƽ���Ա��ⶶ��*/
		if (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4) == 0)
		{
			if (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_5) == 0)		//PB1���½��ش����жϣ���ʱ�����һ��PB0�ĵ�ƽ��Ŀ�����ж���ת����
			{
				Encoder_Count_Left --;					//�˷�����Ϊ��ת��������������
			}
		}
		EXTI_ClearITPendingBit(EXTI_Line4);			//����ⲿ�ж�1���ߵ��жϱ�־λ
													//�жϱ�־λ�������
													//�����жϽ��������ϵش�����������������
	}
}

void EXTI9_5_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line5) == SET)		//�ж��Ƿ����ⲿ�ж�4���ߴ������ж�
	{
		/*��������������������󣬿��ٴ��ж����ŵ�ƽ���Ա��ⶶ��*/
		if (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_5) == 0)
		{
			if (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4) == 0)		//PB1���½��ش����жϣ���ʱ�����һ��PB0�ĵ�ƽ��Ŀ�����ж���ת����
			{
				Encoder_Count_Left ++;					//�˷�����Ϊ��ת��������������
			}
		}
		EXTI_ClearITPendingBit(EXTI_Line5);			//����ⲿ�ж�1���ߵ��жϱ�־λ
													//�жϱ�־λ�������
													//�����жϽ��������ϵش�����������������
	}
}
