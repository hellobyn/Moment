
/*********************************************************************************************************
** 
**                               			������ͨ��ѧ                                     
**
**--------------------------------------------------------------------------------------------------------
** 	�ļ�����		IRSensor.h
** 	����ʱ�䣺		2013-7-16 11:15
** 	������Ա�� 		�Ա���
** 	�ļ�����:  		������ͷ�ļ�
** 
**--------------------------------------------------------------------------------------------------------
** 	����޸�ʱ�䣺	2013-7-16 11:15 
** 	����޸���Ա��	�Ա���	
** 	�汾�ţ� 	   	V1.0
** 	�汾������ 		������ADC��ϴ���ͨ��
**
*********************************************************************************************************/

#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"

/*********************************************************************************************************
**	���⴫�����˿ڶ���
*********************************************************************************************************/
#define	IR1 				GPIO_Pin_4							/*	���⴫����1 �� 	PC4				*/
#define	IR2  				GPIO_Pin_5							/*	���⴫����2 ��ǰ��	PC5				*/
#define	IR3 				GPIO_Pin_0							/*	���⴫����3 ��ǰ��	PB0				*/
#define	IR4  				GPIO_Pin_1							/*	���⴫����4 �ҷ�	PB1				*/

#define	ADC_1 				ADC_Channel_14						/*	���⴫����1 �� 	ADCͨ��14		*/
#define	ADC_2  				ADC_Channel_15						/*	���⴫����2 ��ǰ��	ADCͨ��15		*/
#define	ADC_3 				ADC_Channel_8						/*	���⴫����3 ��ǰ��	ADCͨ��8		*/
#define	ADC_4  				ADC_Channel_9						/*	���⴫����4 �ҷ�	ADCͨ��9		*/

#define	IR13Ctrl 			GPIO_Pin_10							/*	���⴫����1��3 ���ƶ�	PC10		*/
#define	IR24Ctrl  			GPIO_Pin_2							/*	���⴫����2��4 ���ƶ�	PA2			*/

#define	IR13On 				GPIOC->BSRR = GPIO_Pin_10			/*	���⴫����1��3 ��	PC10			*/
#define	IR13Off 			GPIOC->BRR  = GPIO_Pin_10			/*	���⴫����1��3 ��	PC10			*/
#define	IR24On  			GPIOA->BSRR = GPIO_Pin_2			/*	���⴫����2��4 ��	PA2				*/
#define	IR24Off  			GPIOA->BRR  = GPIO_Pin_2			/*	���⴫����2��4 ��	PA2				*/

#define ADC1_DR_Address		((u32)0x4001244C)					/*	DMAͨ���������ַ					*/

/*********************************************************************************************************
**	���⴫�����ṹ�嶨��
*********************************************************************************************************/
struct __IRSensor
{
	u8		IsCorrectionAllowed;								/*	�������ʹ�� TRUE����������		*/
	u8		CrosswayStatus[3];									/*	ǽ����Ϣ�ݴ� �������������			*/
	u16 	ADCValue[4];										/*	���⴫����ADCֵ�洢					*/
	vu16 	ADCCache[4];										/*	DMAģʽ��ADCֵ�ݴ���				*/
	s32 	errorD[3];											/*	���������ߺ������FOR PID			*/
	s32 	valueP, valueI, valueD, deltaPID;					/*	����������PID����������			*/
	s32 	motorPID;											/*	����������PID���������			*/
	s32 	lateralError;										/*	���������ߺ������-ʵʱ				*/
	float	leftDis,rightDis;									/*	������������ҵ���ľ���			*/
};
typedef struct __IRSensor IRSENSOR;								/*	�������ṹ��motor					*/

																/*	CrosswayStatus[3]����λ���ܽ���		**
																**	bit0:������ǽ ������±�־			**
																**	bit1:�Ҳ���ǽ->��ǽ ������±�־	**
																**	bit3:�����ǽ->��ǽ ������±�־	**
																**	bit4:������ǽ ɲ����־λ			**
																**	bit5:�Ҳ���·�� �жϱ�־λ			**
																**	bit7:�����·�� �жϱ�־λ			*/

/*********************************************************************************************************
**	ȫ�ֱ�������
*********************************************************************************************************/
extern IRSENSOR IRSensor;

/*********************************************************************************************************
**	�����⺯������
*********************************************************************************************************/
void SensorIRInit(void);
void SensorDMAInit(void);
void IRCheck(void);
void IRSensor_DisCorrection(void);
void IRSensor_ClearCache(void);
void IRSensor_Constructor(void);
