#include <stm32f4xx.h>
#include <stdio.h>
#include <stdarg.h>
#include "sr04.h"


uint8_t Serial_RxData;		//���崮�ڽ��յ����ݱ���
uint8_t Serial_RxFlag;		//���崮�ڽ��յı�־λ����
uint8_t Serial_Stop;



void USART3_Init(u32 bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE); //ʹ��GPIOBʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);//ʹ��USART3ʱ��	
	
	//����3��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3); 
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3); 
	
  	//USART3�˿�����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11 ; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOB,&GPIO_InitStructure); 
  
	//Usart3 NVIC ����
  	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;//��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���

   	//USART3 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  	USART_Init(USART3, &USART_InitStructure); //��ʼ������3

  	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//�������ڽ����ж�	
	USART_Cmd(USART3, ENABLE);                    //ʹ�ܴ���3 	

}

/**
  * ��    �������ڷ���һ���ֽ�
  * ��    ����Byte Ҫ���͵�һ���ֽ�
  * �� �� ֵ����
  */
void Serial_SendByte(uint8_t Byte)
{
	USART_SendData(USART3, Byte);		//���ֽ�����д�����ݼĴ�����д���USART�Զ�����ʱ����
	while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);	//�ȴ��������
	/*�´�д�����ݼĴ������Զ����������ɱ�־λ���ʴ�ѭ�������������־λ*/
}


/**
  * ��    �������ڷ���һ������
  * ��    ����Array Ҫ����������׵�ַ
  * ��    ����Length Ҫ��������ĳ���
  * �� �� ֵ����
  */
void Serial_SendArray(uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	for (i = 0; i < Length; i ++)		//��������
	{
		Serial_SendByte(Array[i]);		//���ε���Serial_SendByte����ÿ���ֽ�����
	}
}

/**
  * ��    �������ڷ���һ���ַ���
  * ��    ����String Ҫ�����ַ������׵�ַ
  * �� �� ֵ����
  */
void Serial_SendString(char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i ++)//�����ַ����飨�ַ������������ַ���������־λ��ֹͣ
	{
		Serial_SendByte(String[i]);		//���ε���Serial_SendByte����ÿ���ֽ�����
	}
}

/**
  * ��    ������ȡ���ڽ��ձ�־λ
  * ��    ������
  * �� �� ֵ�����ڽ��ձ�־λ����Χ��0~1�����յ����ݺ󣬱�־λ��1����ȡ���־λ�Զ�����
  */
uint8_t Serial_GetRxFlag(void)
{
	if (Serial_RxFlag == 1)			//�����־λΪ1
	{
		Serial_RxFlag = 0;
		return 1;					//�򷵻�1�����Զ������־λ
	}
	return 0;						//�����־λΪ0���򷵻�0
}

/**
  * ��    ������ȡ���ڽ��յ�����
  * ��    ������
  * �� �� ֵ�����յ����ݣ���Χ��0~255
  */
uint8_t Serial_GetRxData(void)
{
	return Serial_RxData;			//���ؽ��յ����ݱ���
}

uint8_t Serial_GetStop(void)
{
	if(Serial_Stop ==1 )
	{
		Serial_Stop =0;
		return 1;
	}
	else 
	{
		return Serial_Stop;
	}
}


/**
  * ��    ����USART3�жϺ���
  * ��    ������
  * �� �� ֵ����
  * ע������˺���Ϊ�жϺ�����������ã��жϴ������Զ�ִ��
  *           ������ΪԤ����ָ�����ƣ����Դ������ļ�����
  *           ��ȷ����������ȷ���������κβ��죬�����жϺ��������ܽ���
  */



void USART3_IRQHandler(void)
{
	if (USART_GetITStatus(USART3, USART_IT_RXNE) == SET)		//�ж��Ƿ���USART1�Ľ����¼��������ж�
	{
		Serial_RxData = USART_ReceiveData(USART3);				//��ȡ���ݼĴ���������ڽ��յ����ݱ���
		Serial_SendByte(Serial_RxData);
		if(	(Serial_RxData == 20)||(Serial_RxData == 21)||(Serial_RxData == 99)==1)       //ͣ��
		{
			Serial_Stop=1;
			
		}

		else
		{
		
		}
			
		Serial_RxFlag = 1;								 		//�ý��ձ�־λ����Ϊ1
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);			//���USART1��RXNE��־λ
																//��ȡ���ݼĴ������Զ�����˱�־λ
																	//����Ѿ���ȡ�����ݼĴ�����Ҳ���Բ�ִ�д˴���
	}
}
