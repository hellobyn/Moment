
/*********************************************************************************************************
** 
**                               			������ͨ��ѧ                                     
**
**--------------------------------------------------------------------------------------------------------
** 	�ļ�����		Usart.c
** 	����ʱ�䣺		2013-7-16 10:00
** 	������Ա�� 		�Ա���
** 	�ļ�����:  		����ͨ��Դ����
** 
**--------------------------------------------------------------------------------------------------------
** 	����޸�ʱ�䣺	2013-7-16 10:00 
** 	����޸���Ա��	�Ա���	
** 	�汾�ţ� 	   	V1.0
** 	�汾������ 		������ADC��ϴ���ͨ��
**
*********************************************************************************************************/

#include "Usart.h"

/*********************************************************************************************************
**	����ԭ��:	int SendChar (int ch)
**	��������:	���ڷ��ͺ���
**	�������:	ch ��������
**	�������:	��
**	���ز���:	��
**	�޸�ʱ��:	2013-7-8	9:45
**	����:		�Ա���
*********************************************************************************************************/
int SendChar (int ch) 
{
	while (!(USART1->SR & USART_FLAG_TXE));
	USART1->DR = (ch & 0x1FF);
	return (ch);
}

/*********************************************************************************************************
**	����ԭ��:	int GetKey (void)
**	��������:	���ڽ��պ���
**	�������:	��
**	�������:	��
**	���ز���:	���յ�������
**	�޸�ʱ��:	2013-7-8	9:45
**	����:		�Ա���
*********************************************************************************************************/
int GetKey (void) 
{
	while (!(USART1->SR & USART_FLAG_RXNE));
	return ((int)(USART1->DR & 0x1FF));
}

/*********************************************************************************************************
**	����ԭ��:	void UsartInit (void)
**	��������:	���ڳ�ʼ��
**	�������:	��
**	�������:	��
**	���ز���:	��
**	�޸�ʱ��:	2013-7-8	9:45
**	����:		�Ա���
*********************************************************************************************************/
void UsartInit (void)
{
	USART_InitTypeDef USART_InitStructure;																						
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 , ENABLE);					/*	����RCCʱ��				*/
																			
	USART_DeInit(USART1);													/*	ȱʡֵ��ʼ������		*/
	GPIO_InitStructure.GPIO_Pin = Usart1Tx;                       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;     					/*	�����������			*/
	GPIO_Init(GPIOA, &GPIO_InitStructure);                	
	GPIO_InitStructure.GPIO_Pin = Usart1Rx;                    		
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 					/*	��������				*/
    GPIO_Init(GPIOA, &GPIO_InitStructure);
		                  
	USART_InitStructure.USART_BaudRate = 57600;           					/*	���ڲ�����57600			*/
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; 			/*	�����ֳ�8λ				*/		
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                  /*	1λֹͣ��				*/		
    USART_InitStructure.USART_Parity = USART_Parity_No;                    	/*	����żУ��				*/		
    USART_InitStructure.USART_HardwareFlowControl 
							= USART_HardwareFlowControl_None;				/*	��������				*/
    USART_InitStructure.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;			/*	ʹ�ܴ��ڷ��͡����չ���	*/		
	USART_Init(USART1, &USART_InitStructure);                               /*	��Ԥ��ֵ��ʼ������		*/  
   	USART_Cmd(USART1, ENABLE);  
	USART_ClearFlag(USART1,USART_FLAG_TC); 
}
