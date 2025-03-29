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
#define  UART4_TX_PIN				GPIO_Pin_0					// TX 引脚
#define	UART4_TX_PORT				GPIOA							// TX 引脚端口
#define  UART4_RX_PIN				GPIO_Pin_1             // RX 引脚
#define	UART4_RX_PORT				GPIOA                   // RX 引脚端口
#define  UART4_RX_PinSource     GPIO_PinSource1 
#define  UART4_TX_PinSource     GPIO_PinSource0
u8 rx_buff[Package_size];
u8 new_package[Package_size];
u8 g_new_package_flag = 0;//接收到新的一包数据标志

u8 IR_Data_number[8];//数字值
u16 IR_Data_Anglo[8];//模拟值
u8 g_Amode_Data = 0;//模拟型标志
u8 g_Dmode_Data = 0;//数字型标志

//USART2
void UART4_init(u32 baudrate)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd ( UART4_TX_CLK|UART4_RX_CLK, ENABLE); 	//IO口时钟配置

	//IO配置
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;   	 //复用模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;   //推挽
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;		 //上拉
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; //速度等级

	//初始化 TX	引脚
	GPIO_InitStructure.GPIO_Pin = UART4_TX_PIN;	 
	GPIO_Init(UART4_TX_PORT, &GPIO_InitStructure);	
	//初始化 RX 引脚													   
	GPIO_InitStructure.GPIO_Pin = UART4_RX_PIN;	
	GPIO_Init(UART4_RX_PORT, &GPIO_InitStructure);		
	
	//IO复用，复用到UART4
	GPIO_PinAFConfig(UART4_TX_PORT,UART4_TX_PinSource,GPIO_AF_UART4); 
	GPIO_PinAFConfig(UART4_RX_PORT,UART4_RX_PinSource,GPIO_AF_UART4);	
	
	USART_InitTypeDef USART_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
	
	// IO口初始化
		 
	// 配置串口各项参数
	USART_InitStructure.USART_BaudRate 	 = 115200; //波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; //数据位8位
	USART_InitStructure.USART_StopBits   = USART_StopBits_1; //停止位1位
	USART_InitStructure.USART_Parity     = USART_Parity_No ; //无校验
	USART_InitStructure.USART_Mode 	    = USART_Mode_Rx | USART_Mode_Tx; //发送和接收模式
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 不使用硬件流控制
	
	USART_Init(UART4,&USART_InitStructure); //初始化串口1
	USART_Cmd(UART4,ENABLE);	//使能串口1
	
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
    g_start = 1;//开始接收标志
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
      if(rxtemp == '#')//结束
      {
        g_start = 0;
        step = 0;
        memcpy(new_package,rx_buff,Package_size);//赋值
        g_new_package_flag = 1;
        memset(rx_buff,0,Package_size);//清空数据

      }
      
      if(step >= Package_size)//数据异常
      {
        g_start = 0;
        step = 0;
        memset(rx_buff,0,Package_size);//清空数据
      }
    }
  }
}

//发送一个字符
void UART4_Send_U8(uint8_t ch)
{
	while (USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET)
		;
	USART_SendData(UART4, ch);
}

//发送一个字符串
/**
 * @Brief: UsART2发送数据
 * @Note: 
 * @Parm: BufferPtr:待发送的数据  Length:数据长度
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

//串口中断服务函数
void UART4_IRQHandler(void)
{
	uint8_t Rx2_Temp;
	if (USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)
	{
		USART_ClearITPendingBit(UART4, USART_IT_RXNE);
		Rx2_Temp = USART_ReceiveData(UART4);
		Deal_IR_Usart(Rx2_Temp);//处理数据
		//USART2_Send_U8(Rx2_Temp); //只收发测试

	}
}



//************************下半部分为接收数据的操作，上半部分为usart的初始化*******************


//寻找字符型数据
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
	if(adjust == 1)//校准命令
	{
		send_buf[1] = '1';
	}
	else
	{
		send_buf[1] = '0';
	}
	if(aData == 1)//模拟值数据
	{
		send_buf[3] = '1';
		g_Amode_Data = 1;
	}
	else
	{
		send_buf[3] = '0';
		g_Amode_Data = 0;
	}
	if(dData == 1)//数字值数据
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

//数据例子：$D,x1:0,x2:0,x3:0,x4:0,x5:0,x6:0,x7:0,x8:0#
void Deal_Usart_Data(void) //处理数字型数据
{
	if(new_package[1]!='D')
	{
		return; //此包数据不是数字型数据
	}
	for(u8 i = 0;i<8;i++)
	{
		IR_Data_number[i] = (new_package[6+i*5]-'0');//把字符转成数字 //6 11 16 21 26 
	}
	
	memset(new_package,0,Package_size);//清除旧数据
  
}




//---------------------------总封装使用函数---------------------------

void TRAKER_Init(void)
{
	UART4_init(115200);
	Delay_ms(1000);//等待红外正常
	
	/*while(1)
	{
		if(g_new_package_flag == 1)
		{
			g_new_package_flag = 0;
			
			if(g_Dmode_Data == 1)
			{
				Deal_Usart_Data();//数值型数据处理
			}
			
		}
	}*/
}

void TRAKER_SERVICE(void)
{
	send_control_data(0,0,1); //根据协议发送对应的命令即可 此为只接收数值型数据
	if(g_new_package_flag == 1)
	{
		g_new_package_flag = 0;
		
		if(g_Dmode_Data == 1)
		{
			Deal_Usart_Data();//数值型数据处理
		}
			
	}
}

//************************轨迹应用前进函数*************************



