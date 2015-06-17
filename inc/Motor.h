
/********************************************************************************************************
** 
**                               			������ͨ��ѧ                                     
**
**-------------------------------------------------------------------------------------------------------
** 	�ļ�����		Motor.h
** 	����ʱ�䣺		2013-7-9 17��15
** 	������Ա�� 		�Ա���
** 	�ļ�����:  		�������ͷ�ļ�
** 
**-------------------------------------------------------------------------------------------------------
** 	����޸�ʱ�䣺	2013-7-9 17:20 
** 	����޸���Ա��	�Ա���	
** 	�汾�ţ� 	   	V1.0
** 	�汾������ 		���г����ĵ������ʵ��
**
*********************************************************************************************************/

#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "Interface.h"
#include "IRSensor.h"

/*********************************************************************************************************
**	����˿ڶ���
*********************************************************************************************************/
#define	LeftMotorPWM 		GPIO_Pin_8							/*	���� �ٶȿ��� PC8	TIM8_CH3		*/
#define	LeftMotorDIR  		GPIO_Pin_9							/*	���� ������� PC9	TIM8_CH4		*/
#define	RightMotorPWM		GPIO_Pin_7							/*	�ҵ�� �ٶȿ��� PC7	TIM8_CH2		*/	 
#define	RightMotorDIR		GPIO_Pin_6	 						/*	�ҵ�� ������� PC6	TIM8_CH1		*/
#define	LeftENCA  			GPIO_Pin_0							/*	���� ������Aͨ�� PA0	TIM5_CH1	*/
#define	LeftENCB  			GPIO_Pin_1							/*	���� ������Bͨ�� PA1	TIM5_CH2	*/
#define	RightENCA 			GPIO_Pin_6							/*	�ҵ�� ������Aͨ�� PA6	TIM3_CH1	*/
#define	RightENCB 			GPIO_Pin_7							/*	�ҵ�� ������Bͨ�� PA7	TIM3_CH2	*/

/*********************************************************************************************************
**	���ת�������趨
*********************************************************************************************************/
#define	leftMotorGoBack		GPIOC->BRR  = GPIO_Pin_9			/*	���� ��ת		�˿��ø�			*/
#define	leftMotorGoAhead 	GPIOC->BSRR = GPIO_Pin_9			/*	���� ��ת		�˿��õ�			*/
#define	rightMotorGoBack 	GPIOC->BRR  = GPIO_Pin_6			/*	�ҵ�� ��ת		�˿��ø�			*/
#define	rightMotorGoAhead 	GPIOC->BSRR = GPIO_Pin_6			/*	�ҵ�� ��ת		�˿��õ�			*/

/*********************************************************************************************************
**	�������״̬����
*********************************************************************************************************/
#define	MOTORSTOP 			0									/*	���ֹͣת��						*/
#define	MOTORRUN	 		1									/*	�������							*/

/*********************************************************************************************************
**	����������Խṹ�嶨��
*********************************************************************************************************/
struct __Motor
{	
	u16 	State;												/*	�������״̬ 						*/
	double  AccelValue[100];									/*	������ٶȴ�С	mm/s2				*/
	u16		AccelTimeLeft[100];									/*	�������ʱ��	0.5ms				*/
	u16		AccelPointer;										/*	������ٶ��� 	������				*/
	double	targetD;											/*	���Ŀ��λ��	um					*/
	double  targetV;											/*	���Ŀ���ٶ�	um/s				*/
	double 	acturalD;											/*	�����ǰλ�� ����ֵ					*/
	double  disError[3];										/*	���λ���������					*/
	double 	valueP, valueI, valueD, deltaPID;					/*	PID���в���ֵ						*/
	s32		motorPID;											/*	PID�������ֵ						*/
	s16		PWM;												/*	ʵ�ʵ�����PWMֵ					*/
};
typedef struct __Motor MOTOR;									/*	�������ṹ��motor					*/

/*********************************************************************************************************
**	ȫ�ֱ�������
*********************************************************************************************************/
extern MOTOR leftMotor;											/*	����״̬����						*/
extern MOTOR rightMotor;										/*	�ҵ��״̬����						*/	

/*********************************************************************************************************
**	�����ز�������
*********************************************************************************************************/
#define	KpENC	 			500									/*	�ٶ�PID����������ϵ��Kp				*/
#define	KiENC	 			0									/*	�ٶ�PID����������ϵ��Ki				*/
#define	KdENC 				50									/*	�ٶ�PID������΢��ϵ��Kd				*/
#define	KpIR 				90									/*	�ٶ�PID����������ϵ��Kp				*/
#define	KiIR 				0									/*	�ٶ�PID����������ϵ��Ki				*/
#define	KdIR 				50									/*	�ٶ�PID������΢��ϵ��Kd				*/
#define	ENCToDis	 		11.43								/*	������->λ�� ת��ϵ�� ��λum		*/
																/*	ENDToDis = 4.602 * ����ֱ�� ȡ��	*/

/*********************************************************************************************************
**	������ƺ�������
*********************************************************************************************************/
void MotorInit(void);
void MotorENCInit(void);
void LeftMotorDrv(s16 leftPWM);
void RightMotorDrv(s16 rightPWM);
void LeftmotorPID(void);
void RightmotorPID(void);
void leftMotor_TargetDCalculation(void);
void rightMotor_TargetDCalculation(void);
void SearchTurnLeft(void);
void SearchTurnRight(void);
void MouseStart(void);
void SearchBreak(void);
void MouseTurnBack(void);
void MouseGoBack(void);
void leftMotor_Constructor(void);
void rightMotor_Constructor(void);
void leftMotor_ClearCache(void);
void rightMotor_ClearCache(void);
