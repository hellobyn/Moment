
/*********************************************************************************************************
** 
**                               			������ͨ��ѧ                                     
**
**--------------------------------------------------------------------------------------------------------
** 	�ļ�����		Interface.c
** 	����ʱ�䣺		2014-1-22 14:30
** 	������Ա�� 		�Ա���
** 	�ļ�����:  		����ģ�����Դ�ļ�
** 
**--------------------------------------------------------------------------------------------------------
** 	����޸�ʱ�䣺	2014-1-25 23:00 
** 	����޸���Ա��	�Ա���	
** 	�汾�ţ� 	   	V1.0
** 	�汾������ 		���г����ĵ������ʵ��
**
*********************************************************************************************************/

#include "Interface.h"

/*********************************************************************************************************
**	����ԭ��:	void LEDInit(void)
**	��������:	����ģ����Ƴ�ʼ��
**	�������:	��
**	�������:	��
**	���ز���:	��
**	�޸�ʱ��:	2014-1-22	14:30
*********************************************************************************************************/
void InterfaceInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4 , ENABLE);					/*	ʹ�ܶ�ʱ��4ʱ��			*/
																	
    GPIO_InitStructure.GPIO_Pin = red | green | blue | buzzer;				/*	LED���������˿�����		*/
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;                 		/*	�����������			*/
    GPIO_Init(GPIOB, &GPIO_InitStructure);
																	  
	TIM_TimeBaseStructure.TIM_Period = 99;             						/*	PWMƵ�� = 5kHz			*/
    TIM_TimeBaseStructure.TIM_Prescaler = 71;            					/*	����Ԥ��Ƶֵ = 72		*/
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;    						/*	ʱ�ӷָ��Ϊ1��		*/
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;				/*	��ʱ�����ϼ���ģʽ		*/
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	                        
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;						/*	��ʱ������ΪPWM1ģʽ	*/
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;			/*	ʹ������Ƚ�״̬		*/
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;				/*	��ʱ������Ƚϼ��Ը�	*/
	TIM_OCInitStructure.TIM_Pulse = 100;									/*	PWM��ʼռ�ձ� = 0		*/
	
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);								/*	��ʼ����ʱ��4��1ͨ��	*/
	TIM_OC2Init(TIM4, &TIM_OCInitStructure);								/*	��ʼ����ʱ��4��2ͨ��	*/
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);								/*	��ʼ����ʱ��4��3ͨ��	*/
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);								/*	��ʼ����ʱ��4��4ͨ��	*/

	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);						/*	ʹ��CCR1��Ԥװ�ؼĴ���	*/
	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);						/*	ʹ��CCR2��Ԥװ�ؼĴ���	*/
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);						/*	ʹ��CCR3��Ԥװ�ؼĴ���	*/
	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);						/*	ʹ��CCR4��Ԥװ�ؼĴ���	*/

	TIM_ARRPreloadConfig(TIM4, ENABLE);										/*	ʹ��ARR��Ԥװ�ؼĴ���	*/
	TIM_Cmd(TIM4, ENABLE);													/*	ʹ�ܶ�ʱ��4				*/
}
