
/*********************************************************************************************************
** 
**    										    北京交通大学                                     
**
**--------------------------------------------------------------------------------------------------------
** 	文件名：		Maze.c
** 	创建时间：		2013-7-9 17：20
** 	创建人员： 		赵秉贤
** 	文件描述:  		主程序文件 迷宫算法
** 
**--------------------------------------------------------------------------------------------------------
** 	最后修改时间：	2014-1-7 10:15 
** 	最后修改人员：	赵秉贤	
** 	版本号： 	   	V1.1
** 	版本描述： 		Moment 0_2 电机测试
**
*********************************************************************************************************/

#include "stm32f10x.h"
#include "Motor.h"
#include "Usart.h"
#include "Delay.h"
#include "Interface.h"
#include "Maze.h"


/*********************************************************************************************************
**	变量定义
*********************************************************************************************************/

s32 				dataCache[1000][12]	=	{0};							/*	数据缓存器				*/
u16 				numGroup 			= 	0;								/*	串口发送时序			*/
MICROMOUSE 			mouse				=	{0,0,0,0,0};					/*	电脑鼠状态存储			*/
static MAZECOOR		mouseCoor			= 	{0,0};							/*	电脑鼠坐标储存			*/
static MAZECOOR		crossWay[256]		= 	{0,0};							/*	储存有未走支路的坐标	*/
static u8			mapBlock[16][16] 	= 	{0};							/*	地图信息储存			*/
static u8			mapStep[16][16] 	= 	{0};							/*	坐标点等高值储存		*/
//static u8			mazeStartX			=	0;								/*	迷宫起点横坐标			*/
//static u8			mazeStartY			=	0;								/*	迷宫起点纵坐标			*/
u8 data[4];

/*********************************************************************************************************
**	主函数
*********************************************************************************************************/ 

int main(void)
{	
	char dataUsart[40];													/*	串口发送暂存器			*/
	u16 usartGroup;
	u8 tempStartY;
	u8 numCrossWay;
	u8 unCoverRoad;
	
	ClockInit();		  												/*	系统时钟初始化			*/
	DelayInit(72);														/*	延时函数初始化			*/
	UsartInit();														/*	串口通信初始化			*/
	MotorInit();												   		/*	电机驱动初始化			*/
	SensorIRInit();												   		/*	红外检测初始化			*/
	KeyInit();															/*	按键检测初始化			*/
	InterfaceInit();													/*	交互模块初始化			*/

	while(mouse.state != 4)			//mouse.state != SPURT
	{	
		switch(mouse.state)
		{
			case WAIT:
				Delayms(20);
				if(KeyCheck() == TRUE)
				{
					blueLEDBlink;	
					MotorENCInit();					
					Timer2IRQInit();					
					while(1)
					{
						if((IRSensor.ADCValue[2] >= 1500) && (IRSensor.ADCValue[3] >= 1500))
						{
							Delayms(50);
							if((IRSensor.ADCValue[2] >= 1500) && (IRSensor.ADCValue[3] >= 1500))
							{
								greenLEDBlink;
								Delayms(1000);
								break;
							}
						}
					}			
					mouse.state = START;
					WallCheck();
					MouseStart();
				}
				break;
				
			case START:	
				SearchGoAhead();				
				if(mapBlock[mouseCoor.X][mouseCoor.Y] & 0x08)
				{
//					mazeStartX = 15;
					mouseCoor.X = 15;
		
					tempStartY = mouseCoor.Y;
					do
					{
						mapBlock[15][tempStartY] = mapBlock[0][tempStartY];
						mapBlock[0][tempStartY] = 0;
					}
					while(tempStartY--);
					
					crossWay[numCrossWay].X = 15;
					crossWay[numCrossWay].Y = 0;
					numCrossWay++;
				}
				if(mapBlock[mouseCoor.X][mouseCoor.Y] & 0x02)
				{
					crossWay[numCrossWay].X = 0;
					crossWay[numCrossWay].Y = 0;
					numCrossWay++;
				}
				mouse.state = MAZESEARCH;
				break;
				
			case MAZESEARCH:
				unCoverRoad = CrossWayCheck(mouseCoor.X,mouseCoor.Y);
//				redLedOff;
//				greenLedOff;
//				blueLedOff;
//				if(unCoverRoad == 0)
//					redLedOn;
//				if(unCoverRoad == 1)
//					blueLedOn;
//				if(unCoverRoad == 2)
//					greenLedOn;	
//				if(unCoverRoad == 3)
//				{
//					redLedOn;
//					blueLedOn;
//					greenLedOn;
//				}					
				
				if(unCoverRoad)
				{
					if(unCoverRoad > 1)
					{
						crossWay[numCrossWay].X = mouseCoor.X;
						crossWay[numCrossWay].Y = mouseCoor.Y;
						numCrossWay++;
					}
					CrossWayChoice();
					SearchGoAhead();
				}
				else
				{
					SearchBreak();
//					if((mouseCoor.X == 7) && (mouseCoor.Y == 0))
//						redLedOn;
					MouseTurnBack();
					while(--numCrossWay)
					{
						unCoverRoad = CrossWayCheck(crossWay[numCrossWay].X,
													crossWay[numCrossWay].Y);
						if(unCoverRoad)
						{
//							if((crossWay[numCrossWay].X == 4) && (crossWay[numCrossWay].Y == 4))
//								blueLedOn;
							
							SpurtGoTo(crossWay[numCrossWay].X,
									  crossWay[numCrossWay].Y);
									  
							if(unCoverRoad > 1)
								numCrossWay++;
							CrossWayChoice();
							SearchGoAhead();
							break;
						}
					}					
					if(numCrossWay == 0)
					{					
						SpurtGoTo(crossWay[0].X,
								  crossWay[0].Y);
						while (IRSensor.CrosswayStatus[1] & 0x10);
						SearchBreak();
						MouseTurnBack();
						mouse.state = SPURT;
					}
				}		
				break;
			case SPURT:
				SpurtGoTo(7,7);
				SearchBreak();
				MouseTurnBack();
				
				SpurtGoTo(0,0);
				while (IRSensor.CrosswayStatus[1] & 0x10);
				SearchBreak();
				MouseTurnBack();
				mouse.state = 4;
				break;
			default:
                break;
		}
	}
	
	while(1)
	{
		if(KeyCheck() == TRUE)
		{
			greenLEDBlink;
			TIM_Cmd(TIM2,DISABLE);
			LeftMotorDrv(0);
			RightMotorDrv(0);
			break;
		}
	}	
	while(1) 
	{
		if(KeyCheck() == TRUE)
		{	
			greenLedOn;
			Delayms(1000);
			greenLedOff;	
			for(usartGroup = 0; usartGroup <= numGroup; usartGroup++)
			{
				while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
				sprintf(dataUsart, "%d, %d, %d\n",
					dataCache[usartGroup][0], dataCache[usartGroup][1], dataCache[usartGroup][2]);
				printf(dataUsart);
			}
		}				
	}
}

/*********************************************************************************************************
**	函数原型:	void ClockInit(void)
**	函数功能:	系统时钟初始化
**	输入参数:	无
**	输出参数:	无
**	返回参数:	无
**	修改时间:	2013-7-15	11:15
*********************************************************************************************************/
void ClockInit(void)
{
	ErrorStatus HSEStartUpStatus;											/* 	定义错误状态变量		*/
	RCC_DeInit();															/*	RCC寄存器初始化为缺省值	*/
	RCC_HSEConfig(RCC_HSE_ON);												/*	使能外部高速时钟HSE		*/
	HSEStartUpStatus = RCC_WaitForHSEStartUp();							   	/*	等待外部晶振工作		*/
	if(HSEStartUpStatus == SUCCESS)											/*	如果外部晶振正常工作	*/
	{
		FLASH_SetLatency(FLASH_Latency_2);									/*	Flash存储器延时2周期	*/
		RCC_HCLKConfig(RCC_SYSCLK_Div1);									/*	AHB时钟 = 系统时钟		*/
		RCC_PCLK2Config(RCC_HCLK_Div1);										/*	高速AHB时钟 = 系统时钟	*/
		RCC_PCLK1Config(RCC_HCLK_Div2);										/*	低速AHB时钟 = 系统时钟/2*/
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);				/*	PLL时钟 = 系统时钟×9	*/
		RCC_PLLCmd(ENABLE);													/*	PLL时钟使能				*/
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
		{
		}																	
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);							/*	选择PLL为系统时钟		*/
		while(RCC_GetSYSCLKSource() != 0x08)								/*	等待系统时钟设置完成	*/
		{
		}
	}	
	RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOA|							/*	使能GPIOA时钟			*/
							RCC_APB2Periph_GPIOB|							/*	使能GPIOB时钟			*/
							RCC_APB2Periph_GPIOC|							/*	使能GPIOC时钟			*/
							RCC_APB2Periph_GPIOD|							/*	使能GPIOD时钟			*/
							RCC_APB2Periph_AFIO,							/*	使能AFIO时钟			*/
							ENABLE);
}

/*********************************************************************************************************
**	函数原型:	void KeyInit(void)
**	函数功能:	按键检测初始化
**	输入参数:	无
**	输出参数:	无
**	返回参数:	无
**	修改时间:	2014-1-7	11:15
*********************************************************************************************************/
void KeyInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = Key;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;							/*	设置管脚上拉输入		*/
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*********************************************************************************************************
**	函数原型:	u8 KeyCheck(void)
**	函数功能:	按键检测
**	输入参数:	无
**	输出参数:	无
**	返回参数:	TURE：	按键以按下
				FALSE：	按键未按下
**	修改时间:	2014-1-7	11:15
*********************************************************************************************************/
u8 KeyCheck(void)
{
	if(GPIO_ReadInputDataBit(GPIOA, Key) == 0)								/*	读取管脚电位状态		*/
	{
		Delayms(50);														/*	延时50ms				*/
		while(GPIO_ReadInputDataBit(GPIOA, Key) == 0);						/*	重新检测防止误判		*/
		return(TRUE);														/*	返回按键被按下			*/
	}
	else
	{
	 	return(FALSE);														/*	返回按键未按下			*/
	}		
}

/*********************************************************************************************************
**	函数原型:	s32 Abs(s32 absValue)
**	函数功能:	求参数的绝对值
**	输入参数:	待求参数
**	输出参数:	无
**	返回参数:	所求参数的绝对值
**	修改时间:	2014-2-20	15:55
*********************************************************************************************************/
s32 Abs(s32 absValue)
{
	if(absValue < 0)														/*	如果输入参数为负数		*/							
		return -absValue;													/*	输出它的相反数			*/			
	else 
		return absValue;
}

/*********************************************************************************************************
**	函数原型:	void DataCache(void)
**	函数功能:	运行数据缓存函数
**	输入参数:	无
**	输出参数:	无
**	返回参数:	无
**	修改时间:	2014-3-6	11:00
*********************************************************************************************************/
void DataCache(void)
{
	dataCache[numGroup][0] = mouseCoor.X;//ADCValue[0];
	dataCache[numGroup][1] = mouseCoor.Y;//ADCValue[1];
	dataCache[numGroup][2] = mapBlock[mouseCoor.X][mouseCoor.Y];
//	dataCache[numGroup][3] = data;
//	dataCache[numGroup][4] = leftMotor.currentD;//leftMotor.accel;
//	dataCache[numGroup][5] = rightMotor.currentD;//rightMotor.targetV;
//	dataCache[numGroup][6] = leftMotor.targetD;//rightMotor.currentD;
//	dataCache[numGroup][7] = rightMotor.targetD;//leftMotor.currentD;
//	dataCache[numGroup][8] = leftMotor.limitD;//leftMotor.targetD;
//	dataCache[numGroup][9] = rightMotor.limitD;//rightMotor.limitD;
//	dataCache[numGroup][10] = lPWM;
//	dataCache[numGroup][11] = rPWM;
	numGroup++;
}

/*********************************************************************************************************
**	函数原型:	void Timer2IRQInit(void)
**	函数功能:	定时中断初始化
**	输入参数:	无
**	输出参数:	无
**	返回参数:	无
**	修改时间:	2014-2-18	17:50
*********************************************************************************************************/
void Timer2IRQInit(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);							/*	3bit抢断位，1bit响应位	*/
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  						/*	选择TIM2全局中断		*/
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;				/*	设置抢断优先级为1		*/
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;						/*	设置响应优先级为0		*/	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;							/*	开中断					*/
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_DeInit(TIM2);														/*	定时器复位				*/
	TIM_TimeBaseStructure.TIM_Period=499;									/*	设定定时时长为1ms		*/
	TIM_TimeBaseStructure.TIM_Prescaler=35;									/*	定时器16分频 频率为1MHz	*/
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;					/*	时钟分割值为1			*/
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;				/*	向上计数模式			*/
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);							/*	初始化定时器的值		*/
	TIM_ClearFlag(TIM2,TIM_FLAG_Update);									/*	清除中断标志位			*/
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);								/*	使能中断				*/
	TIM_Cmd(TIM2,ENABLE);													/*	定能定时器				*/
}

/*********************************************************************************************************
**	函数原型:	void SearchGoAhead(void)
**	函数功能:	电脑鼠搜索前进
**	输入参数:	无
**	输出参数:	无
**	返回参数:	无
**	修改时间:	2014-4-28 14:30 
*********************************************************************************************************/
void SearchGoAhead(void)		//整体坐标更新逻辑需要进一步探讨
{
	while (1)
	{
		if ((!(IRSensor.CrosswayStatus[2] & 0x0A)) && (IRSensor.CrosswayStatus[1] & 0x0A))//校正坐标 有墙->无墙
		{					//当前逻辑仅在 左右两侧都有墙->任意一侧无墙壁时有效 是否修改有待商榷（逻辑性和适用性）
			mouse.coorSwt = 0;
			if (IRSensor.CrosswayStatus[1] & 0x08)
			{
				mouse.vertD = 63000;
			}
			else if (IRSensor.CrosswayStatus[1] & 0x02)
			{
				mouse.vertD = 75000;
			}
			IRSensor.IsCorrectionAllowed = FALSE;
		}
		if (mouse.vertD >= 145000)
		{		
			if ((IRSensor.CrosswayStatus[1] & 0x80)	&& (!mouse.coorSwt))			//第一次初步判断左边有出口	
			{																		//没有添加路口重复判断
				CoorUpdate();
				break;
			}
			else if ((IRSensor.CrosswayStatus[1] & 0x20) && (!mouse.coorSwt))		//第一次初步判断右边有出口	
			{																		//尚未添加重复判断
				CoorUpdate();
				break;
			}
			else if (!(IRSensor.CrosswayStatus[1] & 0x01))
			{																	//同样的问题 重复判断尚未添加
				CoorUpdate();
				while (IRSensor.CrosswayStatus[1] & 0x10);
				break;
			}
		}
		if(mouse.vertD >= 180000)
		{
			if(mouse.coorSwt != 3)											//防止直行路口坐标二次更新
				CoorUpdate();
			else
				mouse.coorSwt = 1;
			mouse.vertD -= 180000;
		}
	}
}

/*********************************************************************************************************
**	函数原型:	void CoorUpdate(void)
**	函数功能:	电脑鼠坐标更新函数
**	输入参数:	无
**	返回参数:	无
**	修改时间:	2014-4-4	11:30
*********************************************************************************************************/
void CoorUpdate(void)
{
	switch(mouse.dir)
	{
		case MAZEANGLE0:													/*	电脑鼠方向为0度			*/
			mouseCoor.Y++;													/*	纵坐标 + 1				*/
			break;

		case MAZEANGLE90:													/*	电脑鼠方向为90度		*/
			mouseCoor.X++;													/*	横坐标 + 1				*/
			break;

		case MAZEANGLE180:													/*	电脑鼠方向为180度		*/
			mouseCoor.Y--;													/*	纵坐标 - 1				*/
			break;

		case MAZEANGLE270:													/*	电脑鼠方向为270度		*/
			mouseCoor.X--;													/*	横坐标 - 1				*/	
			break;
		default:
			break;
	}
	mouse.coorSwt = 1;
	WallCheck();															/*	墙壁信息检测			*/
}

/*********************************************************************************************************
**	函数原型:	void WallCheck(void)
**	函数功能:	电脑鼠墙壁检测函数
**	输入参数:	无
**	返回参数:	无
**	修改时间:	2014-4-8	21:30
*********************************************************************************************************/
void WallCheck(void)
{
	u8 mazeBezel = 0;
	mazeBezel |= MOUSEWAY_B;
	
	if (IRSensor.CrosswayStatus[1] & 0x80)   								/*	左侧墙壁判断			*/
		mazeBezel |=  MOUSEWAY_L; 
	else 
		mazeBezel &= ~MOUSEWAY_L ;
	
	if (IRSensor.CrosswayStatus[1] & 0x01)									/*	前方墙壁判断			*/
		mazeBezel |=  MOUSEWAY_F;
	else 
		mazeBezel &= ~MOUSEWAY_F;

	if (IRSensor.CrosswayStatus[1] & 0x20) 									/*	右侧墙壁判断			*/
		mazeBezel |=  MOUSEWAY_R;
	else 
		mazeBezel &= ~MOUSEWAY_R;
		
	if (mapBlock[mouseCoor.X][mouseCoor.Y] == 0x00) 
    {
        mapBlock[mouseCoor.X][mouseCoor.Y] = mazeBezel;						/*	墙壁信息记录			*/
		DataCache();
    }
}

/*********************************************************************************************************
**	函数原型:	u8 CrossWayCheck(u8 mazeX, u8 mazeY)
**	函数功能:	统计某坐标点还未走过的支路数
**	输入参数:	mazeX:带检测点的横坐标
				mazeY:带检测点的纵坐标
**	返回参数:	unCoverRoad:未走过的支路数
**	修改时间:	2014-4-16	19:40
*********************************************************************************************************/
u8 CrossWayCheck(u8 mazeX, u8 mazeY)
{
	u8 unCoverRoad = 0;
	if((mapBlock[mazeX][mazeY] & 0x01) &&									/*	绝对方向 迷宫上方有路	*/
	   (mapBlock[mazeX][mazeY + 1]) == 0x00)								/*	绝对方向 迷宫上方未走过	*/
		unCoverRoad++;														/*	可前进方向数加1			*/
		
	if((mapBlock[mazeX][mazeY] & 0x02) &&									/*	绝对方向 迷宫右方有路	*/
	   (mapBlock[mazeX + 1][mazeY]) == 0x00)								/*	绝对方向 迷宫右方未走过	*/
		unCoverRoad++;														/*	可前进方向数加1			*/
		
	if((mapBlock[mazeX][mazeY] & 0x04) &&									/*	绝对方向 迷宫下方有路	*/
	   (mapBlock[mazeX][mazeY - 1]) == 0x00)								/*	绝对方向 迷宫下方未走过	*/
		unCoverRoad++;														/*	可前进方向数加1			*/
		
	if((mapBlock[mazeX][mazeY] & 0x08) &&									/*	绝对方向 迷宫左方有路	*/
	   (mapBlock[mazeX - 1][mazeY]) == 0x00)								/*	绝对方向 迷宫左方未走过	*/
		unCoverRoad++;														/*	可前进方向数加1			*/
	
	return unCoverRoad;														/*	返回可前进方向数		*/
}

/*********************************************************************************************************
**	函数原型:	void CrossWayChoice(void)
**	函数功能:	选择一条支路作为前进方向
**	输入参数:	无
**	返回参数:	无
**	修改时间:	2014-4-16	19:57
*********************************************************************************************************/
void CrossWayChoice(void)
{
	if((mapBlock[mouseCoor.X][mouseCoor.Y] & MOUSEWAY_R) &&					/*	电脑鼠的右边有路		*/
	   (MapBlockDataGet(MOUSERIGHT) == 0x00))								/*	电脑鼠右边没有走过		*/
	{
		SearchTurnRight();													/*	电脑鼠右转				*/
		return;
	}
	   
	if((mapBlock[mouseCoor.X][mouseCoor.Y] & MOUSEWAY_F) &&					/*	电脑鼠的前边有路		*/		
	   (MapBlockDataGet(MOUSEFRONT) == 0x00))								/*	电脑鼠前边没有走过	*/
	{
	   IRSensor.IsCorrectionAllowed = TRUE;
	   mouse.coorSwt = 3;
	   return;																/*	电脑鼠不用转弯			*/
	}
	
	if((mapBlock[mouseCoor.X][mouseCoor.Y] & MOUSEWAY_L) &&					/*	电脑鼠左转				*/
	   (MapBlockDataGet(MOUSELEFT) == 0x00))								/*	电脑鼠左边没有走过		*/
	{
		SearchTurnLeft();													/*	电脑鼠左转				*/
		return;
	}
//	if(mapBlock[mouseCoor.X][mouseCoor.Y] & MOUSEWAY_B)
//	{
//		SearchBreak();
//		MouseTurnBack();
//		return;
//	}

}

/*********************************************************************************************************
**	函数原型:	u8 MapBlockDataGet(u8 dirTemp)
**	函数功能:	根据电脑鼠的相对方向，取出该方向上迷宫格的墙壁资料
**	输入参数:	dirTemp：电脑鼠的相对方向
**	返回参数:	相对方向上的迷宫格墙壁信息
**	修改时间:	2014-4-16	20:10
*********************************************************************************************************/
u8 MapBlockDataGet(u8 dirTemp)
{
	u8 mazeX = 0,mazeY = 0;
	
	switch(dirTemp)
	{
		case MOUSEFRONT:
			dirTemp = mouse.dir;
			break;
			
		case MOUSERIGHT:
			dirTemp = (mouse.dir + 1) % 4;
			break;
			
		case MOUSELEFT:
			dirTemp = (mouse.dir + 3) % 4;
			break;
			
		default:
			break;
	}
	
	switch(dirTemp)
	{
		case 0:
			mazeX = mouseCoor.X;
			mazeY = mouseCoor.Y + 1;
			break;
			
		case 1:
			mazeX = mouseCoor.X + 1;
			mazeY = mouseCoor.Y;
			break;
			
		case 2:
			mazeX = mouseCoor.X;
			mazeY = mouseCoor.Y - 1;
			break;
			
		case 3:
			mazeX = mouseCoor.X - 1;
			mazeY = mouseCoor.Y;
			break;

		default:
			break;
	}	
	return(mapBlock[mazeX][mazeY]);
}

/*********************************************************************************************************
**	函数原型:	void MapStepEdit(u8 mazeX,u8 mazeY)
**	函数功能:	制作以目标点为起点的等高图
**	输入参数:	目标点的横纵坐标
**	输出参数:	各坐标点的等高值
**	返回参数:	无
**	修改时间:	2014-5-20	11:10
*********************************************************************************************************/
void MapStepEdit(u8 mazeX,u8 mazeY)
{
	MAZECOOR	crossWay[256]		= 	{0};						/*	等高图有未走支路的坐标	*/
	u8			dirStack[16][16]	= 	{0};						/*	储存坐标点的进出方向	*/
	u8 numCrossWay;
	u8 unCoverRoad;
	u8 blockStep = 1;
	u8 i,j;
	
	crossWay[numCrossWay].X = mazeX;
	crossWay[numCrossWay].Y = mazeY;
	numCrossWay++;
	
	for (i = 0; i < 16; i++)
	{
		for (j = 0; j < 16; j++)
			mapStep[i][j] = 0xFF;
	}
	
	while(numCrossWay)
	{
		mapStep[mazeX][mazeY] = blockStep++;
		
		unCoverRoad = 0;
		if(mapBlock[mazeX][mazeY] & 0x01) 
		{
			dirStack[mazeX][mazeY] &= 0xF0;
			dirStack[mazeX][mazeY] |= 0x01;
			if((dirStack[mazeX][mazeY] >> 4) != 
			   (dirStack[mazeX][mazeY] & 0x0F))
				blockStep++;
			if(mapStep[mazeX][mazeY + 1] > blockStep)
				unCoverRoad++;
			if((dirStack[mazeX][mazeY] >> 4) != 
			   (dirStack[mazeX][mazeY] & 0x0F))
				blockStep--;
		}
		
		if (mapBlock[mazeX][mazeY] & 0x02) 
		{
			dirStack[mazeX][mazeY] &= 0xF0;
			dirStack[mazeX][mazeY] |= 0x02;
			if((dirStack[mazeX][mazeY] >> 4) != 
			   (dirStack[mazeX][mazeY] & 0x0F))
				blockStep++;
			if(mapStep[mazeX + 1][mazeY] > blockStep)
				unCoverRoad++;
			if((dirStack[mazeX][mazeY] >> 4) != 
			   (dirStack[mazeX][mazeY] & 0x0F))
				blockStep--;
		}
		
		if (mapBlock[mazeX][mazeY] & 0x04) 
		{
			dirStack[mazeX][mazeY] &= 0xF0;
			dirStack[mazeX][mazeY] |= 0x04;
			if((dirStack[mazeX][mazeY] >> 4) != 
			   (dirStack[mazeX][mazeY] & 0x0F))
				blockStep++;
			if(mapStep[mazeX][mazeY - 1] > blockStep)
				unCoverRoad++;
			if((dirStack[mazeX][mazeY] >> 4) != 
			   (dirStack[mazeX][mazeY] & 0x0F))
				blockStep--;
		}
		
		if (mapBlock[mazeX][mazeY] & 0x08) 
		{
			dirStack[mazeX][mazeY] &= 0xF0;
			dirStack[mazeX][mazeY] |= 0x08;
			if((dirStack[mazeX][mazeY] >> 4) != 
			   (dirStack[mazeX][mazeY] & 0x0F))
				blockStep++;
			if(mapStep[mazeX - 1][mazeY] > blockStep)
				unCoverRoad++;
			if((dirStack[mazeX][mazeY] >> 4) != 
			   (dirStack[mazeX][mazeY] & 0x0F))				
				blockStep--;
		}
//		if ((mapBlock[mazeX][mazeY] & 0x01) &&                             
//			(mapStep[mazeX][mazeY + 1] > blockStep)) 
//			unCoverRoad++;
//        if ((mapBlock[mazeX][mazeY] & 0x02) &&
//            (mapStep[mazeX + 1][mazeY] > blockStep)) 
//            unCoverRoad++;
//        if ((mapBlock[mazeX][mazeY] & 0x04) &&
//            (mapStep[mazeX][mazeY - 1] > blockStep)) 
//            unCoverRoad++;
//        if ((mapBlock[mazeX][mazeY] & 0x08) &&
//            (mapStep[mazeX - 1][mazeY] > blockStep)) 
//            unCoverRoad++;

		if(unCoverRoad == 0)
		{
			numCrossWay--;
			mazeX = crossWay[numCrossWay].X;
			mazeY = crossWay[numCrossWay].Y;
			blockStep = mapStep[mazeX][mazeY];
		}
		else
		{
			if(unCoverRoad > 1)
			{
				crossWay[numCrossWay].X = mazeX;
				crossWay[numCrossWay].Y = mazeY;
				numCrossWay++;
			}
						
			if(mapBlock[mazeX][mazeY] & 0x01) 
			{
				dirStack[mazeX][mazeY] &= 0xF0;
				dirStack[mazeX][mazeY] |= 0x01;
				if((dirStack[mazeX][mazeY] >> 4) != 
				   (dirStack[mazeX][mazeY] & 0x0F))
					blockStep++;
				if(mapStep[mazeX][mazeY + 1] > blockStep)
				{				
					mazeY++;
					dirStack[mazeX][mazeY] = 0x10;
					continue;
				}
				if((dirStack[mazeX][mazeY] >> 4) != 
				   (dirStack[mazeX][mazeY] & 0x0F))
					blockStep--;
			}
			
			if (mapBlock[mazeX][mazeY] & 0x02) 
			{
				dirStack[mazeX][mazeY] &= 0xF0;
				dirStack[mazeX][mazeY] |= 0x02;
				if((dirStack[mazeX][mazeY] >> 4) != 
				   (dirStack[mazeX][mazeY] & 0x0F))
					blockStep++;
				if(mapStep[mazeX + 1][mazeY] > blockStep)
				{				
					mazeX++;
					dirStack[mazeX][mazeY] = 0x20;
					continue;
				}
				if((dirStack[mazeX][mazeY] >> 4) != 
				   (dirStack[mazeX][mazeY] & 0x0F))
					blockStep--;
			}
			
			if (mapBlock[mazeX][mazeY] & 0x04) 
			{
				dirStack[mazeX][mazeY] &= 0xF0;
				dirStack[mazeX][mazeY] |= 0x04;
				if((dirStack[mazeX][mazeY] >> 4) != 
				   (dirStack[mazeX][mazeY] & 0x0F))
					blockStep++;
				if(mapStep[mazeX][mazeY - 1] > blockStep)
				{				
					mazeY--;
					dirStack[mazeX][mazeY] = 0x40;
					continue;
				}
				if((dirStack[mazeX][mazeY] >> 4) != 
				   (dirStack[mazeX][mazeY] & 0x0F))
					blockStep--;
			}
			
			if (mapBlock[mazeX][mazeY] & 0x08) 
			{
				dirStack[mazeX][mazeY] &= 0xF0;
				dirStack[mazeX][mazeY] |= 0x08;
				if((dirStack[mazeX][mazeY] >> 4) != 
				   (dirStack[mazeX][mazeY] & 0x0F))
					blockStep++;
				if(mapStep[mazeX - 1][mazeY] > blockStep)
				{				
					mazeX--;
					dirStack[mazeX][mazeY] = 0x80;
					continue;
				}
				if((dirStack[mazeX][mazeY] >> 4) != 
				   (dirStack[mazeX][mazeY] & 0x0F))				
					blockStep--;
			}
//			if ((mapBlock[mazeX][mazeY] & 0x02) &&
//				(mapStep[mazeX + 1][mazeY] > blockStep)) 
//			{
//				dirStack[mazeX][mazeY] |= 0x02;
//				if((dirStack[mazeX][mazeY] >> 4) != 
//				   (dirStack[mazeX][mazeY] & 0x0F))
//					blockStep++;
//				mazeX++;
//				dirStack[mazeX][mazeY] = 0x20;
//				continue;
//			}
//			if ((mapBlock[mazeX][mazeY] & 0x04) &&
//				(mapStep[mazeX][mazeY - 1] > (blockStep))) 
//			{
//				dirStack[mazeX][mazeY] |= 0x04;
//				if((dirStack[mazeX][mazeY] >> 4) != 
//				   (dirStack[mazeX][mazeY] & 0x0F))
//					blockStep++;
//				mazeY--;
//				dirStack[mazeX][mazeY] = 0x40;
//				continue;
//			}
//			if ((mapBlock[mazeX][mazeY] & 0x08) &&
//				(mapStep[mazeX - 1][mazeY] > (blockStep))) 
//			{
//				dirStack[mazeX][mazeY] |= 0x08;
//				if((dirStack[mazeX][mazeY] >> 4) != 
//				   (dirStack[mazeX][mazeY] & 0x0F))
//					blockStep++;
//				mazeX--;
//				dirStack[mazeX][mazeY] = 0x80;
//				continue;
//			}
		}
	}	
}


/*********************************************************************************************************
**	函数原型:	void SpurtGoTo(u8 mazeX,u8 mazeY)
**	函数功能:	驱动电脑鼠直接运动到指定坐标格
**	输入参数:	目标点的横纵坐标
**	返回参数:	无�
**	修改时间:	2014-4-16	20:10
*********************************************************************************************************/
void SpurtGoTo(u8 dstX,u8 dstY)
{
	u8 blockStep;
	u8 strBlock = 0;
	u8 mazeX,mazeY;
	u8 dirTemp;
	
	mazeX = mouseCoor.X;
	mazeY = mouseCoor.Y;	
	MapStepEdit(dstX,dstY);
	MouseStart();
	
	while((mazeX != dstX) || (mazeY != dstY))
	{
		blockStep = mapStep[mazeX][mazeY];
		
		if((mapBlock[mazeX][mazeY] & 0x01) &&
		   (mapStep[mazeX][mazeY + 1] < blockStep))
		{
			dirTemp = MAZEANGLE0;
			if(dirTemp == mouse.dir)
			{
				strBlock++;
				mazeY++;
				continue;
			}
		}
		if((mapBlock[mazeX][mazeY] & 0x02) &&
		   (mapStep[mazeX + 1][mazeY] < blockStep))
		{
			dirTemp = MAZEANGLE90;
			if(dirTemp == mouse.dir)
			{
				strBlock++;
				mazeX++;
				continue;
			}
		}
		if((mapBlock[mazeX][mazeY] & 0x04) &&
		   (mapStep[mazeX][mazeY - 1] < blockStep))
		{
			dirTemp = MAZEANGLE180;
			if(dirTemp == mouse.dir)
			{
				strBlock++;
				mazeY--;
				continue;
			}
		}
		if((mapBlock[mazeX][mazeY] & 0x08) &&
		   (mapStep[mazeX - 1][mazeY] < blockStep))
		{
			dirTemp = MAZEANGLE270;
			if(dirTemp == mouse.dir)
			{
				strBlock++;
				mazeX--;
				continue;
			}
		}
		
		dirTemp = (dirTemp + 4 - mouse.dir) % 4;
//		if((mazeX == 7) && (mazeY == 4))
//			redLedOn;
//		if(strBlock == 5)
//			blueLedOn;
		if(strBlock)
			MouseGoStr(strBlock);
		strBlock = 0;
		switch(dirTemp)
		{
			case 1:
				SearchTurnRight();	
				break;
			case 2:
				MouseTurnBack();	
				break;

			case 3:
				SearchTurnLeft();
				break;
			default:
				break;
        }
	}
	if(strBlock)
		MouseGoStr(strBlock);
}

/*********************************************************************************************************
**	函数原型:	void MouseGoStr(u8 strBlock)
**	函数功能:	驱动电脑鼠向前直行strBlock格
**	输入参数:	前进格数
**	返回参数:	无
**	修改时间:	2014-5-20	20:10
*********************************************************************************************************/
void MouseGoStr(u8 strBlock)
{
	while(1)
	{
		if(mouse.vertD >= 180000)
		{
			CoorUpdate();
			mouse.vertD -= 180000;
			strBlock--;
		}
		if ((IRSensor.CrosswayStatus[1] & 0x0A) && (!(IRSensor.CrosswayStatus[2] & 0x0A)))
		{
			if (IRSensor.CrosswayStatus[1] & 0x08)
			{
				mouse.vertD = 63000;
			}
			else if (IRSensor.CrosswayStatus[1] & 0x02)
			{
				mouse.vertD = 75000;
			}
			IRSensor.IsCorrectionAllowed = FALSE;
		}
		if (mouse.vertD >= 145000)
		{
			IRSensor.IsCorrectionAllowed = TRUE;
			if(strBlock < 2)
			{
				CoorUpdate();
				break;
			}
//			if (IRSensor.CrosswayStatus[1] != IRSensor.CrosswayStatus[2])
//				break;
//			else if (IRSensor.CrosswayStatus[1] & 0x80)					//第一次初步判断左边有出口	
//			{
//				CoorUpdate();
//				SearchTurnLeft();
//				break;
//			}
//			else if (IRSensor.CrosswayStatus[1] & 0x04)
//			{
//				IRSensor.IsCorrectionAllowed = TRUE;
//				break;
//			}
//			else if (IRSensor.CrosswayStatus[1] & 0x20)					//第一次初步判断左边有出口	
//			{
//				while (mouse.vertD <= 155000);
//				CoorUpdate();
//				SearchTurnRight();
//				break;
//			}
//			else
//			{
//				while (IRSensor.CrosswayStatus[1] & 0x40);
//				CoorUpdate();
//				SearchBreak();
//				MouseTurnBack();
//				break;	
//			}
		}
	}	
}

/*********************************************************************************************************
**	函数原型:	void TIM2_IRQHandler(void)
**	函数功能:	TIM2定时中断服务子程序
**	输入参数:	无
**	返回参数:	无
**	修改时间:	2014-2-18	17:50
*********************************************************************************************************/
void TIM2_IRQHandler(void)
{
	static u8 timCount = 0;
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)!=RESET)
		TIM_ClearITPendingBit(TIM2,TIM_FLAG_Update);						/*	清除中断标志位			*/
	leftMotor_TargetDCalculation();											/*	左电机目标速度曲线生成	*/
	rightMotor_TargetDCalculation();										/*	右电机目标速度曲线生成	*/
	IRCheck();	
	LeftmotorPID();															/*	左电机控制函数			*/	
	RightmotorPID();														/*	右电机控制函数			*/

	if ( timCount == 0 )
	{
//		DataCache();	
	}
	timCount =( timCount + 1 )% 100;
}
