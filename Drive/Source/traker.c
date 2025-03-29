#include <stm32f4xx.h>
#include <stdio.h>
#include <stdarg.h>
#include <delay.h>
#include <string.h>
//#include "app_usart2.h"
//#include "bsp_usart.h"

#define Package_size 100
#define	UART4_TX_CLK		RCC_AHB1Periph_GPIOA
#define	UART4_RX_CLK		RCC_AHB1Periph_GPIOA 
#define  UART4_TX_PIN				GPIO_Pin_0					// TX ����
#define	UART4_TX_PORT				GPIOA							// TX ���Ŷ˿�
#define  UART4_RX_PIN				GPIO_Pin_1             // RX ����
#define	UART4_RX_PORT				GPIOA                   // RX ���Ŷ˿�
#define  UART4_RX_PinSource     GPIO_PinSource1 
#define  UART4_TX_PinSource     GPIO_PinSource0
u8 rx_buff[Package_size];
u8 new_package[Package_size];
u8 g_new_package_flag = 0;//���յ��µ�һ�����ݱ�־

u8 IR_Data_number[8];//����ֵ
u16 IR_Data_Anglo[8];//ģ��ֵ
u8 g_Amode_Data = 0;//ģ���ͱ�־
u8 g_Dmode_Data = 0;//�����ͱ�־

//USART2
void UART4_init(u32 baudrate)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd ( UART4_TX_CLK|UART4_RX_CLK, ENABLE); 	//IO��ʱ������

	//IO����
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;   	 //����ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;   //����
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;		 //����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; //�ٶȵȼ�

	//��ʼ�� TX	����
	GPIO_InitStructure.GPIO_Pin = UART4_TX_PIN;	 
	GPIO_Init(UART4_TX_PORT, &GPIO_InitStructure);	
	//��ʼ�� RX ����													   
	GPIO_InitStructure.GPIO_Pin = UART4_RX_PIN;	
	GPIO_Init(UART4_RX_PORT, &GPIO_InitStructure);		
	
	//IO���ã����õ�UART4
	GPIO_PinAFConfig(UART4_TX_PORT,UART4_TX_PinSource,GPIO_AF_UART4); 
	GPIO_PinAFConfig(UART4_RX_PORT,UART4_RX_PinSource,GPIO_AF_UART4);	
	
	USART_InitTypeDef USART_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
	
	// IO�ڳ�ʼ��
		 
	// ���ô��ڸ������
	USART_InitStructure.USART_BaudRate 	 = 115200; //������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; //����λ8λ
	USART_InitStructure.USART_StopBits   = USART_StopBits_1; //ֹͣλ1λ
	USART_InitStructure.USART_Parity     = USART_Parity_No ; //��У��
	USART_InitStructure.USART_Mode 	    = USART_Mode_Rx | USART_Mode_Tx; //���ͺͽ���ģʽ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // ��ʹ��Ӳ��������
	
	USART_Init(UART4,&USART_InitStructure); //��ʼ������1
	USART_Cmd(UART4,ENABLE);	//ʹ�ܴ���1
	
	USART_ITConfig(UART4,USART_IT_RXNE,ENABLE);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}

void Deal_IR_Usart(u8 rxtemp)
{
	static u8 g_start = 0;
  static u8 step = 0;
  if(rxtemp == '$')
  {  
    g_start = 1;//��ʼ���ձ�־
    rx_buff[step] = rxtemp;
    step++;
  }
  else
  {
    if(g_start == 0)
    {
      return;
    }
    else
    {
      rx_buff[step] = rxtemp;
      step ++;
      if(rxtemp == '#')//����
      {
        g_start = 0;
        step = 0;
        memcpy(new_package,rx_buff,Package_size);//��ֵ
        g_new_package_flag = 1;
        memset(rx_buff,0,Package_size);//�������

      }
      
      if(step >= Package_size)//�����쳣
      {
        g_start = 0;
        step = 0;
        memset(rx_buff,0,Package_size);//�������
      }
    }
  }
}

//����һ���ַ�
void UART4_Send_U8(uint8_t ch)
{
	while (USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET)
		;
	USART_SendData(UART4, ch);
}

//����һ���ַ���
/**
 * @Brief: UsART2��������
 * @Note: 
 * @Parm: BufferPtr:�����͵�����  Length:���ݳ���
 * @Retval: 
 */
void UART4_Send_ArrayU8(uint8_t *BufferPtr, uint16_t Length)
{
	while (Length--)
	{
		UART4_Send_U8(*BufferPtr);
		BufferPtr++;
	}
}

//�����жϷ�����
void UART4_IRQHandler(void)
{
	uint8_t Rx2_Temp;
	if (USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)
	{
		USART_ClearITPendingBit(UART4, USART_IT_RXNE);
		Rx2_Temp = USART_ReceiveData(UART4);
		Deal_IR_Usart(Rx2_Temp);//��������
		//USART2_Send_U8(Rx2_Temp); //ֻ�շ�����

	}
}



//************************�°벿��Ϊ�������ݵĲ������ϰ벿��Ϊusart�ĳ�ʼ��*******************


//Ѱ���ַ�������
int StringFind(const char *pSrc, const char *pDst)  
{  
    int i, j;  
    for (i=0; pSrc[i]!='\0'; i++)  
    {  
        if(pSrc[i]!=pDst[0])  
            continue;         
        j = 0;  
        while(pDst[j]!='\0' && pSrc[i+j]!='\0')  
        {  
            j++;  
            if(pDst[j]!=pSrc[i+j])  
            break;  
        }  
        if(pDst[j]=='\0')  
            return i;  
    }  
    return -1;  
}  




void send_control_data(u8 adjust,u8 aData,u8 dData)
{
	u8 send_buf[8] = "$0,0,0#";
	if(adjust == 1)//У׼����
	{
		send_buf[1] = '1';
	}
	else
	{
		send_buf[1] = '0';
	}
	if(aData == 1)//ģ��ֵ����
	{
		send_buf[3] = '1';
		g_Amode_Data = 1;
	}
	else
	{
		send_buf[3] = '0';
		g_Amode_Data = 0;
	}
	if(dData == 1)//����ֵ����
	{
		send_buf[5] = '1';
		g_Dmode_Data = 1;
	}
	else
	{
		send_buf[5] = '0';
		g_Dmode_Data = 0;
	}
	
	UART4_Send_ArrayU8(send_buf,strlen((char*)send_buf));
	
}

//�������ӣ�$D,x1:0,x2:0,x3:0,x4:0,x5:0,x6:0,x7:0,x8:0#
void Deal_Usart_Data(void) //��������������
{
	if(new_package[1]!='D')
	{
		return; //�˰����ݲ�������������
	}
	for(u8 i = 0;i<8;i++)
	{
		IR_Data_number[i] = (new_package[6+i*5]-'0');//���ַ�ת������ //6 11 16 21 26 
	}
	
	memset(new_package,0,Package_size);//���������
  
}




//---------------------------�ܷ�װʹ�ú���---------------------------

void TRAKER_Init(void)
{
	UART4_init(115200);
	Delay_ms(1000);//�ȴ���������
	
	/*while(1)
	{
		if(g_new_package_flag == 1)
		{
			g_new_package_flag = 0;
			
			if(g_Dmode_Data == 1)
			{
				Deal_Usart_Data();//��ֵ�����ݴ���
			}
			
		}
	}*/
}

void TRAKER_SERVICE(void)
{
	send_control_data(0,0,1); //����Э�鷢�Ͷ�Ӧ������� ��Ϊֻ������ֵ������
	if(g_new_package_flag == 1)
	{
		g_new_package_flag = 0;
		
		if(g_Dmode_Data == 1)
		{
			Deal_Usart_Data();//��ֵ�����ݴ���
		}
			
	}
}

//************************�켣Ӧ��ǰ������*************************



