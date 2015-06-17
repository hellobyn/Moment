
/*********************************************************************************************************
** 
**                               			北京交通大学                                     
**
**--------------------------------------------------------------------------------------------------------
** 	文件名：		Interface.c
** 	创建时间：		2014-1-22 14:30
** 	创建人员： 		赵秉贤
** 	文件描述:  		交互模块控制源文件
** 
**--------------------------------------------------------------------------------------------------------
** 	最后修改时间：	2014-1-25 23:00 
** 	最后修改人员：	赵秉贤	
** 	版本号： 	   	V1.0
** 	版本描述： 		进行初步的电机控制实验
**
*********************************************************************************************************/

#include "Interface.h"

/*********************************************************************************************************
**	函数原型:	void LEDInit(void)
**	函数功能:	交互模块控制初始化
**	输入参数:	无
**	输出参数:	无
**	返回参数:	无
**	修改时间:	2014-1-22	14:30
*********************************************************************************************************/
void InterfaceInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4 , ENABLE);					/*	使能定时器4时钟			*/
																	
    GPIO_InitStructure.GPIO_Pin = red | green | blue | buzzer;				/*	LED、蜂鸣器端口配置		*/
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;                 		/*	复用推免输出			*/
    GPIO_Init(GPIOB, &GPIO_InitStructure);
																	  
	TIM_TimeBaseStructure.TIM_Period = 99;             						/*	PWM频率 = 5kHz			*/
    TIM_TimeBaseStructure.TIM_Prescaler = 71;            					/*	设置预分频值 = 72		*/
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;    						/*	时钟分割倍数为1倍		*/
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;				/*	定时器向上计数模式		*/
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	                        
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;						/*	定时器设置为PWM1模式	*/
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;			/*	使能输出比较状态		*/
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;				/*	定时器输出比较极性高	*/
	TIM_OCInitStructure.TIM_Pulse = 100;									/*	PWM初始占空比 = 0		*/
	
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);								/*	初始化定时器4的1通道	*/
	TIM_OC2Init(TIM4, &TIM_OCInitStructure);								/*	初始化定时器4的2通道	*/
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);								/*	初始化定时器4的3通道	*/
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);								/*	初始化定时器4的4通道	*/

	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);						/*	使能CCR1的预装载寄存器	*/
	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);						/*	使能CCR2的预装载寄存器	*/
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);						/*	使能CCR3的预装载寄存器	*/
	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);						/*	使能CCR4的预装载寄存器	*/

	TIM_ARRPreloadConfig(TIM4, ENABLE);										/*	使能ARR的预装载寄存器	*/
	TIM_Cmd(TIM4, ENABLE);													/*	使能定时器4				*/
}
