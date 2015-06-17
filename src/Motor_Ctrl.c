
/*********************************************************************************************************
** 
**                               			北京交通大学                                     
**
**--------------------------------------------------------------------------------------------------------
** 	文件名：		Motor_Ctrl.c
** 	创建时间：		2013-7-9 17：20
** 	创建人员： 		赵秉贤
** 	文件描述:  		电机控制源文件
** 
**--------------------------------------------------------------------------------------------------------
** 	最后修改时间：	2014-2-18 18:30 
** 	最后修改人员：	赵秉贤	
** 	版本号： 	   	V1.0
** 	版本描述： 		进行初步的电机控制实验
**
*********************************************************************************************************/

#include "Motor.h"
#include "Maze.h"
#include "Delay.h"

/*********************************************************************************************************
**	全局变量定义
*********************************************************************************************************/
MOTOR leftMotor;															/*	初始化左电机状态		*/
MOTOR rightMotor;															/*	初始化右电机状态		*/

/*********************************************************************************************************
**	函数原型:	void leftMotor_Constructor(void)
**	函数功能:	左电机初始化配置
**	输入参数:	无
**	输出参数:	无
**	返回参数:	无
**	修改时间:	2013-7-12	15:00
*********************************************************************************************************/
void leftMotor_Constructor(void)
{
	u8 i;
	for (i=0;i<100;i++)
	{
		leftMotor.AccelValue[i] = 0;
		leftMotor.AccelTimeLeft[i] = 0;
	}
	leftMotor.AccelTimeLeft[0] = 65535;
	leftMotor.targetD = 0;
	leftMotor.targetV = 0;	
	leftMotor.acturalD = 0;
	for (i=0;i<3;i++)
		leftMotor.disError[i] = 0;
	leftMotor.valueP = 0;
	leftMotor.valueI = 0;
	leftMotor.valueD = 0;
	leftMotor.deltaPID = 0;
	leftMotor.motorPID = 0;
	leftMotor.PWM = 0;
}

/*********************************************************************************************************
**	函数原型:	void rightMotor_Constructor(void)
**	函数功能:	右电机初始化配置
**	输入参数:	无
**	输出参数:	无
**	返回参数:	无
**	修改时间:	2013-7-12	15:00
*********************************************************************************************************/
void rightMotor_Constructor(void)
{
	u8 i;
	for (i=0;i<100;i++)
	{
		rightMotor.AccelValue[i] = 0;
		rightMotor.AccelTimeLeft[i] = 0;
	}
	rightMotor.AccelTimeLeft[0] = 65535;
	rightMotor.targetD = 0;
	rightMotor.targetV = 0;	
	rightMotor.acturalD = 0;
	for (i=0;i<3;i++)
		rightMotor.disError[i] = 0;
	rightMotor.valueP = 0;
	rightMotor.valueI = 0;
	rightMotor.valueD = 0;
	rightMotor.deltaPID = 0;
	rightMotor.motorPID = 0;
	rightMotor.PWM = 0;	
}

/*********************************************************************************************************
**	函数原型:	void leftMotor_ClearCache(void)
**	函数功能:	左电机运行参数清除
**	输入参数:	无
**	输出参数:	无
**	返回参数:	无
**	修改时间:	2013-7-12	15:00
*********************************************************************************************************/
void leftMotor_ClearCache(void)
{
	u8 i;
	leftMotor.targetD = 0;
	leftMotor.targetV = 0;	
	leftMotor.acturalD = 0;
	for (i=0;i<3;i++)
		leftMotor.disError[i] = 0;
	leftMotor.valueP = 0;
	leftMotor.valueI = 0;
	leftMotor.valueD = 0;
	leftMotor.deltaPID = 0;
	leftMotor.motorPID = 0;
	leftMotor.PWM = 0;
}

/*********************************************************************************************************
**	函数原型:	void rightMotor_Constructor(void)
**	函数功能:	右电机运行参数清除
**	输入参数:	无
**	输出参数:	无
**	返回参数:	无
**	修改时间:	2013-7-12	15:00
*********************************************************************************************************/
void rightMotor_ClearCache(void)
{
	u8 i;
	rightMotor.targetD = 0;
	rightMotor.targetV = 0;	
	rightMotor.acturalD = 0;
	for (i=0;i<3;i++)
		rightMotor.disError[i] = 0;
	rightMotor.valueP = 0;
	rightMotor.valueI = 0;
	rightMotor.valueD = 0;
	rightMotor.deltaPID = 0;
	rightMotor.motorPID = 0;
	rightMotor.PWM = 0;	
}

/*********************************************************************************************************
**	函数原型:	void MotorInit(void)
**	函数功能:	电机驱动初始化函数
**	输入参数:	无
**	输出参数:	无
**	返回参数:	无
**	修改时间:	2013-7-12	15:00
*********************************************************************************************************/
void MotorInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8 , ENABLE);					/*	使能定时器8时钟			*/
																	
    GPIO_InitStructure.GPIO_Pin = LeftMotorPWM | RightMotorPWM;				/*	左右电机调速端口配置	*/
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;                 		/*	复用推免输出			*/
    GPIO_Init(GPIOC, &GPIO_InitStructure);
																	  
    GPIO_InitStructure.GPIO_Pin = LeftMotorDIR | RightMotorDIR;				/*	左右电机方向端口配置	*/
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;				 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;                 		/*	开漏输出				*/
    GPIO_Init(GPIOC,&GPIO_InitStructure);

	TIM_TimeBaseStructure.TIM_Period = 2879;             					/*	PWM频率 = 25kHz			*/
    TIM_TimeBaseStructure.TIM_Prescaler = 0;            					/*	设置预分频值 = 1		*/
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;    						/*	时钟分割倍数为1倍		*/
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;				/*	定时器向上计数模式		*/
	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);
	                        
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;						/*	定时器设置为PWM1模式	*/
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;			/*	使能输出比较状态		*/
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;				/*	定时器输出比较极性高	*/
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;				/*	MOE=0 设置输出比较空闲	*/
	TIM_OCInitStructure.TIM_Pulse = 0;										/*	PWM初始占空比 = 0		*/
	TIM_OC2Init(TIM8, &TIM_OCInitStructure);								/*	初始化定时器8的2通道	*/
	TIM_OC3Init(TIM8, &TIM_OCInitStructure);								/*	初始化定时器8的3通道	*/

	TIM_OC2PreloadConfig(TIM8, TIM_OCPreload_Enable);						/*	使能CCR2的预装载寄存器	*/
	TIM_OC3PreloadConfig(TIM8, TIM_OCPreload_Enable);						/*	使能CCR3的预装载寄存器	*/

	TIM_ARRPreloadConfig(TIM8, ENABLE);										/*	使能ARR的预装载寄存器	*/
	TIM_Cmd(TIM8, ENABLE);													/*	使能定时器8				*/
	TIM_CtrlPWMOutputs(TIM8, ENABLE);  										/*	使能定时器8的PWM输出功能*/
	leftMotorGoAhead;														/*	方向控制端置低			*/
	rightMotorGoAhead;														/*	方向控制端置低			*/
	leftMotor_Constructor();
	rightMotor_Constructor();
//	MotorENCInit();															/*	编码器检测初始化		*/
}

/*********************************************************************************************************
**	函数原型:	void MotorENCInit(void)
**	函数功能:	电机编码器初始化函数
**	输入参数:	无
**	输出参数:	无
**	返回参数:	无
**	修改时间:	2014-2-13 11:20 
*********************************************************************************************************/
void MotorENCInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);					/*	使能定时器3时钟			*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);					/*	使能定时器5时钟			*/
	
	GPIO_StructInit(&GPIO_InitStructure);   
	GPIO_InitStructure.GPIO_Pin = LeftENCA | LeftENCB | 					/*	左电机编码器端口配置	*/
								 RightENCA | RightENCB;						/*	右电机编码器端口配置	*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 					/*	浮空输入				*/
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	TIM_TimeBaseStructure.TIM_Period = 32000;             					/*	计数重载值，溢出中断	*/
    TIM_TimeBaseStructure.TIM_Prescaler = 0;            					/*	设置预分频值 = 1		*/
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;    				/*	时钟分割倍数为1倍		*/
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;				/*	定时器向上计数模式		*/
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
	
	TIM_DeInit(TIM3);
	TIM_DeInit(TIM5);
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);

	TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12,TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	TIM_EncoderInterfaceConfig(TIM5, TIM_EncoderMode_TI12,TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	TIM_ICStructInit(&TIM_ICInitStructure);									/*	设置定时器为编码器模式	*/
	
	TIM_ICInitStructure.TIM_ICFilter = 0x0; 								/*	比较滤波器参数设为0		*/
	TIM_ICInit(TIM3, &TIM_ICInitStructure);
	TIM_ICInit(TIM5, &TIM_ICInitStructure);

	TIM_ClearFlag(TIM3, TIM_FLAG_Update);									/*	清除TIM3中断标志位		*/
	TIM_ClearFlag(TIM5, TIM_FLAG_Update);									/*	清除TIM5中断标志位		*/
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);								/*	使能TIM3中断源			*/
	TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);								/*	使能TIM5中断源			*/
	
	TIM3->CNT = 32000;														/*	TIM3初始计数值清零		*/
	TIM5->CNT = 32000;														/*	TIM5初始计数值清零		*/
	TIM_Cmd(TIM3, ENABLE);													/*	使能外设 定时器3		*/
	TIM_Cmd(TIM5, ENABLE);													/*	使能外设 定时器5		*/
}

/*********************************************************************************************************
**	函数原型:	void LeftMotorDrv(s16 leftPWM)
**	函数功能:	左电机驱动函数
**	输入参数:	正转：电机PWM输出占空比 （0-1000）
				反转：电机PWM输出占空比 （2879-1879）		
**	输出参数:	无
**	返回参数:	无
**	修改时间:	2014-2-18 14:30 
*********************************************************************************************************/
void LeftMotorDrv(s16 leftPWM)
{
	leftMotor.PWM = leftPWM;
	if(leftPWM >= 0)														/*	如果目标速度为正数		*/
	{
		if(leftPWM >= 1500) leftPWM = 1500;									/*	设置PWM输出上限为1200	*/
		leftMotorGoAhead;													/*	选择电机转动方向		*/
	}
	else																	/*	如果目标速度为负数		*/
	{	
		if(leftPWM <= -1500) leftPWM = -1500;								/*	设置PWM输出下限为-1200	*/
		leftPWM = 2879 + leftPWM;											/*	则输出PWM占空比取补		*/
		leftMotorGoBack;													/*	选择电机转动方向		*/
	}
	TIM_SetCompare3(TIM8,leftPWM);											/*	设定PWM输出占空比		*/
}

/*********************************************************************************************************
**	函数原型:	void leftmotorPID(void)
**	函数功能:	左电机控制PID函数
**	输入参数:	无
**	输出参数:	无
**	返回参数:	无
**	修改时间:	2014-2-19 15:30 
*********************************************************************************************************/
void LeftmotorPID(void)
{
	leftMotor.disError[2] = leftMotor.disError[1];							/*	e(k-2)赋值				*/
	leftMotor.disError[1] = leftMotor.disError[0];							/*	e(k-1)赋值				*/
	leftMotor.disError[0] += leftMotor.targetD - leftMotor.acturalD;		/*	e(k)赋值				*/
	
	
	leftMotor.valueP = KpENC *( leftMotor.disError[0] - leftMotor.disError[1] );
																			/*	比例算子部分计算		*/
	leftMotor.valueI = KiENC *  leftMotor.disError[0];						/*	积分算子部分计算		*/
	leftMotor.valueD = KdENC *( leftMotor.disError[0] - 2 * leftMotor.disError[1] + leftMotor.disError[2] );
																			/*	微分算子部分计算		*/
	leftMotor.deltaPID = ( leftMotor.valueP + leftMotor.valueI + leftMotor.valueD ) / 1000;	
																			/*	PID增量值计算			*/
	leftMotor.motorPID += leftMotor.deltaPID;								/*	PID输出结果				*/
	LeftMotorDrv( leftMotor.motorPID );										/*	输出结果至电机驱动函数	*/
	
}

/*********************************************************************************************************
**	函数原型:	void leftMotor_TargetDCalculation(void)
**	函数功能:	左电机控制函数
**	输入参数:	无
**	输出参数:	无
**	返回参数:	无
**	修改时间:	2014-2-20 10:30 
*********************************************************************************************************/
void leftMotor_TargetDCalculation(void)
{	
	leftMotor.acturalD = TIM_GetCounter(TIM5) - 32000;
	TIM5->CNT = 32000;														/*	右电机编码器计数值复位	*/
	leftMotor.acturalD = leftMotor.acturalD * ENCToDis;						/*	计算左轮动作位移	um	*/
	
	mouse.vertD += leftMotor.acturalD / 2;									/*	电机剩余位移计算	um	*/
	leftMotor.targetD = (leftMotor.targetV + leftMotor.AccelValue[leftMotor.AccelPointer] * 0.25) * 0.0005;
																			/*	电机目标位移计算		*/
	leftMotor.targetV += leftMotor.AccelValue[leftMotor.AccelPointer] * 0.5;
																			/*	电机目标速度计算		*/
	if (leftMotor.targetV >= 1000000) leftMotor.targetV = 1000000;	
	leftMotor.AccelTimeLeft[leftMotor.AccelPointer] -= 1;
	if (leftMotor.AccelTimeLeft[leftMotor.AccelPointer] == 0)
		leftMotor.AccelPointer += 1;
}	

/*********************************************************************************************************
**	函数原型:	void RightMotorDrv(s16 rightPWM)
**	函数功能:	右电机驱动函数
**	输入参数:	正转：电机PWM输出占空比 （0-1000）
				反转：电机PWM输出占空比 （2879-1879）
**	输出参数:	无
**	返回参数:	无
**	修改时间:	2014-2-18 14:30 
*********************************************************************************************************/
void RightMotorDrv(s16 rightPWM)
{
	rightMotor.PWM = rightPWM;
	if(rightPWM >= 0)														/*	如果目标速度为正数		*/
	{	
		if(rightPWM >= 1500) rightPWM = 1500;								/*	设置PWM输出上限为1000	*/
		rightMotorGoAhead;													/*	选择电机转动方向		*/
	}
	else
	{
		if(rightPWM <= -1500) rightPWM = -1500;								/*	设置PWM输出上限为1000	*/
		rightPWM = 2879 + rightPWM;											/*	反转PWM占空比取反		*/
		rightMotorGoBack;													/*	选择电机转动方向		*/
	}
	TIM_SetCompare2(TIM8,rightPWM);											/*	设定PWM输出占空比		*/
}

/*********************************************************************************************************
**	函数原型:	void rightmotorPID(void)
**	函数功能:	右电机控制PID函数
**	输入参数:	无
**	输出参数:	无
**	返回参数:	无
**	修改时间:	2014-2-20 10:30 
*********************************************************************************************************/
void RightmotorPID(void)
{
	rightMotor.disError[2] = rightMotor.disError[1];						/*	e(k-2)赋值				*/
	rightMotor.disError[1] = rightMotor.disError[0];						/*	e(k-1)赋值				*/
	rightMotor.disError[0] += rightMotor.targetD - rightMotor.acturalD;		/*	e(k)赋值				*/
	
	
	rightMotor.valueP = KpENC *( rightMotor.disError[0] - rightMotor.disError[1] );
																			/*	比例算子部分计算		*/
	rightMotor.valueI = KiENC *  rightMotor.disError[0];					/*	积分算子部分计算		*/
	rightMotor.valueD = KdENC *( rightMotor.disError[0] - 2 * rightMotor.disError[1] + rightMotor.disError[2] );
																			/*	微分算子部分计算		*/
	rightMotor.deltaPID = ( rightMotor.valueP + rightMotor.valueI + rightMotor.valueD ) / 1000;	
																			/*	PID增量值计算			*/
	rightMotor.motorPID = rightMotor.motorPID + rightMotor.deltaPID;		/*	PID输出结果				*/
	RightMotorDrv( rightMotor.motorPID );									/*	输出结果至电机驱动函数	*/
}

/*********************************************************************************************************
**	函数原型:	void rightMotor_TargetDCalculation(void)
**	函数功能:	右电机控制函数
**	输入参数:	无
**	输出参数:	无
**	返回参数:	无
**	修改时间:	2014-2-20 10:30 
*********************************************************************************************************/
void rightMotor_TargetDCalculation(void)
{ 	
	rightMotor.acturalD = 32000 - TIM_GetCounter(TIM3);
	TIM3->CNT = 32000;														/*	左电机编码器计数值复位	*/
	rightMotor.acturalD = rightMotor.acturalD * ENCToDis;					/*	计算左轮动作位移 um		*/
	mouse.vertD += rightMotor.acturalD / 2;
	rightMotor.targetD = (rightMotor.targetV + rightMotor.AccelValue[rightMotor.AccelPointer] * 0.25) * 0.0005;
																			/*	电机目标位移计算		*/
	rightMotor.targetV += rightMotor.AccelValue[rightMotor.AccelPointer] * 0.5;
																			/*	电机目标速度计算		*/	
	if (rightMotor.targetV >= 1000000) rightMotor.targetV = 1000000;		
	rightMotor.AccelTimeLeft[rightMotor.AccelPointer] -= 1;
	if (rightMotor.AccelTimeLeft[rightMotor.AccelPointer] == 0)
		rightMotor.AccelPointer += 1;
}

/*********************************************************************************************************
**	函数原型:	void SearchTurnLeft(void)
**	函数功能:	电脑鼠搜索左转
**	输入参数:	无
**	输出参数:	无
**	返回参数:	无
**	修改时间:	2014-4-28 14:30 
*********************************************************************************************************/
void SearchTurnLeft(void)
{
	IRSensor.IsCorrectionAllowed = FALSE;									/*	红外矫正禁止			*/

	leftMotor.AccelValue[0] = -1300;										/*	左轮减速度设定			*/
	leftMotor.AccelTimeLeft[0] = 400;										/*	减速时间				*/
	leftMotor.AccelPointer = 0;												/*	初始化变速段数			*/

	rightMotor.AccelValue[0] = 1300;										/*	右轮加速度设定			*/
	rightMotor.AccelTimeLeft[0] = 400;										/*	加速时间				*/
	rightMotor.AccelPointer = 0;											/*	初始化变速段数			*/

	mouse.vertD = 0;														/*	清零当前位移			*/
	
	leftMotor.AccelValue[1] = 0;											/*	设定左轮匀速			*/
	leftMotor.AccelTimeLeft[1] = 20;										/*	匀速运行时间			*/
	rightMotor.AccelValue[1] = 0;											/*	设定右轮匀速			*/
	rightMotor.AccelTimeLeft[1] = 20;										/*	匀速运行时间			*/

	leftMotor.AccelValue[2] = 1300;											/*	左轮加速度设定			*/
	leftMotor.AccelTimeLeft[2] = 400;										/*	加速时间				*/							
	rightMotor.AccelValue[2] = -1300;										/*	右轮减速度设定			*/
	rightMotor.AccelTimeLeft[2] = 400;										/*	减速时间				*/

	mouse.dir = (mouse.dir + 3) % 4;										/*	电脑鼠绝对方向记录		*/
	
	while (leftMotor.AccelPointer != 3);									/*	等待电脑鼠完成变速曲线	*/
	
	leftMotor.AccelValue[0] = 0;											/*	电机匀速运行			*/
	leftMotor.AccelTimeLeft[0] = 19200;										/*	足够长的时间设定		*/
	leftMotor.AccelPointer = 0;												/*	变速运行段数			*/
	rightMotor.AccelValue[0] = 0;											/*	电机匀速运行			*/
	rightMotor.AccelTimeLeft[0] = 19200;									/*	足够长的时间设定		*/
	rightMotor.AccelPointer = 0;											/*	变速运行段数			*/
	
	IRSensor.IsCorrectionAllowed = TRUE;									/*	红外矫正使能			*/
	mouse.vertD = 115500;													/*	过弯之后位置设定		*/
	mouse.coorSwt = 0;
}

/*********************************************************************************************************
**	函数原型:	void SearchTurnRight(void)
**	函数功能:	电脑鼠搜索右转
**	输入参数:	无
**	输出参数:	无
**	返回参数:	无
**	修改时间:	2014-4-28 14:30 
*********************************************************************************************************/
void SearchTurnRight(void)
{
	while (mouse.vertD <= 155000);											/*	微调入弯位置			*/
	IRSensor.IsCorrectionAllowed = FALSE;	
	
	leftMotor.AccelValue[0] = 1300;	
	leftMotor.AccelTimeLeft[0] = 400;
	leftMotor.AccelPointer = 0;
	
	rightMotor.AccelValue[0] = -1300;
	rightMotor.AccelTimeLeft[0] = 400;
	rightMotor.AccelPointer = 0;

	mouse.vertD = 0;
	
	leftMotor.AccelValue[1] = 0;
	leftMotor.AccelTimeLeft[1] = 5;
	rightMotor.AccelValue[1] = 0;
	rightMotor.AccelTimeLeft[1] = 5;	

	leftMotor.AccelValue[2] = -1300;
	leftMotor.AccelTimeLeft[2] = 400;
	rightMotor.AccelValue[2] = 1300;
	rightMotor.AccelTimeLeft[2] = 400;

	mouse.dir = (mouse.dir + 1) % 4;
	
	while (leftMotor.AccelPointer != 3);
	
	leftMotor.AccelValue[1] = 0;
	leftMotor.AccelTimeLeft[1] = 19200;
	leftMotor.AccelPointer = 1;	
	rightMotor.AccelValue[1] = 0;
	rightMotor.AccelTimeLeft[1] = 19200;	
	rightMotor.AccelPointer = 1;	
	
	IRSensor.IsCorrectionAllowed = TRUE;
	mouse.vertD = 107500;
	mouse.coorSwt = 0;
}

/*********************************************************************************************************
**	函数原型:	void MouseStart(void)
**	函数功能:	电脑鼠启动
**	输入参数:	无
**	输出参数:	无
**	返回参数:	无
**	修改时间:	2014-5-13 14:30 
*********************************************************************************************************/
void MouseStart(void)
{	
	leftMotor_ClearCache();													/*	清空左轮运行参数		*/
	rightMotor_ClearCache();												/*	清空右轮运行参数		*/
	IRSensor.IsCorrectionAllowed = TRUE;									/*	允许红外矫正			*/
		
	leftMotor.AccelValue[0] = 2000;											/*	加速度设定				*/
	leftMotor.AccelTimeLeft[0] = 300;										/*	加速时间				*/
	leftMotor.AccelPointer = 0;								
	rightMotor.AccelValue[0] = 2000;										/*	加速度设定				*/		
	rightMotor.AccelTimeLeft[0] = 300;										/*	加速时间				*/
	rightMotor.AccelPointer = 0;
	
	leftMotor.AccelValue[1] = 0;											/*	加速完成 匀速运行		*/
	leftMotor.AccelTimeLeft[1] = 19200;										/*	设定足够长的运行时间	*/
	rightMotor.AccelValue[1] = 0;
	rightMotor.AccelTimeLeft[1] = 19200;
}

/*********************************************************************************************************
**	函数原型:	void SearchBreak(void)
**	函数功能:	电脑鼠刹车函数
**	输入参数:	无
**	输出参数:	无
**	返回参数:	无
**	修改时间:	2014-4-28 14:30 
*********************************************************************************************************/
void SearchBreak(void)
{
	IRSensor.IsCorrectionAllowed = FALSE;									/*	禁止红外矫正			*/
	leftMotor.AccelValue[0] = -1000;										/*	设定减速度				*/
	leftMotor.AccelTimeLeft[0] = 600;										/*	设定减速时间			*/
	leftMotor.AccelPointer = 0;	
	rightMotor.AccelValue[0] = -1000;
	rightMotor.AccelTimeLeft[0] = 600;	
	rightMotor.AccelPointer = 0;		
	
	leftMotor.AccelValue[1] = 0;											/*	减速完成 微调姿态		*/
	leftMotor.AccelTimeLeft[1] = 50;										/*	停车姿态微调时间		*/
	rightMotor.AccelValue[1] = 0;
	rightMotor.AccelTimeLeft[1] = 50;
	while (leftMotor.AccelPointer != 2);									/*	等待过程完成			*/
}
/*********************************************************************************************************
**	函数原型:	void MouseTurnBack(void)
**	函数功能:	电脑鼠搜索回转
**	输入参数:	无
**	输出参数:	无
**	返回参数:	无
**	修改时间:	2014-4-28 14:30 
*********************************************************************************************************/
void MouseTurnBack(void)
{
	IRSensor.IsCorrectionAllowed = FALSE;
	leftMotor_ClearCache();
	rightMotor_ClearCache();	
	
	leftMotor.AccelValue[0] = -2000;
	leftMotor.AccelTimeLeft[0] = 300;	
	leftMotor.AccelPointer = 0;	
	rightMotor.AccelValue[0] = 2000;
	rightMotor.AccelTimeLeft[0] = 300;	
	rightMotor.AccelPointer = 0;
	
	mouse.vertD = 0;
	
	leftMotor.AccelValue[1] = 0;
	leftMotor.AccelTimeLeft[1] = 480;	
	rightMotor.AccelValue[1] = 0;
	rightMotor.AccelTimeLeft[1] = 480;	
	
	leftMotor.AccelValue[2] = 2000;
	leftMotor.AccelTimeLeft[2] = 300;	
	rightMotor.AccelValue[2] = -2000;
	rightMotor.AccelTimeLeft[2] = 300;	
	
	leftMotor.AccelValue[3] = 0;
	leftMotor.AccelTimeLeft[3] = 50;	
	rightMotor.AccelValue[3] = 0;
	rightMotor.AccelTimeLeft[3] = 50;		

	mouse.dir = (mouse.dir + 2) % 4;
	while (leftMotor.AccelPointer != 4);
	mouse.vertD = 0;
	MouseGoBack();
}

/*********************************************************************************************************
**	函数原型:	void MouseGoBack(void)
**	函数功能:	电脑鼠搜索回转之后后退校正
**	输入参数:	无
**	输出参数:	无
**	返回参数:	无
**	修改时间:	2014-4-28 14:30 
*********************************************************************************************************/
void MouseGoBack(void)
{
	IRSensor.IsCorrectionAllowed = TRUE;
	leftMotor_ClearCache();
	rightMotor_ClearCache();	
	
	leftMotor.AccelValue[0] = -1000;
	leftMotor.AccelTimeLeft[0] = 400;	
	leftMotor.AccelPointer = 0;	
	rightMotor.AccelValue[0] = -1000;
	rightMotor.AccelTimeLeft[0] = 400;	
	rightMotor.AccelPointer = 0;	

	leftMotor.AccelValue[1] = 0;
	leftMotor.AccelTimeLeft[1] = 150;	
	rightMotor.AccelValue[1] = 0;
	rightMotor.AccelTimeLeft[1] = 150;	
	
	leftMotor.AccelValue[2] = 1000;
	leftMotor.AccelTimeLeft[2] = 400;	
	rightMotor.AccelValue[2] = 1000;
	rightMotor.AccelTimeLeft[2] = 400;	
	
	leftMotor.AccelValue[3] = 0;
	leftMotor.AccelTimeLeft[3] = 50;	
	rightMotor.AccelValue[3] = 0;
	rightMotor.AccelTimeLeft[3] = 50;	
	
	while (leftMotor.AccelPointer != 4);
	mouse.vertD = 5000;
}



