
/*********************************************************************************************************
** 
**                               			北京交通大学                                     
**
**--------------------------------------------------------------------------------------------------------
** 	文件名：		Usart.c
** 	创建时间：		2013-7-16 10:00
** 	创建人员： 		赵秉贤
** 	文件描述:  		串口通信源程序
** 
**--------------------------------------------------------------------------------------------------------
** 	最后修改时间：	2013-7-16 10:00 
** 	最后修改人员：	赵秉贤	
** 	版本号： 	   	V1.0
** 	版本描述： 		红外检测ADC配合串口通信
**
*********************************************************************************************************/

#include "Usart.h"

/*********************************************************************************************************
**	函数原型:	int SendChar (int ch)
**	函数功能:	串口发送函数
**	输入参数:	ch 发送数据
**	输出参数:	无
**	返回参数:	无
**	修改时间:	2013-7-8	9:45
**	作者:		赵秉贤
*********************************************************************************************************/
int SendChar (int ch) 
{
	while (!(USART1->SR & USART_FLAG_TXE));
	USART1->DR = (ch & 0x1FF);
	return (ch);
}

/*********************************************************************************************************
**	函数原型:	int GetKey (void)
**	函数功能:	串口接收函数
**	输入参数:	无
**	输出参数:	无
**	返回参数:	接收到的数据
**	修改时间:	2013-7-8	9:45
**	作者:		赵秉贤
*********************************************************************************************************/
int GetKey (void) 
{
	while (!(USART1->SR & USART_FLAG_RXNE));
	return ((int)(USART1->DR & 0x1FF));
}

/*********************************************************************************************************
**	函数原型:	void UsartInit (void)
**	函数功能:	串口初始化
**	输入参数:	无
**	输出参数:	无
**	返回参数:	无
**	修改时间:	2013-7-8	9:45
**	作者:		赵秉贤
*********************************************************************************************************/
void UsartInit (void)
{
	USART_InitTypeDef USART_InitStructure;																						
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 , ENABLE);					/*	开启RCC时钟				*/
																			
	USART_DeInit(USART1);													/*	缺省值初始化串口		*/
	GPIO_InitStructure.GPIO_Pin = Usart1Tx;                       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;     					/*	复用推挽输出			*/
	GPIO_Init(GPIOA, &GPIO_InitStructure);                	
	GPIO_InitStructure.GPIO_Pin = Usart1Rx;                    		
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 					/*	浮空输入				*/
    GPIO_Init(GPIOA, &GPIO_InitStructure);
		                  
	USART_InitStructure.USART_BaudRate = 57600;           					/*	串口波特率57600			*/
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; 			/*	数据字长8位				*/		
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                  /*	1位停止字				*/		
    USART_InitStructure.USART_Parity = USART_Parity_No;                    	/*	无奇偶校验				*/		
    USART_InitStructure.USART_HardwareFlowControl 
							= USART_HardwareFlowControl_None;				/*	无流控制				*/
    USART_InitStructure.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;			/*	使能串口发送、接收功能	*/		
	USART_Init(USART1, &USART_InitStructure);                               /*	按预设值初始化串口		*/  
   	USART_Cmd(USART1, ENABLE);  
	USART_ClearFlag(USART1,USART_FLAG_TC); 
}
