
/*********************************************************************************************************
** 
**    										    ±±¾©½»Í¨´óÑ§                                     
**
**--------------------------------------------------------------------------------------------------------
** 	ÎÄ¼þÃû£º		Maze.c
** 	´´½¨Ê±¼ä£º		2013-7-9 17£º20
** 	´´½¨ÈËÔ±£º 		ÕÔ±üÏÍ
** 	ÎÄ¼þÃèÊö:  		Ö÷³ÌÐòÎÄ¼þ ÃÔ¹¬Ëã·¨
** 
**--------------------------------------------------------------------------------------------------------
** 	×îºóÐÞ¸ÄÊ±¼ä£º	2014-1-7 10:15 
** 	×îºóÐÞ¸ÄÈËÔ±£º	ÕÔ±üÏÍ	
** 	°æ±¾ºÅ£º 	   	V1.1
** 	°æ±¾ÃèÊö£º 		Moment 0_2 µç»ú²âÊÔ
**
*********************************************************************************************************/

#include "stm32f10x.h"
#include "Motor.h"
#include "Usart.h"
#include "Delay.h"
#include "Interface.h"
#include "Maze.h"


/*********************************************************************************************************
**	±äÁ¿¶¨Òå
*********************************************************************************************************/

s32 				dataCache[1000][12]	=	{0};							/*	Êý¾Ý»º´æÆ÷				*/
u16 				numGroup 			= 	0;								/*	´®¿Ú·¢ËÍÊ±Ðò			*/
MICROMOUSE 			mouse				=	{0,0,0,0,0};					/*	µçÄÔÊó×´Ì¬´æ´¢			*/
static MAZECOOR		mouseCoor			= 	{0,0};							/*	µçÄÔÊó×ø±ê´¢´æ			*/
static MAZECOOR		crossWay[256]		= 	{0,0};							/*	´¢´æÓÐÎ´×ßÖ§Â·µÄ×ø±ê	*/
static u8			mapBlock[16][16] 	= 	{0};							/*	µØÍ¼ÐÅÏ¢´¢´æ			*/
static u8			mapStep[16][16] 	= 	{0};							/*	×ø±êµãµÈ¸ßÖµ´¢´æ		*/
//static u8			mazeStartX			=	0;								/*	ÃÔ¹¬Æðµãºá×ø±ê			*/
//static u8			mazeStartY			=	0;								/*	ÃÔ¹¬Æðµã×Ý×ø±ê			*/
u8 data[4];

/*********************************************************************************************************
**	Ö÷º¯Êý
*********************************************************************************************************/ 

int main(void)
{	
	char dataUsart[40];													/*	´®¿Ú·¢ËÍÔÝ´æÆ÷			*/
	u16 usartGroup;
	u8 tempStartY;
	u8 numCrossWay;
	u8 unCoverRoad;
	
	ClockInit();		  												/*	ÏµÍ³Ê±ÖÓ³õÊ¼»¯			*/
	DelayInit(72);														/*	ÑÓÊ±º¯Êý³õÊ¼»¯			*/
	UsartInit();														/*	´®¿ÚÍ¨ÐÅ³õÊ¼»¯			*/
	MotorInit();												   		/*	µç»úÇý¶¯³õÊ¼»¯			*/
	SensorIRInit();												   		/*	ºìÍâ¼ì²â³õÊ¼»¯			*/
	KeyInit();															/*	°´¼ü¼ì²â³õÊ¼»¯			*/
	InterfaceInit();													/*	½»»¥Ä£¿é³õÊ¼»¯			*/

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
**	º¯ÊýÔ­ÐÍ:	void ClockInit(void)
**	º¯Êý¹¦ÄÜ:	ÏµÍ³Ê±ÖÓ³õÊ¼»¯
**	ÊäÈë²ÎÊý:	ÎÞ
**	Êä³ö²ÎÊý:	ÎÞ
**	·µ»Ø²ÎÊý:	ÎÞ
**	ÐÞ¸ÄÊ±¼ä:	2013-7-15	11:15
*********************************************************************************************************/
void ClockInit(void)
{
	ErrorStatus HSEStartUpStatus;											/* 	¶¨Òå´íÎó×´Ì¬±äÁ¿		*/
	RCC_DeInit();															/*	RCC¼Ä´æÆ÷³õÊ¼»¯ÎªÈ±Ê¡Öµ	*/
	RCC_HSEConfig(RCC_HSE_ON);												/*	Ê¹ÄÜÍâ²¿¸ßËÙÊ±ÖÓHSE		*/
	HSEStartUpStatus = RCC_WaitForHSEStartUp();							   	/*	µÈ´ýÍâ²¿¾§Õñ¹¤×÷		*/
	if(HSEStartUpStatus == SUCCESS)											/*	Èç¹ûÍâ²¿¾§ÕñÕý³£¹¤×÷	*/
	{
		FLASH_SetLatency(FLASH_Latency_2);									/*	Flash´æ´¢Æ÷ÑÓÊ±2ÖÜÆÚ	*/
		RCC_HCLKConfig(RCC_SYSCLK_Div1);									/*	AHBÊ±ÖÓ = ÏµÍ³Ê±ÖÓ		*/
		RCC_PCLK2Config(RCC_HCLK_Div1);										/*	¸ßËÙAHBÊ±ÖÓ = ÏµÍ³Ê±ÖÓ	*/
		RCC_PCLK1Config(RCC_HCLK_Div2);										/*	µÍËÙAHBÊ±ÖÓ = ÏµÍ³Ê±ÖÓ/2*/
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);				/*	PLLÊ±ÖÓ = ÏµÍ³Ê±ÖÓ¡Á9	*/
		RCC_PLLCmd(ENABLE);													/*	PLLÊ±ÖÓÊ¹ÄÜ				*/
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
		{
		}																	
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);							/*	Ñ¡ÔñPLLÎªÏµÍ³Ê±ÖÓ		*/
		while(RCC_GetSYSCLKSource() != 0x08)								/*	µÈ´ýÏµÍ³Ê±ÖÓÉèÖÃÍê³É	*/
		{
		}
	}	
	RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOA|							/*	Ê¹ÄÜGPIOAÊ±ÖÓ			*/
							RCC_APB2Periph_GPIOB|							/*	Ê¹ÄÜGPIOBÊ±ÖÓ			*/
							RCC_APB2Periph_GPIOC|							/*	Ê¹ÄÜGPIOCÊ±ÖÓ			*/
							RCC_APB2Periph_GPIOD|							/*	Ê¹ÄÜGPIODÊ±ÖÓ			*/
							RCC_APB2Periph_AFIO,							/*	Ê¹ÄÜAFIOÊ±ÖÓ			*/
							ENABLE);
}

/*********************************************************************************************************
**	º¯ÊýÔ­ÐÍ:	void KeyInit(void)
**	º¯Êý¹¦ÄÜ:	°´¼ü¼ì²â³õÊ¼»¯
**	ÊäÈë²ÎÊý:	ÎÞ
**	Êä³ö²ÎÊý:	ÎÞ
**	·µ»Ø²ÎÊý:	ÎÞ
**	ÐÞ¸ÄÊ±¼ä:	2014-1-7	11:15
*********************************************************************************************************/
void KeyInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = Key;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;							/*	ÉèÖÃ¹Ü½ÅÉÏÀ­ÊäÈë		*/
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*********************************************************************************************************
**	º¯ÊýÔ­ÐÍ:	u8 KeyCheck(void)
**	º¯Êý¹¦ÄÜ:	°´¼ü¼ì²â
**	ÊäÈë²ÎÊý:	ÎÞ
**	Êä³ö²ÎÊý:	ÎÞ
**	·µ»Ø²ÎÊý:	TURE£º	°´¼üÒÔ°´ÏÂ
				FALSE£º	°´¼üÎ´°´ÏÂ
**	ÐÞ¸ÄÊ±¼ä:	2014-1-7	11:15
*********************************************************************************************************/
u8 KeyCheck(void)
{
	if(GPIO_ReadInputDataBit(GPIOA, Key) == 0)								/*	¶ÁÈ¡¹Ü½ÅµçÎ»×´Ì¬		*/
	{
		Delayms(50);														/*	ÑÓÊ±50ms				*/
		while(GPIO_ReadInputDataBit(GPIOA, Key) == 0);						/*	ÖØÐÂ¼ì²â·ÀÖ¹ÎóÅÐ		*/
		return(TRUE);														/*	·µ»Ø°´¼ü±»°´ÏÂ			*/
	}
	else
	{
	 	return(FALSE);														/*	·µ»Ø°´¼üÎ´°´ÏÂ			*/
	}		
}

/*********************************************************************************************************
**	º¯ÊýÔ­ÐÍ:	s32 Abs(s32 absValue)
**	º¯Êý¹¦ÄÜ:	Çó²ÎÊýµÄ¾ø¶ÔÖµ
**	ÊäÈë²ÎÊý:	´ýÇó²ÎÊý
**	Êä³ö²ÎÊý:	ÎÞ
**	·µ»Ø²ÎÊý:	ËùÇó²ÎÊýµÄ¾ø¶ÔÖµ
**	ÐÞ¸ÄÊ±¼ä:	2014-2-20	15:55
*********************************************************************************************************/
s32 Abs(s32 absValue)
{
	if(absValue < 0)														/*	Èç¹ûÊäÈë²ÎÊýÎª¸ºÊý		*/							
		return -absValue;													/*	Êä³öËüµÄÏà·´Êý			*/			
	else 
		return absValue;
}

/*********************************************************************************************************
**	º¯ÊýÔ­ÐÍ:	void DataCache(void)
**	º¯Êý¹¦ÄÜ:	ÔËÐÐÊý¾Ý»º´æº¯Êý
**	ÊäÈë²ÎÊý:	ÎÞ
**	Êä³ö²ÎÊý:	ÎÞ
**	·µ»Ø²ÎÊý:	ÎÞ
**	ÐÞ¸ÄÊ±¼ä:	2014-3-6	11:00
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
**	º¯ÊýÔ­ÐÍ:	void Timer2IRQInit(void)
**	º¯Êý¹¦ÄÜ:	¶¨Ê±ÖÐ¶Ï³õÊ¼»¯
**	ÊäÈë²ÎÊý:	ÎÞ
**	Êä³ö²ÎÊý:	ÎÞ
**	·µ»Ø²ÎÊý:	ÎÞ
**	ÐÞ¸ÄÊ±¼ä:	2014-2-18	17:50
*********************************************************************************************************/
void Timer2IRQInit(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);							/*	3bitÇÀ¶ÏÎ»£¬1bitÏìÓ¦Î»	*/
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  						/*	Ñ¡ÔñTIM2È«¾ÖÖÐ¶Ï		*/
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;				/*	ÉèÖÃÇÀ¶ÏÓÅÏÈ¼¶Îª1		*/
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;						/*	ÉèÖÃÏìÓ¦ÓÅÏÈ¼¶Îª0		*/	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;							/*	¿ªÖÐ¶Ï					*/
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_DeInit(TIM2);														/*	¶¨Ê±Æ÷¸´Î»				*/
	TIM_TimeBaseStructure.TIM_Period=499;									/*	Éè¶¨¶¨Ê±Ê±³¤Îª1ms		*/
	TIM_TimeBaseStructure.TIM_Prescaler=35;									/*	¶¨Ê±Æ÷16·ÖÆµ ÆµÂÊÎª1MHz	*/
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;					/*	Ê±ÖÓ·Ö¸îÖµÎª1			*/
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;				/*	ÏòÉÏ¼ÆÊýÄ£Ê½			*/
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);							/*	³õÊ¼»¯¶¨Ê±Æ÷µÄÖµ		*/
	TIM_ClearFlag(TIM2,TIM_FLAG_Update);									/*	Çå³ýÖÐ¶Ï±êÖ¾Î»			*/
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);								/*	Ê¹ÄÜÖÐ¶Ï				*/
	TIM_Cmd(TIM2,ENABLE);													/*	¶¨ÄÜ¶¨Ê±Æ÷				*/
}

/*********************************************************************************************************
**	º¯ÊýÔ­ÐÍ:	void SearchGoAhead(void)
**	º¯Êý¹¦ÄÜ:	µçÄÔÊóËÑË÷Ç°½ø
**	ÊäÈë²ÎÊý:	ÎÞ
**	Êä³ö²ÎÊý:	ÎÞ
**	·µ»Ø²ÎÊý:	ÎÞ
**	ÐÞ¸ÄÊ±¼ä:	2014-4-28 14:30 
*********************************************************************************************************/
void SearchGoAhead(void)		//ÕûÌå×ø±ê¸üÐÂÂß¼­ÐèÒª½øÒ»²½Ì½ÌÖ
{
	while (1)
	{
		if ((!(IRSensor.CrosswayStatus[2] & 0x0A)) && (IRSensor.CrosswayStatus[1] & 0x0A))//Ð£Õý×ø±ê ÓÐÇ½->ÎÞÇ½
		{					//µ±Ç°Âß¼­½öÔÚ ×óÓÒÁ½²à¶¼ÓÐÇ½->ÈÎÒâÒ»²àÎÞÇ½±ÚÊ±ÓÐÐ§ ÊÇ·ñÐÞ¸ÄÓÐ´ýÉÌÈ¶£¨Âß¼­ÐÔºÍÊÊÓÃÐÔ£©
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
			if ((IRSensor.CrosswayStatus[1] & 0x80)	&& (!mouse.coorSwt))			//µÚÒ»´Î³õ²½ÅÐ¶Ï×ó±ßÓÐ³ö¿Ú	
			{																		//Ã»ÓÐÌí¼ÓÂ·¿ÚÖØ¸´ÅÐ¶Ï
				CoorUpdate();
				break;
			}
			else if ((IRSensor.CrosswayStatus[1] & 0x20) && (!mouse.coorSwt))		//µÚÒ»´Î³õ²½ÅÐ¶ÏÓÒ±ßÓÐ³ö¿Ú	
			{																		//ÉÐÎ´Ìí¼ÓÖØ¸´ÅÐ¶Ï
				CoorUpdate();
				break;
			}
			else if (!(IRSensor.CrosswayStatus[1] & 0x01))
			{																	//Í¬ÑùµÄÎÊÌâ ÖØ¸´ÅÐ¶ÏÉÐÎ´Ìí¼Ó
				CoorUpdate();
				while (IRSensor.CrosswayStatus[1] & 0x10);
				break;
			}
		}
		if(mouse.vertD >= 180000)
		{
			if(mouse.coorSwt != 3)											//·ÀÖ¹Ö±ÐÐÂ·¿Ú×ø±ê¶þ´Î¸üÐÂ
				CoorUpdate();
			else
				mouse.coorSwt = 1;
			mouse.vertD -= 180000;
		}
	}
}

/*********************************************************************************************************
**	º¯ÊýÔ­ÐÍ:	void CoorUpdate(void)
**	º¯Êý¹¦ÄÜ:	µçÄÔÊó×ø±ê¸üÐÂº¯Êý
**	ÊäÈë²ÎÊý:	ÎÞ
**	·µ»Ø²ÎÊý:	ÎÞ
**	ÐÞ¸ÄÊ±¼ä:	2014-4-4	11:30
*********************************************************************************************************/
void CoorUpdate(void)
{
	switch(mouse.dir)
	{
		case MAZEANGLE0:													/*	µçÄÔÊó·½ÏòÎª0¶È			*/
			mouseCoor.Y++;													/*	×Ý×ø±ê + 1				*/
			break;

		case MAZEANGLE90:													/*	µçÄÔÊó·½ÏòÎª90¶È		*/
			mouseCoor.X++;													/*	ºá×ø±ê + 1				*/
			break;

		case MAZEANGLE180:													/*	µçÄÔÊó·½ÏòÎª180¶È		*/
			mouseCoor.Y--;													/*	×Ý×ø±ê - 1				*/
			break;

		case MAZEANGLE270:													/*	µçÄÔÊó·½ÏòÎª270¶È		*/
			mouseCoor.X--;													/*	ºá×ø±ê - 1				*/	
			break;
		default:
			break;
	}
	mouse.coorSwt = 1;
	WallCheck();															/*	Ç½±ÚÐÅÏ¢¼ì²â			*/
}

/*********************************************************************************************************
**	º¯ÊýÔ­ÐÍ:	void WallCheck(void)
**	º¯Êý¹¦ÄÜ:	µçÄÔÊóÇ½±Ú¼ì²âº¯Êý
**	ÊäÈë²ÎÊý:	ÎÞ
**	·µ»Ø²ÎÊý:	ÎÞ
**	ÐÞ¸ÄÊ±¼ä:	2014-4-8	21:30
*********************************************************************************************************/
void WallCheck(void)
{
	u8 mazeBezel = 0;
	mazeBezel |= MOUSEWAY_B;
	
	if (IRSensor.CrosswayStatus[1] & 0x80)   								/*	×ó²àÇ½±ÚÅÐ¶Ï			*/
		mazeBezel |=  MOUSEWAY_L; 
	else 
		mazeBezel &= ~MOUSEWAY_L ;
	
	if (IRSensor.CrosswayStatus[1] & 0x01)									/*	Ç°·½Ç½±ÚÅÐ¶Ï			*/
		mazeBezel |=  MOUSEWAY_F;
	else 
		mazeBezel &= ~MOUSEWAY_F;

	if (IRSensor.CrosswayStatus[1] & 0x20) 									/*	ÓÒ²àÇ½±ÚÅÐ¶Ï			*/
		mazeBezel |=  MOUSEWAY_R;
	else 
		mazeBezel &= ~MOUSEWAY_R;
		
	if (mapBlock[mouseCoor.X][mouseCoor.Y] == 0x00) 
    {
        mapBlock[mouseCoor.X][mouseCoor.Y] = mazeBezel;						/*	Ç½±ÚÐÅÏ¢¼ÇÂ¼			*/
		DataCache();
    }
}

/*********************************************************************************************************
**	º¯ÊýÔ­ÐÍ:	u8 CrossWayCheck(u8 mazeX, u8 mazeY)
**	º¯Êý¹¦ÄÜ:	Í³¼ÆÄ³×ø±êµã»¹Î´×ß¹ýµÄÖ§Â·Êý
**	ÊäÈë²ÎÊý:	mazeX:´ø¼ì²âµãµÄºá×ø±ê
				mazeY:´ø¼ì²âµãµÄ×Ý×ø±ê
**	·µ»Ø²ÎÊý:	unCoverRoad:Î´×ß¹ýµÄÖ§Â·Êý
**	ÐÞ¸ÄÊ±¼ä:	2014-4-16	19:40
*********************************************************************************************************/
u8 CrossWayCheck(u8 mazeX, u8 mazeY)
{
	u8 unCoverRoad = 0;
	if((mapBlock[mazeX][mazeY] & 0x01) &&									/*	¾ø¶Ô·½Ïò ÃÔ¹¬ÉÏ·½ÓÐÂ·	*/
	   (mapBlock[mazeX][mazeY + 1]) == 0x00)								/*	¾ø¶Ô·½Ïò ÃÔ¹¬ÉÏ·½Î´×ß¹ý	*/
		unCoverRoad++;														/*	¿ÉÇ°½ø·½ÏòÊý¼Ó1			*/
		
	if((mapBlock[mazeX][mazeY] & 0x02) &&									/*	¾ø¶Ô·½Ïò ÃÔ¹¬ÓÒ·½ÓÐÂ·	*/
	   (mapBlock[mazeX + 1][mazeY]) == 0x00)								/*	¾ø¶Ô·½Ïò ÃÔ¹¬ÓÒ·½Î´×ß¹ý	*/
		unCoverRoad++;														/*	¿ÉÇ°½ø·½ÏòÊý¼Ó1			*/
		
	if((mapBlock[mazeX][mazeY] & 0x04) &&									/*	¾ø¶Ô·½Ïò ÃÔ¹¬ÏÂ·½ÓÐÂ·	*/
	   (mapBlock[mazeX][mazeY - 1]) == 0x00)								/*	¾ø¶Ô·½Ïò ÃÔ¹¬ÏÂ·½Î´×ß¹ý	*/
		unCoverRoad++;														/*	¿ÉÇ°½ø·½ÏòÊý¼Ó1			*/
		
	if((mapBlock[mazeX][mazeY] & 0x08) &&									/*	¾ø¶Ô·½Ïò ÃÔ¹¬×ó·½ÓÐÂ·	*/
	   (mapBlock[mazeX - 1][mazeY]) == 0x00)								/*	¾ø¶Ô·½Ïò ÃÔ¹¬×ó·½Î´×ß¹ý	*/
		unCoverRoad++;														/*	¿ÉÇ°½ø·½ÏòÊý¼Ó1			*/
	
	return unCoverRoad;														/*	·µ»Ø¿ÉÇ°½ø·½ÏòÊý		*/
}

/*********************************************************************************************************
**	º¯ÊýÔ­ÐÍ:	void CrossWayChoice(void)
**	º¯Êý¹¦ÄÜ:	Ñ¡ÔñÒ»ÌõÖ§Â·×÷ÎªÇ°½ø·½Ïò
**	ÊäÈë²ÎÊý:	ÎÞ
**	·µ»Ø²ÎÊý:	ÎÞ
**	ÐÞ¸ÄÊ±¼ä:	2014-4-16	19:57
*********************************************************************************************************/
void CrossWayChoice(void)
{
	if((mapBlock[mouseCoor.X][mouseCoor.Y] & MOUSEWAY_R) &&					/*	µçÄÔÊóµÄÓÒ±ßÓÐÂ·		*/
	   (MapBlockDataGet(MOUSERIGHT) == 0x00))								/*	µçÄÔÊóÓÒ±ßÃ»ÓÐ×ß¹ý		*/
	{
		SearchTurnRight();													/*	µçÄÔÊóÓÒ×ª				*/
		return;
	}
	   
	if((mapBlock[mouseCoor.X][mouseCoor.Y] & MOUSEWAY_F) &&					/*	µçÄÔÊóµÄÇ°±ßÓÐÂ·		*/		
	   (MapBlockDataGet(MOUSEFRONT) == 0x00))								/*	µçÄÔÊóÇ°±ßÃ»ÓÐ×ß¹ý	*/
	{
	   IRSensor.IsCorrectionAllowed = TRUE;
	   mouse.coorSwt = 3;
	   return;																/*	µçÄÔÊó²»ÓÃ×ªÍä			*/
	}
	
	if((mapBlock[mouseCoor.X][mouseCoor.Y] & MOUSEWAY_L) &&					/*	µçÄÔÊó×ó×ª				*/
	   (MapBlockDataGet(MOUSELEFT) == 0x00))								/*	µçÄÔÊó×ó±ßÃ»ÓÐ×ß¹ý		*/
	{
		SearchTurnLeft();													/*	µçÄÔÊó×ó×ª				*/
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
**	º¯ÊýÔ­ÐÍ:	u8 MapBlockDataGet(u8 dirTemp)
**	º¯Êý¹¦ÄÜ:	¸ù¾ÝµçÄÔÊóµÄÏà¶Ô·½Ïò£¬È¡³ö¸Ã·½ÏòÉÏÃÔ¹¬¸ñµÄÇ½±Ú×ÊÁÏ
**	ÊäÈë²ÎÊý:	dirTemp£ºµçÄÔÊóµÄÏà¶Ô·½Ïò
**	·µ»Ø²ÎÊý:	Ïà¶Ô·½ÏòÉÏµÄÃÔ¹¬¸ñÇ½±ÚÐÅÏ¢
**	ÐÞ¸ÄÊ±¼ä:	2014-4-16	20:10
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
**	º¯ÊýÔ­ÐÍ:	void MapStepEdit(u8 mazeX,u8 mazeY)
**	º¯Êý¹¦ÄÜ:	ÖÆ×÷ÒÔÄ¿±êµãÎªÆðµãµÄµÈ¸ßÍ¼
**	ÊäÈë²ÎÊý:	Ä¿±êµãµÄºá×Ý×ø±ê
**	Êä³ö²ÎÊý:	¸÷×ø±êµãµÄµÈ¸ßÖµ
**	·µ»Ø²ÎÊý:	ÎÞ
**	ÐÞ¸ÄÊ±¼ä:	2014-5-20	11:10
*********************************************************************************************************/
void MapStepEdit(u8 mazeX,u8 mazeY)
{
	MAZECOOR	crossWay[256]		= 	{0};						/*	µÈ¸ßÍ¼ÓÐÎ´×ßÖ§Â·µÄ×ø±ê	*/
	u8			dirStack[16][16]	= 	{0};						/*	´¢´æ×ø±êµãµÄ½ø³ö·½Ïò	*/
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
**	º¯ÊýÔ­ÐÍ:	void SpurtGoTo(u8 mazeX,u8 mazeY)
**	º¯Êý¹¦ÄÜ:	Çý¶¯µçÄÔÊóÖ±½ÓÔË¶¯µ½Ö¸¶¨×ø±ê¸ñ
**	ÊäÈë²ÎÊý:	Ä¿±êµãµÄºá×Ý×ø±ê
**	·µ»Ø²ÎÊý:	ÎÞ¢
**	ÐÞ¸ÄÊ±¼ä:	2014-4-16	20:10
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
**	º¯ÊýÔ­ÐÍ:	void MouseGoStr(u8 strBlock)
**	º¯Êý¹¦ÄÜ:	Çý¶¯µçÄÔÊóÏòÇ°Ö±ÐÐstrBlock¸ñ
**	ÊäÈë²ÎÊý:	Ç°½ø¸ñÊý
**	·µ»Ø²ÎÊý:	ÎÞ
**	ÐÞ¸ÄÊ±¼ä:	2014-5-20	20:10
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
//			else if (IRSensor.CrosswayStatus[1] & 0x80)					//µÚÒ»´Î³õ²½ÅÐ¶Ï×ó±ßÓÐ³ö¿Ú	
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
//			else if (IRSensor.CrosswayStatus[1] & 0x20)					//µÚÒ»´Î³õ²½ÅÐ¶Ï×ó±ßÓÐ³ö¿Ú	
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
**	º¯ÊýÔ­ÐÍ:	void TIM2_IRQHandler(void)
**	º¯Êý¹¦ÄÜ:	TIM2¶¨Ê±ÖÐ¶Ï·þÎñ×Ó³ÌÐò
**	ÊäÈë²ÎÊý:	ÎÞ
**	·µ»Ø²ÎÊý:	ÎÞ
**	ÐÞ¸ÄÊ±¼ä:	2014-2-18	17:50
*********************************************************************************************************/
void TIM2_IRQHandler(void)
{
	static u8 timCount = 0;
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)!=RESET)
		TIM_ClearITPendingBit(TIM2,TIM_FLAG_Update);						/*	Çå³ýÖÐ¶Ï±êÖ¾Î»			*/
	leftMotor_TargetDCalculation();											/*	×óµç»úÄ¿±êËÙ¶ÈÇúÏßÉú³É	*/
	rightMotor_TargetDCalculation();										/*	ÓÒµç»úÄ¿±êËÙ¶ÈÇúÏßÉú³É	*/
	IRCheck();	
	LeftmotorPID();															/*	×óµç»ú¿ØÖÆº¯Êý			*/	
	RightmotorPID();														/*	ÓÒµç»ú¿ØÖÆº¯Êý			*/

	if ( timCount == 0 )
	{
//		DataCache();	
	}
	timCount =( timCount + 1 )% 100;
}
