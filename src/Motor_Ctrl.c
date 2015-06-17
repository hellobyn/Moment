
/*********************************************************************************************************
** 
**                               			������ͨ��ѧ                                     
**
**--------------------------------------------------------------------------------------------------------
** 	�ļ�����		Motor_Ctrl.c
** 	����ʱ�䣺		2013-7-9 17��20
** 	������Ա�� 		�Ա���
** 	�ļ�����:  		�������Դ�ļ�
** 
**--------------------------------------------------------------------------------------------------------
** 	����޸�ʱ�䣺	2014-2-18 18:30 
** 	����޸���Ա��	�Ա���	
** 	�汾�ţ� 	   	V1.0
** 	�汾������ 		���г����ĵ������ʵ��
**
*********************************************************************************************************/

#include "Motor.h"
#include "Maze.h"
#include "Delay.h"

/*********************************************************************************************************
**	ȫ�ֱ�������
*********************************************************************************************************/
MOTOR leftMotor;															/*	��ʼ������״̬		*/
MOTOR rightMotor;															/*	��ʼ���ҵ��״̬		*/

/*********************************************************************************************************
**	����ԭ��:	void leftMotor_Constructor(void)
**	��������:	������ʼ������
**	�������:	��
**	�������:	��
**	���ز���:	��
**	�޸�ʱ��:	2013-7-12	15:00
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
**	����ԭ��:	void rightMotor_Constructor(void)
**	��������:	�ҵ����ʼ������
**	�������:	��
**	�������:	��
**	���ز���:	��
**	�޸�ʱ��:	2013-7-12	15:00
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
**	����ԭ��:	void leftMotor_ClearCache(void)
**	��������:	�������в������
**	�������:	��
**	�������:	��
**	���ز���:	��
**	�޸�ʱ��:	2013-7-12	15:00
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
**	����ԭ��:	void rightMotor_Constructor(void)
**	��������:	�ҵ�����в������
**	�������:	��
**	�������:	��
**	���ز���:	��
**	�޸�ʱ��:	2013-7-12	15:00
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
**	����ԭ��:	void MotorInit(void)
**	��������:	���������ʼ������
**	�������:	��
**	�������:	��
**	���ز���:	��
**	�޸�ʱ��:	2013-7-12	15:00
*********************************************************************************************************/
void MotorInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8 , ENABLE);					/*	ʹ�ܶ�ʱ��8ʱ��			*/
																	
    GPIO_InitStructure.GPIO_Pin = LeftMotorPWM | RightMotorPWM;				/*	���ҵ�����ٶ˿�����	*/
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;                 		/*	�����������			*/
    GPIO_Init(GPIOC, &GPIO_InitStructure);
																	  
    GPIO_InitStructure.GPIO_Pin = LeftMotorDIR | RightMotorDIR;				/*	���ҵ������˿�����	*/
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;				 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;                 		/*	��©���				*/
    GPIO_Init(GPIOC,&GPIO_InitStructure);

	TIM_TimeBaseStructure.TIM_Period = 2879;             					/*	PWMƵ�� = 25kHz			*/
    TIM_TimeBaseStructure.TIM_Prescaler = 0;            					/*	����Ԥ��Ƶֵ = 1		*/
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;    						/*	ʱ�ӷָ��Ϊ1��		*/
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;				/*	��ʱ�����ϼ���ģʽ		*/
	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);
	                        
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;						/*	��ʱ������ΪPWM1ģʽ	*/
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;			/*	ʹ������Ƚ�״̬		*/
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;				/*	��ʱ������Ƚϼ��Ը�	*/
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;				/*	MOE=0 ��������ȽϿ���	*/
	TIM_OCInitStructure.TIM_Pulse = 0;										/*	PWM��ʼռ�ձ� = 0		*/
	TIM_OC2Init(TIM8, &TIM_OCInitStructure);								/*	��ʼ����ʱ��8��2ͨ��	*/
	TIM_OC3Init(TIM8, &TIM_OCInitStructure);								/*	��ʼ����ʱ��8��3ͨ��	*/

	TIM_OC2PreloadConfig(TIM8, TIM_OCPreload_Enable);						/*	ʹ��CCR2��Ԥװ�ؼĴ���	*/
	TIM_OC3PreloadConfig(TIM8, TIM_OCPreload_Enable);						/*	ʹ��CCR3��Ԥװ�ؼĴ���	*/

	TIM_ARRPreloadConfig(TIM8, ENABLE);										/*	ʹ��ARR��Ԥװ�ؼĴ���	*/
	TIM_Cmd(TIM8, ENABLE);													/*	ʹ�ܶ�ʱ��8				*/
	TIM_CtrlPWMOutputs(TIM8, ENABLE);  										/*	ʹ�ܶ�ʱ��8��PWM�������*/
	leftMotorGoAhead;														/*	������ƶ��õ�			*/
	rightMotorGoAhead;														/*	������ƶ��õ�			*/
	leftMotor_Constructor();
	rightMotor_Constructor();
//	MotorENCInit();															/*	����������ʼ��		*/
}

/*********************************************************************************************************
**	����ԭ��:	void MotorENCInit(void)
**	��������:	�����������ʼ������
**	�������:	��
**	�������:	��
**	���ز���:	��
**	�޸�ʱ��:	2014-2-13 11:20 
*********************************************************************************************************/
void MotorENCInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);					/*	ʹ�ܶ�ʱ��3ʱ��			*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);					/*	ʹ�ܶ�ʱ��5ʱ��			*/
	
	GPIO_StructInit(&GPIO_InitStructure);   
	GPIO_InitStructure.GPIO_Pin = LeftENCA | LeftENCB | 					/*	�����������˿�����	*/
								 RightENCA | RightENCB;						/*	�ҵ���������˿�����	*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 					/*	��������				*/
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	TIM_TimeBaseStructure.TIM_Period = 32000;             					/*	��������ֵ������ж�	*/
    TIM_TimeBaseStructure.TIM_Prescaler = 0;            					/*	����Ԥ��Ƶֵ = 1		*/
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;    				/*	ʱ�ӷָ��Ϊ1��		*/
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;				/*	��ʱ�����ϼ���ģʽ		*/
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
	
	TIM_DeInit(TIM3);
	TIM_DeInit(TIM5);
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);

	TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12,TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	TIM_EncoderInterfaceConfig(TIM5, TIM_EncoderMode_TI12,TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	TIM_ICStructInit(&TIM_ICInitStructure);									/*	���ö�ʱ��Ϊ������ģʽ	*/
	
	TIM_ICInitStructure.TIM_ICFilter = 0x0; 								/*	�Ƚ��˲���������Ϊ0		*/
	TIM_ICInit(TIM3, &TIM_ICInitStructure);
	TIM_ICInit(TIM5, &TIM_ICInitStructure);

	TIM_ClearFlag(TIM3, TIM_FLAG_Update);									/*	���TIM3�жϱ�־λ		*/
	TIM_ClearFlag(TIM5, TIM_FLAG_Update);									/*	���TIM5�жϱ�־λ		*/
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);								/*	ʹ��TIM3�ж�Դ			*/
	TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);								/*	ʹ��TIM5�ж�Դ			*/
	
	TIM3->CNT = 32000;														/*	TIM3��ʼ����ֵ����		*/
	TIM5->CNT = 32000;														/*	TIM5��ʼ����ֵ����		*/
	TIM_Cmd(TIM3, ENABLE);													/*	ʹ������ ��ʱ��3		*/
	TIM_Cmd(TIM5, ENABLE);													/*	ʹ������ ��ʱ��5		*/
}

/*********************************************************************************************************
**	����ԭ��:	void LeftMotorDrv(s16 leftPWM)
**	��������:	������������
**	�������:	��ת�����PWM���ռ�ձ� ��0-1000��
				��ת�����PWM���ռ�ձ� ��2879-1879��		
**	�������:	��
**	���ز���:	��
**	�޸�ʱ��:	2014-2-18 14:30 
*********************************************************************************************************/
void LeftMotorDrv(s16 leftPWM)
{
	leftMotor.PWM = leftPWM;
	if(leftPWM >= 0)														/*	���Ŀ���ٶ�Ϊ����		*/
	{
		if(leftPWM >= 1500) leftPWM = 1500;									/*	����PWM�������Ϊ1200	*/
		leftMotorGoAhead;													/*	ѡ����ת������		*/
	}
	else																	/*	���Ŀ���ٶ�Ϊ����		*/
	{	
		if(leftPWM <= -1500) leftPWM = -1500;								/*	����PWM�������Ϊ-1200	*/
		leftPWM = 2879 + leftPWM;											/*	�����PWMռ�ձ�ȡ��		*/
		leftMotorGoBack;													/*	ѡ����ת������		*/
	}
	TIM_SetCompare3(TIM8,leftPWM);											/*	�趨PWM���ռ�ձ�		*/
}

/*********************************************************************************************************
**	����ԭ��:	void leftmotorPID(void)
**	��������:	��������PID����
**	�������:	��
**	�������:	��
**	���ز���:	��
**	�޸�ʱ��:	2014-2-19 15:30 
*********************************************************************************************************/
void LeftmotorPID(void)
{
	leftMotor.disError[2] = leftMotor.disError[1];							/*	e(k-2)��ֵ				*/
	leftMotor.disError[1] = leftMotor.disError[0];							/*	e(k-1)��ֵ				*/
	leftMotor.disError[0] += leftMotor.targetD - leftMotor.acturalD;		/*	e(k)��ֵ				*/
	
	
	leftMotor.valueP = KpENC *( leftMotor.disError[0] - leftMotor.disError[1] );
																			/*	�������Ӳ��ּ���		*/
	leftMotor.valueI = KiENC *  leftMotor.disError[0];						/*	�������Ӳ��ּ���		*/
	leftMotor.valueD = KdENC *( leftMotor.disError[0] - 2 * leftMotor.disError[1] + leftMotor.disError[2] );
																			/*	΢�����Ӳ��ּ���		*/
	leftMotor.deltaPID = ( leftMotor.valueP + leftMotor.valueI + leftMotor.valueD ) / 1000;	
																			/*	PID����ֵ����			*/
	leftMotor.motorPID += leftMotor.deltaPID;								/*	PID������				*/
	LeftMotorDrv( leftMotor.motorPID );										/*	�������������������	*/
	
}

/*********************************************************************************************************
**	����ԭ��:	void leftMotor_TargetDCalculation(void)
**	��������:	�������ƺ���
**	�������:	��
**	�������:	��
**	���ز���:	��
**	�޸�ʱ��:	2014-2-20 10:30 
*********************************************************************************************************/
void leftMotor_TargetDCalculation(void)
{	
	leftMotor.acturalD = TIM_GetCounter(TIM5) - 32000;
	TIM5->CNT = 32000;														/*	�ҵ������������ֵ��λ	*/
	leftMotor.acturalD = leftMotor.acturalD * ENCToDis;						/*	�������ֶ���λ��	um	*/
	
	mouse.vertD += leftMotor.acturalD / 2;									/*	���ʣ��λ�Ƽ���	um	*/
	leftMotor.targetD = (leftMotor.targetV + leftMotor.AccelValue[leftMotor.AccelPointer] * 0.25) * 0.0005;
																			/*	���Ŀ��λ�Ƽ���		*/
	leftMotor.targetV += leftMotor.AccelValue[leftMotor.AccelPointer] * 0.5;
																			/*	���Ŀ���ٶȼ���		*/
	if (leftMotor.targetV >= 1000000) leftMotor.targetV = 1000000;	
	leftMotor.AccelTimeLeft[leftMotor.AccelPointer] -= 1;
	if (leftMotor.AccelTimeLeft[leftMotor.AccelPointer] == 0)
		leftMotor.AccelPointer += 1;
}	

/*********************************************************************************************************
**	����ԭ��:	void RightMotorDrv(s16 rightPWM)
**	��������:	�ҵ����������
**	�������:	��ת�����PWM���ռ�ձ� ��0-1000��
				��ת�����PWM���ռ�ձ� ��2879-1879��
**	�������:	��
**	���ز���:	��
**	�޸�ʱ��:	2014-2-18 14:30 
*********************************************************************************************************/
void RightMotorDrv(s16 rightPWM)
{
	rightMotor.PWM = rightPWM;
	if(rightPWM >= 0)														/*	���Ŀ���ٶ�Ϊ����		*/
	{	
		if(rightPWM >= 1500) rightPWM = 1500;								/*	����PWM�������Ϊ1000	*/
		rightMotorGoAhead;													/*	ѡ����ת������		*/
	}
	else
	{
		if(rightPWM <= -1500) rightPWM = -1500;								/*	����PWM�������Ϊ1000	*/
		rightPWM = 2879 + rightPWM;											/*	��תPWMռ�ձ�ȡ��		*/
		rightMotorGoBack;													/*	ѡ����ת������		*/
	}
	TIM_SetCompare2(TIM8,rightPWM);											/*	�趨PWM���ռ�ձ�		*/
}

/*********************************************************************************************************
**	����ԭ��:	void rightmotorPID(void)
**	��������:	�ҵ������PID����
**	�������:	��
**	�������:	��
**	���ز���:	��
**	�޸�ʱ��:	2014-2-20 10:30 
*********************************************************************************************************/
void RightmotorPID(void)
{
	rightMotor.disError[2] = rightMotor.disError[1];						/*	e(k-2)��ֵ				*/
	rightMotor.disError[1] = rightMotor.disError[0];						/*	e(k-1)��ֵ				*/
	rightMotor.disError[0] += rightMotor.targetD - rightMotor.acturalD;		/*	e(k)��ֵ				*/
	
	
	rightMotor.valueP = KpENC *( rightMotor.disError[0] - rightMotor.disError[1] );
																			/*	�������Ӳ��ּ���		*/
	rightMotor.valueI = KiENC *  rightMotor.disError[0];					/*	�������Ӳ��ּ���		*/
	rightMotor.valueD = KdENC *( rightMotor.disError[0] - 2 * rightMotor.disError[1] + rightMotor.disError[2] );
																			/*	΢�����Ӳ��ּ���		*/
	rightMotor.deltaPID = ( rightMotor.valueP + rightMotor.valueI + rightMotor.valueD ) / 1000;	
																			/*	PID����ֵ����			*/
	rightMotor.motorPID = rightMotor.motorPID + rightMotor.deltaPID;		/*	PID������				*/
	RightMotorDrv( rightMotor.motorPID );									/*	�������������������	*/
}

/*********************************************************************************************************
**	����ԭ��:	void rightMotor_TargetDCalculation(void)
**	��������:	�ҵ�����ƺ���
**	�������:	��
**	�������:	��
**	���ز���:	��
**	�޸�ʱ��:	2014-2-20 10:30 
*********************************************************************************************************/
void rightMotor_TargetDCalculation(void)
{ 	
	rightMotor.acturalD = 32000 - TIM_GetCounter(TIM3);
	TIM3->CNT = 32000;														/*	��������������ֵ��λ	*/
	rightMotor.acturalD = rightMotor.acturalD * ENCToDis;					/*	�������ֶ���λ�� um		*/
	mouse.vertD += rightMotor.acturalD / 2;
	rightMotor.targetD = (rightMotor.targetV + rightMotor.AccelValue[rightMotor.AccelPointer] * 0.25) * 0.0005;
																			/*	���Ŀ��λ�Ƽ���		*/
	rightMotor.targetV += rightMotor.AccelValue[rightMotor.AccelPointer] * 0.5;
																			/*	���Ŀ���ٶȼ���		*/	
	if (rightMotor.targetV >= 1000000) rightMotor.targetV = 1000000;		
	rightMotor.AccelTimeLeft[rightMotor.AccelPointer] -= 1;
	if (rightMotor.AccelTimeLeft[rightMotor.AccelPointer] == 0)
		rightMotor.AccelPointer += 1;
}

/*********************************************************************************************************
**	����ԭ��:	void SearchTurnLeft(void)
**	��������:	������������ת
**	�������:	��
**	�������:	��
**	���ز���:	��
**	�޸�ʱ��:	2014-4-28 14:30 
*********************************************************************************************************/
void SearchTurnLeft(void)
{
	IRSensor.IsCorrectionAllowed = FALSE;									/*	���������ֹ			*/

	leftMotor.AccelValue[0] = -1300;										/*	���ּ��ٶ��趨			*/
	leftMotor.AccelTimeLeft[0] = 400;										/*	����ʱ��				*/
	leftMotor.AccelPointer = 0;												/*	��ʼ�����ٶ���			*/

	rightMotor.AccelValue[0] = 1300;										/*	���ּ��ٶ��趨			*/
	rightMotor.AccelTimeLeft[0] = 400;										/*	����ʱ��				*/
	rightMotor.AccelPointer = 0;											/*	��ʼ�����ٶ���			*/

	mouse.vertD = 0;														/*	���㵱ǰλ��			*/
	
	leftMotor.AccelValue[1] = 0;											/*	�趨��������			*/
	leftMotor.AccelTimeLeft[1] = 20;										/*	��������ʱ��			*/
	rightMotor.AccelValue[1] = 0;											/*	�趨��������			*/
	rightMotor.AccelTimeLeft[1] = 20;										/*	��������ʱ��			*/

	leftMotor.AccelValue[2] = 1300;											/*	���ּ��ٶ��趨			*/
	leftMotor.AccelTimeLeft[2] = 400;										/*	����ʱ��				*/							
	rightMotor.AccelValue[2] = -1300;										/*	���ּ��ٶ��趨			*/
	rightMotor.AccelTimeLeft[2] = 400;										/*	����ʱ��				*/

	mouse.dir = (mouse.dir + 3) % 4;										/*	��������Է����¼		*/
	
	while (leftMotor.AccelPointer != 3);									/*	�ȴ���������ɱ�������	*/
	
	leftMotor.AccelValue[0] = 0;											/*	�����������			*/
	leftMotor.AccelTimeLeft[0] = 19200;										/*	�㹻����ʱ���趨		*/
	leftMotor.AccelPointer = 0;												/*	�������ж���			*/
	rightMotor.AccelValue[0] = 0;											/*	�����������			*/
	rightMotor.AccelTimeLeft[0] = 19200;									/*	�㹻����ʱ���趨		*/
	rightMotor.AccelPointer = 0;											/*	�������ж���			*/
	
	IRSensor.IsCorrectionAllowed = TRUE;									/*	�������ʹ��			*/
	mouse.vertD = 115500;													/*	����֮��λ���趨		*/
	mouse.coorSwt = 0;
}

/*********************************************************************************************************
**	����ԭ��:	void SearchTurnRight(void)
**	��������:	������������ת
**	�������:	��
**	�������:	��
**	���ز���:	��
**	�޸�ʱ��:	2014-4-28 14:30 
*********************************************************************************************************/
void SearchTurnRight(void)
{
	while (mouse.vertD <= 155000);											/*	΢������λ��			*/
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
**	����ԭ��:	void MouseStart(void)
**	��������:	����������
**	�������:	��
**	�������:	��
**	���ز���:	��
**	�޸�ʱ��:	2014-5-13 14:30 
*********************************************************************************************************/
void MouseStart(void)
{	
	leftMotor_ClearCache();													/*	����������в���		*/
	rightMotor_ClearCache();												/*	����������в���		*/
	IRSensor.IsCorrectionAllowed = TRUE;									/*	����������			*/
		
	leftMotor.AccelValue[0] = 2000;											/*	���ٶ��趨				*/
	leftMotor.AccelTimeLeft[0] = 300;										/*	����ʱ��				*/
	leftMotor.AccelPointer = 0;								
	rightMotor.AccelValue[0] = 2000;										/*	���ٶ��趨				*/		
	rightMotor.AccelTimeLeft[0] = 300;										/*	����ʱ��				*/
	rightMotor.AccelPointer = 0;
	
	leftMotor.AccelValue[1] = 0;											/*	������� ��������		*/
	leftMotor.AccelTimeLeft[1] = 19200;										/*	�趨�㹻��������ʱ��	*/
	rightMotor.AccelValue[1] = 0;
	rightMotor.AccelTimeLeft[1] = 19200;
}

/*********************************************************************************************************
**	����ԭ��:	void SearchBreak(void)
**	��������:	������ɲ������
**	�������:	��
**	�������:	��
**	���ز���:	��
**	�޸�ʱ��:	2014-4-28 14:30 
*********************************************************************************************************/
void SearchBreak(void)
{
	IRSensor.IsCorrectionAllowed = FALSE;									/*	��ֹ�������			*/
	leftMotor.AccelValue[0] = -1000;										/*	�趨���ٶ�				*/
	leftMotor.AccelTimeLeft[0] = 600;										/*	�趨����ʱ��			*/
	leftMotor.AccelPointer = 0;	
	rightMotor.AccelValue[0] = -1000;
	rightMotor.AccelTimeLeft[0] = 600;	
	rightMotor.AccelPointer = 0;		
	
	leftMotor.AccelValue[1] = 0;											/*	������� ΢����̬		*/
	leftMotor.AccelTimeLeft[1] = 50;										/*	ͣ����̬΢��ʱ��		*/
	rightMotor.AccelValue[1] = 0;
	rightMotor.AccelTimeLeft[1] = 50;
	while (leftMotor.AccelPointer != 2);									/*	�ȴ��������			*/
}
/*********************************************************************************************************
**	����ԭ��:	void MouseTurnBack(void)
**	��������:	������������ת
**	�������:	��
**	�������:	��
**	���ز���:	��
**	�޸�ʱ��:	2014-4-28 14:30 
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
**	����ԭ��:	void MouseGoBack(void)
**	��������:	������������ת֮�����У��
**	�������:	��
**	�������:	��
**	���ز���:	��
**	�޸�ʱ��:	2014-4-28 14:30 
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



