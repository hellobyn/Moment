
/********************************************************************************************************
** 
**                               			北京交通大学                                     
**
**-------------------------------------------------------------------------------------------------------
** 	文件名：		Motor.h
** 	创建时间：		2013-7-9 17：15
** 	创建人员： 		赵秉贤
** 	文件描述:  		电机控制头文件
** 
**-------------------------------------------------------------------------------------------------------
** 	最后修改时间：	2013-7-9 17:20 
** 	最后修改人员：	赵秉贤	
** 	版本号： 	   	V1.0
** 	版本描述： 		进行初步的电机控制实验
**
*********************************************************************************************************/

#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "Interface.h"
#include "IRSensor.h"

/*********************************************************************************************************
**	电机端口定义
*********************************************************************************************************/
#define	LeftMotorPWM 		GPIO_Pin_8							/*	左电机 速度控制 PC8	TIM8_CH3		*/
#define	LeftMotorDIR  		GPIO_Pin_9							/*	左电机 方向控制 PC9	TIM8_CH4		*/
#define	RightMotorPWM		GPIO_Pin_7							/*	右电机 速度控制 PC7	TIM8_CH2		*/	 
#define	RightMotorDIR		GPIO_Pin_6	 						/*	右电机 方向控制 PC6	TIM8_CH1		*/
#define	LeftENCA  			GPIO_Pin_0							/*	左电机 编码器A通道 PA0	TIM5_CH1	*/
#define	LeftENCB  			GPIO_Pin_1							/*	左电机 编码器B通道 PA1	TIM5_CH2	*/
#define	RightENCA 			GPIO_Pin_6							/*	右电机 编码器A通道 PA6	TIM3_CH1	*/
#define	RightENCB 			GPIO_Pin_7							/*	右电机 编码器B通道 PA7	TIM3_CH2	*/

/*********************************************************************************************************
**	电机转动方向设定
*********************************************************************************************************/
#define	leftMotorGoBack		GPIOC->BRR  = GPIO_Pin_9			/*	左电机 反转		端口置高			*/
#define	leftMotorGoAhead 	GPIOC->BSRR = GPIO_Pin_9			/*	左电机 正转		端口置低			*/
#define	rightMotorGoBack 	GPIOC->BRR  = GPIO_Pin_6			/*	右电机 反转		端口置高			*/
#define	rightMotorGoAhead 	GPIOC->BSRR = GPIO_Pin_6			/*	右电机 正转		端口置低			*/

/*********************************************************************************************************
**	电机运行状态定义
*********************************************************************************************************/
#define	MOTORSTOP 			0									/*	电机停止转动						*/
#define	MOTORRUN	 		1									/*	电机运行							*/

/*********************************************************************************************************
**	电机运行属性结构体定义
*********************************************************************************************************/
struct __Motor
{	
	u16 	State;												/*	电机运行状态 						*/
	double  AccelValue[100];									/*	电机加速度大小	mm/s2				*/
	u16		AccelTimeLeft[100];									/*	电机加速时间	0.5ms				*/
	u16		AccelPointer;										/*	电机加速段数 	无量纲				*/
	double	targetD;											/*	电机目标位移	um					*/
	double  targetV;											/*	电机目标速度	um/s				*/
	double 	acturalD;											/*	电机当前位移 计算值					*/
	double  disError[3];										/*	电机位移运行误差					*/
	double 	valueP, valueI, valueD, deltaPID;					/*	PID运行参数值						*/
	s32		motorPID;											/*	PID运算输出值						*/
	s16		PWM;												/*	实际电机输出PWM值					*/
};
typedef struct __Motor MOTOR;									/*	重命名结构体motor					*/

/*********************************************************************************************************
**	全局变量定义
*********************************************************************************************************/
extern MOTOR leftMotor;											/*	左电机状态声明						*/
extern MOTOR rightMotor;										/*	右电机状态声明						*/	

/*********************************************************************************************************
**	电机相关参数定义
*********************************************************************************************************/
#define	KpENC	 			500									/*	速度PID控制器比例系数Kp				*/
#define	KiENC	 			0									/*	速度PID控制器积分系数Ki				*/
#define	KdENC 				50									/*	速度PID控制器微分系数Kd				*/
#define	KpIR 				90									/*	速度PID控制器比例系数Kp				*/
#define	KiIR 				0									/*	速度PID控制器积分系数Ki				*/
#define	KdIR 				50									/*	速度PID控制器微分系数Kd				*/
#define	ENCToDis	 		11.43								/*	编码器->位移 转化系数 单位um		*/
																/*	ENDToDis = 4.602 * 车轮直径 取整	*/

/*********************************************************************************************************
**	电机控制函数声明
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
