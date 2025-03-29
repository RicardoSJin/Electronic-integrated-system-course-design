#include "stm32f4xx.h"

void MOTOR_Init(void)
{
RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);	//����GPIOA��ʱ��
RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	
															//ʹ�ø�������ǰ���뿪��ʱ�ӣ����������Ĳ�����	
	/*GPIO��ʼ��*/
GPIO_InitTypeDef GPIO_InitStructure;					//����ṹ�����
	
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		//GPIOģʽ����ֵΪ�������ģʽ           						��				��                      
GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_8|GPIO_Pin_7;		//GPIO���ţ���ֵΪ��4.5������;4�ŷ���5������6�ŷ���7������
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//GPIO�ٶȣ���ֵΪ50MHz
GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;

GPIO_Init(GPIOA, &GPIO_InitStructure);					//����ֵ��Ĺ���������ݸ�GPIO_Init����
															//�����ڲ����Զ����ݽṹ��Ĳ���������Ӧ�Ĵ���
															//ʵ��GPIOA�ĳ�ʼ��
	
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		//GPIOģʽ����ֵΪ�������ģʽ           						��				��                      
GPIO_InitStructure.GPIO_Pin =GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_14;		//GPIO���ţ���ֵΪ��4.5������;4�ŷ���5������6�ŷ���7������
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//GPIO�ٶȣ���ֵΪ50MHz
GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;

GPIO_Init(GPIOF, &GPIO_InitStructure);					//����ֵ��Ĺ���������ݸ�GPIO_Init����
															//�����ڲ����Զ����ݽṹ��Ĳ���������Ӧ�Ĵ���
															//ʵ��GPIOA�ĳ�ʼ��
	
	

}

void Set_Dir_R(uint8_t dir_R)//0����ת��1������ת
{
	
	if(dir_R == 0 )//�Ҳ࣬��
	{
	GPIO_SetBits(GPIOA, GPIO_Pin_8);
	GPIO_ResetBits(GPIOA, GPIO_Pin_7);
		
	GPIO_SetBits(GPIOF,GPIO_Pin_5); //����
	GPIO_ResetBits(GPIOF,GPIO_Pin_6);
	}
	if(dir_R == 1 )//�Ҳ࣬��
	{
	GPIO_SetBits(GPIOA, GPIO_Pin_7);
	GPIO_ResetBits(GPIOA, GPIO_Pin_8);
		
	GPIO_SetBits(GPIOF, GPIO_Pin_6); //����
	GPIO_ResetBits(GPIOF, GPIO_Pin_5);
	}
}

void Set_Dir_L(uint8_t dir_L)
{
	if(dir_L == 0 )//��෴
	{
	GPIO_SetBits(GPIOA, GPIO_Pin_4);
	GPIO_ResetBits(GPIOA, GPIO_Pin_5);
		
	GPIO_SetBits(GPIOF,GPIO_Pin_14); //����
	GPIO_ResetBits(GPIOA,GPIO_Pin_2);
	}
	if(dir_L == 1 )
	{
	GPIO_SetBits(GPIOA, GPIO_Pin_5);
	GPIO_ResetBits(GPIOA, GPIO_Pin_4);
		
	GPIO_SetBits(GPIOA, GPIO_Pin_2); //����
	GPIO_ResetBits(GPIOF, GPIO_Pin_14);
	}
}

