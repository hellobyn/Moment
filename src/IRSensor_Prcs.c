
/*********************************************************************************************************
** 
**                               			北京交通大学                                     
**
**--------------------------------------------------------------------------------------------------------
** 	文件名：		SensorIR_Prcs.c
** 	创建时间：		2013-7-16 10:15
** 	创建人员： 		赵秉贤
** 	文件描述:  		红外检测源文件
** 
**--------------------------------------------------------------------------------------------------------
** 	最后修改时间：	2014-4-28 10:15 
** 	最后修改人员：	赵秉贤	
** 	版本号： 	   	V1.0
** 	版本描述： 		红外检测ADC配合串口通信
**
*********************************************************************************************************/

#include "Motor.h"
//#include "IRSensor.h"
#include "IRCheck.h"
#include "Maze.h"
#include "stm32f10x_conf.h"

/*********************************************************************************************************
**	变量定义
*********************************************************************************************************/

IRSENSOR IRSensor;

/*********************************************************************************************************
**	函数原型:	void IRSensorConstructor(void)
**	函数功能:	IRSensor结构体初始化
**	输入参数:	无
**	输出参数:	无
**	返回参数:	无
**	修改时间:	2014-5-3	15:49
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
**	函数原型:	void SensorIRInit(void)
**	函数功能:	红外检测初始化
**	输入参数:	无
**	输出参数:	无
**	返回参数:	无
**	修改时间:	2013-7-15	15:00
*********************************************************************************************************/
void SensorIRInit(void)
{
	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;									
																			 								
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE );	  				/*	ADC1时钟使能			*/
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);										/*	设置ADC时钟为8M			*/
	
	GPIO_InitStructure.GPIO_Pin = IR13Ctrl;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;						/*	ADC13控制管脚 推免输出	*/
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = IR24Ctrl;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;						/*	ADC24控制管脚 推免输出	*/
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	                         
	GPIO_InitStructure.GPIO_Pin = IR1 | IR2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;							/*	ADC管脚 模拟输入		*/
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = IR3 | IR4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;							/*	ADC管脚 模拟输入		*/
	GPIO_Init(GPIOB, &GPIO_InitStructure);	

	ADC_DeInit(ADC1);  
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;						/*	双ADC 独立模式			*/
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;							/*	多通道转换模式			*/
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;						/*	连续转换模式			*/
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;		/*	软件触发模式			*/
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;					/*	ADC数据右对齐			*/
	ADC_InitStructure.ADC_NbrOfChannel = 4;									/*	规则转换的通道数目		*/
	ADC_Init(ADC1, &ADC_InitStructure);
	
	ADC_RegularChannelConfig(ADC1, ADC_1, 1, ADC_SampleTime_71Cycles5 );	/*	First  采样时间13.5周期	*/
	ADC_RegularChannelConfig(ADC1, ADC_2, 2, ADC_SampleTime_71Cycles5 );	/*	Second 采样时间13.5周期	*/
	ADC_RegularChannelConfig(ADC1, ADC_3, 3, ADC_SampleTime_71Cycles5 );	/*	Third  采样时间13.5周期	*/
	ADC_RegularChannelConfig(ADC1, ADC_4, 4, ADC_SampleTime_71Cycles5 );	/*	Fourth 采样时间13.5周期	*/									   

	ADC_DMACmd(ADC1, ENABLE);												/*	使能ADC的DMA请求		*/
	ADC_Cmd(ADC1, ENABLE);													/*	使能ADC1				*/
	ADC_ResetCalibration(ADC1);												/*	重置ADC1的校准寄存器	*/
	while(ADC_GetResetCalibrationStatus(ADC1));								/*	等待校准寄存器设置完成	*/
	ADC_StartCalibration(ADC1);												/*	开始ADC1校准			*/	
	while(ADC_GetCalibrationStatus(ADC1));									/*	等待校准完毕			*/
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);									/*	软件使能ADC1转换功能	*/
	IR13Off;																/*	初始化关闭红外发射		*/
	IR24Off;																/*	初始化关闭红外发射		*/
	SensorDMAInit(); 														/*	初始化ADC的DMA功能		*/
	IRSensor_Constructor();
}

/*********************************************************************************************************
**	函数原型:	void SensorDMAInit(void)
**	函数功能:	红外检测DMA传输初始化
**	输入参数:	无
**	输出参数:	ADC转换值
**	返回参数:	无
**	修改时间:	2013-7-19	10:00
*********************************************************************************************************/
void SensorDMAInit(void)
{														 					
	DMA_InitTypeDef DMA_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);						/*	DMA1时钟使能			*/

	DMA_DeInit(DMA1_Channel1);  											/*	缺省初始化DMA1的通道1	*/
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;  			/*	DMA外设基地址			*/	
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)& IRSensor.ADCCache;  		/*	DMA内存基地址			*/	
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  					/*	外设作为数据传输的来源	*/	
	DMA_InitStructure.DMA_BufferSize = 4;  									/*	DMA缓存大小				*/	
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  		/*	外设地址寄存器不变		*/	
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  				/*	内存基址寄存器递增		*/	
	DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord;/*	外设数据宽度为16位		*/	
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; 	/* 	内存数据宽度为16位		*/	
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;  						/* 	CAN工作在循环缓存模式	*/	
	DMA_InitStructure.DMA_Priority = DMA_Priority_High; 					/* 	DMA1的通道1拥有高优先级	*/	
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  							/* 	失能DMA的内存到内存传输	*/	
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);  							/* 	初始化DMA1的通道1		*/	

	DMA_Cmd(DMA1_Channel1, ENABLE);											/* 	使能DMA1的通道1			*/
}

/*********************************************************************************************************
**	函数原型:	void IRSensor_ClearCache(void)
**	函数功能:	红外运行参数清空
**	输入参数:	无
**	输出参数:	ADC转换值
**	返回参数:	无
**	修改时间:	2013-7-19	10:00
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
**	函数原型:	void IRSensor_DisCorrection(void)
**	函数功能:	利用红外线传感器做电脑鼠横向位移校正
**	输入参数:	无
**	输出参数:	无
**	返回参数:	无
**	修改时间:	2014-4-21 14:10 
*********************************************************************************************************/
void IRSensor_DisCorrection(void)
{
	if((IRSensor.ADCValue[1] >= 150) && (IRSensor.ADCValue[2] >= 150))		/*	如果左右都有墙			*/
		IRSensor.lateralError = (IRSensor.leftDis - IRSensor.rightDis) / 2;	/*	横向误差由左右距离计算	*/
	else if(IRSensor.ADCValue[1] >= 150)									/*	如果只有左边有墙		*/
		IRSensor.lateralError = IRSensor.leftDis - 84;						/*	横向误差只由左距计算	*/
	else if(IRSensor.ADCValue[2] >= 150)									/*	如果只有右边有墙		*/
		IRSensor.lateralError = 84 - IRSensor.rightDis;						/*	横向误差只由右距计算	*/
	
	if((IRSensor.lateralError > -3) && (IRSensor.lateralError < 3))			/*	设立最小调整误差±3mm	*/
		IRSensor.lateralError = 0;
	
	if(IRSensor.lateralError > 30) IRSensor.lateralError = 30;				/*	设立最大调整误差±30mm	*/
	if(IRSensor.lateralError < -30) IRSensor.lateralError = -30;
		
	IRSensor.errorD[2] = IRSensor.errorD[1];								/*	e(k-2)赋值				*/
	IRSensor.errorD[1] = IRSensor.errorD[0];								/*	e(k-1)赋值				*/
	IRSensor.errorD[0] = IRSensor.lateralError;								/*	e(k)赋值				*/
			
	IRSensor.valueP = KpIR *( IRSensor.errorD[0] - IRSensor.errorD[1] );	/*	比例算子部分计算		*/
	IRSensor.valueI = KiIR *  IRSensor.errorD[0];							/*	积分算子部分计算		*/
	IRSensor.valueD = KdIR *( IRSensor.errorD[0] - 2 * IRSensor.errorD[1] + IRSensor.errorD[2] );	
																			/*	微分算子部分计算		*/
	IRSensor.deltaPID = ( IRSensor.valueP + IRSensor.valueI +IRSensor. valueD )/ 10;
																			/*	PID增量值计算			*/
	IRSensor.motorPID += IRSensor.deltaPID;									/*	PID输出结果				*/

	leftMotor.targetD -= IRSensor.motorPID;
	rightMotor.targetD += IRSensor.motorPID;
}

/*********************************************************************************************************
**	函数原型:	void IRCheck(void)
**	函数功能:	红外检测控制函数
**	输入参数:	无
**	输出参数:	无
**	返回参数:	无
**	修改时间:	2014-4-28	10:00
*********************************************************************************************************/
void IRCheck(void)
{														 					
	static u8 statusIR = 0;													/*	红外次序检测标志位		*/
	static u8 integer = 0;													/*	红外距离表-整数			*/
	static float decimal = 0;												/*	红外距离表-小数			*/
	
	switch(statusIR)														/*	判断检测次序			*/
	{
		case 0:																
			IR13On;															/*	开13号红外发射管		*/
			IRSensor.CrosswayStatus[0] = 0x11;								
			break;
		
		case 2:
			IRSensor.ADCValue[0] = IRSensor.ADCCache[0];					/*	取1号红外接收管数据		*/
			IRSensor.ADCValue[2] = IRSensor.ADCCache[2];					/*	取3号红外接收管数据		*/
			IR13Off;
			IR24On;
			if (IRSensor.ADCValue[2] <= 100) IRSensor.CrosswayStatus[0] |= 0x02;
			if (IRSensor.ADCValue[2] <= 200) IRSensor.CrosswayStatus[0] |= 0x20;
			if(IRSensor.ADCValue[2] < 150)									/*	左右两侧无墙时			*/
				IRSensor.rightDis = IRSensor.leftDis;						/*	不做横向姿势矫正		*/
			else
			{
				integer = (IRSensor.ADCValue[2] - 150) / 14;				/*	电压-距离表的区间段		*/
				decimal = ((IRSensor.ADCValue[2] - 150) % 14) / 14;			/*	在此区间中的线性比例	*/
				IRSensor.rightDis = (disCheck3[integer] - disCheck3[integer + 1])
									* decimal + disCheck3[integer + 1];		/*	曲线区间点之间线性取值	*/
			}
			break;
			
		case 4:
			IRSensor.ADCValue[1] = IRSensor.ADCCache[1];					/*	取2号红外接收管数据		*/
			IRSensor.ADCValue[3] = IRSensor.ADCCache[3];					/*	取4号红外接收管数据		*/
			IR24Off;
			if (IRSensor.ADCValue[1] <= 100) IRSensor.CrosswayStatus[0] |= 0x08;
			if (IRSensor.ADCValue[1] <= 200) IRSensor.CrosswayStatus[0] |= 0x80;
			if (IRSensor.ADCValue[1] < 150)									/*	左右两侧无墙时			*/
				IRSensor.leftDis = IRSensor.rightDis;						/*	不做横向姿势矫正		*/
			else
			{
				integer = (IRSensor.ADCValue[1] - 150) / 14;				/*	电压-距离表的区间段		*/
				decimal = ((IRSensor.ADCValue[1] - 150) % 14) / 14;			/*	在此区间中的线性比例	*/
				IRSensor.leftDis = (disCheck2[integer] - disCheck2[integer + 1])
						  * decimal + disCheck2[integer + 1];				/*	曲线区间点之间线性取值	*/
			}
			break;
			
		case 5:
			if((IRSensor.ADCValue[0] >= 200) || (IRSensor.ADCValue[3] >= 200)) //250,250
				 IRSensor.CrosswayStatus[0] &= 0xFE;
			if((IRSensor.ADCValue[0] >= 680) || (IRSensor.ADCValue[3] >= 760)) //400,430
				 IRSensor.CrosswayStatus[0] &= 0xEF;
			IRSensor.CrosswayStatus[2] = IRSensor.CrosswayStatus[1];	
			IRSensor.CrosswayStatus[1] = IRSensor.CrosswayStatus[0];
			if (IRSensor.IsCorrectionAllowed)								/*	如果允许红外矫正		*/
				IRSensor_DisCorrection();									/*	执行红外矫正函数		*/
			else IRSensor_ClearCache();										/*	否则清空红外运行参数	*/
			break;
			
		default:
			break;			
	}
	statusIR =( statusIR + 1 )% 10;											/*	红外检测周期5ms	(10*0.5)*/
}
