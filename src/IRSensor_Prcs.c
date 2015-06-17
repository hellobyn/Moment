
/*********************************************************************************************************
** 
**                               			������ͨ��ѧ                                     
**
**--------------------------------------------------------------------------------------------------------
** 	�ļ�����		SensorIR_Prcs.c
** 	����ʱ�䣺		2013-7-16 10:15
** 	������Ա�� 		�Ա���
** 	�ļ�����:  		������Դ�ļ�
** 
**--------------------------------------------------------------------------------------------------------
** 	����޸�ʱ�䣺	2014-4-28 10:15 
** 	����޸���Ա��	�Ա���	
** 	�汾�ţ� 	   	V1.0
** 	�汾������ 		������ADC��ϴ���ͨ��
**
*********************************************************************************************************/

#include "Motor.h"
//#include "IRSensor.h"
#include "IRCheck.h"
#include "Maze.h"
#include "stm32f10x_conf.h"

/*********************************************************************************************************
**	��������
*********************************************************************************************************/

IRSENSOR IRSensor;

/*********************************************************************************************************
**	����ԭ��:	void IRSensorConstructor(void)
**	��������:	IRSensor�ṹ���ʼ��
**	�������:	��
**	�������:	��
**	���ز���:	��
**	�޸�ʱ��:	2014-5-3	15:49
*********************************************************************************************************/
void IRSensor_Constructor(void)
{
	u8 i;
	IRSensor.IsCorrectionAllowed = FALSE;
	for (i=0;i<3;i++)
		IRSensor.CrosswayStatus[i] = 0x04;
	
	for (i=0;i<4;i++)
	{
		IRSensor.ADCValue[i] = 0;
		IRSensor.ADCCache[i] = 0;
	}
	
	for (i=0;i<3;i++)
		IRSensor.errorD[i] = 0;
	IRSensor.valueP = 0;
	IRSensor.valueI = 0;
	IRSensor.valueD = 0;
	IRSensor.deltaPID = 0;
	IRSensor.lateralError = 0;
	IRSensor.motorPID = 0;
	
	IRSensor.leftDis = 0;
	IRSensor.rightDis = 0;
}

/*********************************************************************************************************
**	����ԭ��:	void SensorIRInit(void)
**	��������:	�������ʼ��
**	�������:	��
**	�������:	��
**	���ز���:	��
**	�޸�ʱ��:	2013-7-15	15:00
*********************************************************************************************************/
void SensorIRInit(void)
{
	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;									
																			 								
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE );	  				/*	ADC1ʱ��ʹ��			*/
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);										/*	����ADCʱ��Ϊ8M			*/
	
	GPIO_InitStructure.GPIO_Pin = IR13Ctrl;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;						/*	ADC13���ƹܽ� �������	*/
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = IR24Ctrl;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;						/*	ADC24���ƹܽ� �������	*/
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	                         
	GPIO_InitStructure.GPIO_Pin = IR1 | IR2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;							/*	ADC�ܽ� ģ������		*/
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = IR3 | IR4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;							/*	ADC�ܽ� ģ������		*/
	GPIO_Init(GPIOB, &GPIO_InitStructure);	

	ADC_DeInit(ADC1);  
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;						/*	˫ADC ����ģʽ			*/
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;							/*	��ͨ��ת��ģʽ			*/
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;						/*	����ת��ģʽ			*/
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;		/*	�������ģʽ			*/
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;					/*	ADC�����Ҷ���			*/
	ADC_InitStructure.ADC_NbrOfChannel = 4;									/*	����ת����ͨ����Ŀ		*/
	ADC_Init(ADC1, &ADC_InitStructure);
	
	ADC_RegularChannelConfig(ADC1, ADC_1, 1, ADC_SampleTime_71Cycles5 );	/*	First  ����ʱ��13.5����	*/
	ADC_RegularChannelConfig(ADC1, ADC_2, 2, ADC_SampleTime_71Cycles5 );	/*	Second ����ʱ��13.5����	*/
	ADC_RegularChannelConfig(ADC1, ADC_3, 3, ADC_SampleTime_71Cycles5 );	/*	Third  ����ʱ��13.5����	*/
	ADC_RegularChannelConfig(ADC1, ADC_4, 4, ADC_SampleTime_71Cycles5 );	/*	Fourth ����ʱ��13.5����	*/									   

	ADC_DMACmd(ADC1, ENABLE);												/*	ʹ��ADC��DMA����		*/
	ADC_Cmd(ADC1, ENABLE);													/*	ʹ��ADC1				*/
	ADC_ResetCalibration(ADC1);												/*	����ADC1��У׼�Ĵ���	*/
	while(ADC_GetResetCalibrationStatus(ADC1));								/*	�ȴ�У׼�Ĵ����������	*/
	ADC_StartCalibration(ADC1);												/*	��ʼADC1У׼			*/	
	while(ADC_GetCalibrationStatus(ADC1));									/*	�ȴ�У׼���			*/
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);									/*	���ʹ��ADC1ת������	*/
	IR13Off;																/*	��ʼ���رպ��ⷢ��		*/
	IR24Off;																/*	��ʼ���رպ��ⷢ��		*/
	SensorDMAInit(); 														/*	��ʼ��ADC��DMA����		*/
	IRSensor_Constructor();
}

/*********************************************************************************************************
**	����ԭ��:	void SensorDMAInit(void)
**	��������:	������DMA�����ʼ��
**	�������:	��
**	�������:	ADCת��ֵ
**	���ز���:	��
**	�޸�ʱ��:	2013-7-19	10:00
*********************************************************************************************************/
void SensorDMAInit(void)
{														 					
	DMA_InitTypeDef DMA_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);						/*	DMA1ʱ��ʹ��			*/

	DMA_DeInit(DMA1_Channel1);  											/*	ȱʡ��ʼ��DMA1��ͨ��1	*/
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;  			/*	DMA�������ַ			*/	
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)& IRSensor.ADCCache;  		/*	DMA�ڴ����ַ			*/	
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  					/*	������Ϊ���ݴ������Դ	*/	
	DMA_InitStructure.DMA_BufferSize = 4;  									/*	DMA�����С				*/	
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  		/*	�����ַ�Ĵ�������		*/	
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  				/*	�ڴ��ַ�Ĵ�������		*/	
	DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord;/*	�������ݿ��Ϊ16λ		*/	
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; 	/* 	�ڴ����ݿ��Ϊ16λ		*/	
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;  						/* 	CAN������ѭ������ģʽ	*/	
	DMA_InitStructure.DMA_Priority = DMA_Priority_High; 					/* 	DMA1��ͨ��1ӵ�и����ȼ�	*/	
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  							/* 	ʧ��DMA���ڴ浽�ڴ洫��	*/	
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);  							/* 	��ʼ��DMA1��ͨ��1		*/	

	DMA_Cmd(DMA1_Channel1, ENABLE);											/* 	ʹ��DMA1��ͨ��1			*/
}

/*********************************************************************************************************
**	����ԭ��:	void IRSensor_ClearCache(void)
**	��������:	�������в������
**	�������:	��
**	�������:	ADCת��ֵ
**	���ز���:	��
**	�޸�ʱ��:	2013-7-19	10:00
*********************************************************************************************************/
void IRSensor_ClearCache(void)
{
	u8 i;
	for(i=0;i<3;i++)
		IRSensor.errorD[i] = 0;
	IRSensor.deltaPID = 0;
	IRSensor.motorPID = 0;
	IRSensor.lateralError = 0;
}

/*********************************************************************************************************
**	����ԭ��:	void IRSensor_DisCorrection(void)
**	��������:	���ú����ߴ����������������λ��У��
**	�������:	��
**	�������:	��
**	���ز���:	��
**	�޸�ʱ��:	2014-4-21 14:10 
*********************************************************************************************************/
void IRSensor_DisCorrection(void)
{
	if((IRSensor.ADCValue[1] >= 150) && (IRSensor.ADCValue[2] >= 150))		/*	������Ҷ���ǽ			*/
		IRSensor.lateralError = (IRSensor.leftDis - IRSensor.rightDis) / 2;	/*	������������Ҿ������	*/
	else if(IRSensor.ADCValue[1] >= 150)									/*	���ֻ�������ǽ		*/
		IRSensor.lateralError = IRSensor.leftDis - 84;						/*	�������ֻ��������	*/
	else if(IRSensor.ADCValue[2] >= 150)									/*	���ֻ���ұ���ǽ		*/
		IRSensor.lateralError = 84 - IRSensor.rightDis;						/*	�������ֻ���Ҿ����	*/
	
	if((IRSensor.lateralError > -3) && (IRSensor.lateralError < 3))			/*	������С��������3mm	*/
		IRSensor.lateralError = 0;
	
	if(IRSensor.lateralError > 30) IRSensor.lateralError = 30;				/*	��������������30mm	*/
	if(IRSensor.lateralError < -30) IRSensor.lateralError = -30;
		
	IRSensor.errorD[2] = IRSensor.errorD[1];								/*	e(k-2)��ֵ				*/
	IRSensor.errorD[1] = IRSensor.errorD[0];								/*	e(k-1)��ֵ				*/
	IRSensor.errorD[0] = IRSensor.lateralError;								/*	e(k)��ֵ				*/
			
	IRSensor.valueP = KpIR *( IRSensor.errorD[0] - IRSensor.errorD[1] );	/*	�������Ӳ��ּ���		*/
	IRSensor.valueI = KiIR *  IRSensor.errorD[0];							/*	�������Ӳ��ּ���		*/
	IRSensor.valueD = KdIR *( IRSensor.errorD[0] - 2 * IRSensor.errorD[1] + IRSensor.errorD[2] );	
																			/*	΢�����Ӳ��ּ���		*/
	IRSensor.deltaPID = ( IRSensor.valueP + IRSensor.valueI +IRSensor. valueD )/ 10;
																			/*	PID����ֵ����			*/
	IRSensor.motorPID += IRSensor.deltaPID;									/*	PID������				*/

	leftMotor.targetD -= IRSensor.motorPID;
	rightMotor.targetD += IRSensor.motorPID;
}

/*********************************************************************************************************
**	����ԭ��:	void IRCheck(void)
**	��������:	��������ƺ���
**	�������:	��
**	�������:	��
**	���ز���:	��
**	�޸�ʱ��:	2014-4-28	10:00
*********************************************************************************************************/
void IRCheck(void)
{														 					
	static u8 statusIR = 0;													/*	����������־λ		*/
	static u8 integer = 0;													/*	��������-����			*/
	static float decimal = 0;												/*	��������-С��			*/
	
	switch(statusIR)														/*	�жϼ�����			*/
	{
		case 0:																
			IR13On;															/*	��13�ź��ⷢ���		*/
			IRSensor.CrosswayStatus[0] = 0x11;								
			break;
		
		case 2:
			IRSensor.ADCValue[0] = IRSensor.ADCCache[0];					/*	ȡ1�ź�����չ�����		*/
			IRSensor.ADCValue[2] = IRSensor.ADCCache[2];					/*	ȡ3�ź�����չ�����		*/
			IR13Off;
			IR24On;
			if (IRSensor.ADCValue[2] <= 100) IRSensor.CrosswayStatus[0] |= 0x02;
			if (IRSensor.ADCValue[2] <= 200) IRSensor.CrosswayStatus[0] |= 0x20;
			if(IRSensor.ADCValue[2] < 150)									/*	����������ǽʱ			*/
				IRSensor.rightDis = IRSensor.leftDis;						/*	�����������ƽ���		*/
			else
			{
				integer = (IRSensor.ADCValue[2] - 150) / 14;				/*	��ѹ-�����������		*/
				decimal = ((IRSensor.ADCValue[2] - 150) % 14) / 14;			/*	�ڴ������е����Ա���	*/
				IRSensor.rightDis = (disCheck3[integer] - disCheck3[integer + 1])
									* decimal + disCheck3[integer + 1];		/*	���������֮������ȡֵ	*/
			}
			break;
			
		case 4:
			IRSensor.ADCValue[1] = IRSensor.ADCCache[1];					/*	ȡ2�ź�����չ�����		*/
			IRSensor.ADCValue[3] = IRSensor.ADCCache[3];					/*	ȡ4�ź�����չ�����		*/
			IR24Off;
			if (IRSensor.ADCValue[1] <= 100) IRSensor.CrosswayStatus[0] |= 0x08;
			if (IRSensor.ADCValue[1] <= 200) IRSensor.CrosswayStatus[0] |= 0x80;
			if (IRSensor.ADCValue[1] < 150)									/*	����������ǽʱ			*/
				IRSensor.leftDis = IRSensor.rightDis;						/*	�����������ƽ���		*/
			else
			{
				integer = (IRSensor.ADCValue[1] - 150) / 14;				/*	��ѹ-�����������		*/
				decimal = ((IRSensor.ADCValue[1] - 150) % 14) / 14;			/*	�ڴ������е����Ա���	*/
				IRSensor.leftDis = (disCheck2[integer] - disCheck2[integer + 1])
						  * decimal + disCheck2[integer + 1];				/*	���������֮������ȡֵ	*/
			}
			break;
			
		case 5:
			if((IRSensor.ADCValue[0] >= 200) || (IRSensor.ADCValue[3] >= 200)) //250,250
				 IRSensor.CrosswayStatus[0] &= 0xFE;
			if((IRSensor.ADCValue[0] >= 680) || (IRSensor.ADCValue[3] >= 760)) //400,430
				 IRSensor.CrosswayStatus[0] &= 0xEF;
			IRSensor.CrosswayStatus[2] = IRSensor.CrosswayStatus[1];	
			IRSensor.CrosswayStatus[1] = IRSensor.CrosswayStatus[0];
			if (IRSensor.IsCorrectionAllowed)								/*	�������������		*/
				IRSensor_DisCorrection();									/*	ִ�к����������		*/
			else IRSensor_ClearCache();										/*	������պ������в���	*/
			break;
			
		default:
			break;			
	}
	statusIR =( statusIR + 1 )% 10;											/*	����������5ms	(10*0.5)*/
}
