
/*********************************************************************************************************
** 
**    										    ������ͨ��ѧ                                     
**
**--------------------------------------------------------------------------------------------------------
** 	�ļ�����		Maze.c
** 	����ʱ�䣺		2013-7-9 17��20
** 	������Ա�� 		�Ա���
** 	�ļ�����:  		�������ļ� �Թ��㷨
** 
**--------------------------------------------------------------------------------------------------------
** 	����޸�ʱ�䣺	2014-1-7 10:15 
** 	����޸���Ա��	�Ա���	
** 	�汾�ţ� 	   	V1.1
** 	�汾������ 		Moment 0_2 �������
**
*********************************************************************************************************/

#include "stm32f10x.h"
#include "Motor.h"
#include "Usart.h"
#include "Delay.h"
#include "Interface.h"
#include "Maze.h"


/*********************************************************************************************************
**	��������
*********************************************************************************************************/

s32 				dataCache[1000][12]	=	{0};							/*	���ݻ�����				*/
u16 				numGroup 			= 	0;								/*	���ڷ���ʱ��			*/
MICROMOUSE 			mouse				=	{0,0,0,0,0};					/*	������״̬�洢			*/
static MAZECOOR		mouseCoor			= 	{0,0};							/*	���������괢��			*/
static MAZECOOR		crossWay[256]		= 	{0,0};							/*	������δ��֧·������	*/
static u8			mapBlock[16][16] 	= 	{0};							/*	��ͼ��Ϣ����			*/
static u8			mapStep[16][16] 	= 	{0};							/*	�����ȸ�ֵ����		*/
//static u8			mazeStartX			=	0;								/*	�Թ���������			*/
//static u8			mazeStartY			=	0;								/*	�Թ����������			*/
u8 data[4];

/*********************************************************************************************************
**	������
*********************************************************************************************************/ 

int main(void)
{	
	char dataUsart[40];													/*	���ڷ����ݴ���			*/
	u16 usartGroup;
	u8 tempStartY;
	u8 numCrossWay;
	u8 unCoverRoad;
	
	ClockInit();		  												/*	ϵͳʱ�ӳ�ʼ��			*/
	DelayInit(72);														/*	��ʱ������ʼ��			*/
	UsartInit();														/*	����ͨ�ų�ʼ��			*/
	MotorInit();												   		/*	���������ʼ��			*/
	SensorIRInit();												   		/*	�������ʼ��			*/
	KeyInit();															/*	��������ʼ��			*/
	InterfaceInit();													/*	����ģ���ʼ��			*/

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
**	����ԭ��:	void ClockInit(void)
**	��������:	ϵͳʱ�ӳ�ʼ��
**	�������:	��
**	�������:	��
**	���ز���:	��
**	�޸�ʱ��:	2013-7-15	11:15
*********************************************************************************************************/
void ClockInit(void)
{
	ErrorStatus HSEStartUpStatus;											/* 	�������״̬����		*/
	RCC_DeInit();															/*	RCC�Ĵ�����ʼ��Ϊȱʡֵ	*/
	RCC_HSEConfig(RCC_HSE_ON);												/*	ʹ���ⲿ����ʱ��HSE		*/
	HSEStartUpStatus = RCC_WaitForHSEStartUp();							   	/*	�ȴ��ⲿ������		*/
	if(HSEStartUpStatus == SUCCESS)											/*	����ⲿ������������	*/
	{
		FLASH_SetLatency(FLASH_Latency_2);									/*	Flash�洢����ʱ2����	*/
		RCC_HCLKConfig(RCC_SYSCLK_Div1);									/*	AHBʱ�� = ϵͳʱ��		*/
		RCC_PCLK2Config(RCC_HCLK_Div1);										/*	����AHBʱ�� = ϵͳʱ��	*/
		RCC_PCLK1Config(RCC_HCLK_Div2);										/*	����AHBʱ�� = ϵͳʱ��/2*/
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);				/*	PLLʱ�� = ϵͳʱ�ӡ�9	*/
		RCC_PLLCmd(ENABLE);													/*	PLLʱ��ʹ��				*/
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
		{
		}																	
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);							/*	ѡ��PLLΪϵͳʱ��		*/
		while(RCC_GetSYSCLKSource() != 0x08)								/*	�ȴ�ϵͳʱ���������	*/
		{
		}
	}	
	RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOA|							/*	ʹ��GPIOAʱ��			*/
							RCC_APB2Periph_GPIOB|							/*	ʹ��GPIOBʱ��			*/
							RCC_APB2Periph_GPIOC|							/*	ʹ��GPIOCʱ��			*/
							RCC_APB2Periph_GPIOD|							/*	ʹ��GPIODʱ��			*/
							RCC_APB2Periph_AFIO,							/*	ʹ��AFIOʱ��			*/
							ENABLE);
}

/*********************************************************************************************************
**	����ԭ��:	void KeyInit(void)
**	��������:	��������ʼ��
**	�������:	��
**	�������:	��
**	���ز���:	��
**	�޸�ʱ��:	2014-1-7	11:15
*********************************************************************************************************/
void KeyInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = Key;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;							/*	���ùܽ���������		*/
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*********************************************************************************************************
**	����ԭ��:	u8 KeyCheck(void)
**	��������:	�������
**	�������:	��
**	�������:	��
**	���ز���:	TURE��	�����԰���
				FALSE��	����δ����
**	�޸�ʱ��:	2014-1-7	11:15
*********************************************************************************************************/
u8 KeyCheck(void)
{
	if(GPIO_ReadInputDataBit(GPIOA, Key) == 0)								/*	��ȡ�ܽŵ�λ״̬		*/
	{
		Delayms(50);														/*	��ʱ50ms				*/
		while(GPIO_ReadInputDataBit(GPIOA, Key) == 0);						/*	���¼���ֹ����		*/
		return(TRUE);														/*	���ذ���������			*/
	}
	else
	{
	 	return(FALSE);														/*	���ذ���δ����			*/
	}		
}

/*********************************************************************************************************
**	����ԭ��:	s32 Abs(s32 absValue)
**	��������:	������ľ���ֵ
**	�������:	�������
**	�������:	��
**	���ز���:	��������ľ���ֵ
**	�޸�ʱ��:	2014-2-20	15:55
*********************************************************************************************************/
s32 Abs(s32 absValue)
{
	if(absValue < 0)														/*	����������Ϊ����		*/							
		return -absValue;													/*	��������෴��			*/			
	else 
		return absValue;
}

/*********************************************************************************************************
**	����ԭ��:	void DataCache(void)
**	��������:	�������ݻ��溯��
**	�������:	��
**	�������:	��
**	���ز���:	��
**	�޸�ʱ��:	2014-3-6	11:00
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
**	����ԭ��:	void Timer2IRQInit(void)
**	��������:	��ʱ�жϳ�ʼ��
**	�������:	��
**	�������:	��
**	���ز���:	��
**	�޸�ʱ��:	2014-2-18	17:50
*********************************************************************************************************/
void Timer2IRQInit(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);							/*	3bit����λ��1bit��Ӧλ	*/
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  						/*	ѡ��TIM2ȫ���ж�		*/
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;				/*	�����������ȼ�Ϊ1		*/
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;						/*	������Ӧ���ȼ�Ϊ0		*/	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;							/*	���ж�					*/
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_DeInit(TIM2);														/*	��ʱ����λ				*/
	TIM_TimeBaseStructure.TIM_Period=499;									/*	�趨��ʱʱ��Ϊ1ms		*/
	TIM_TimeBaseStructure.TIM_Prescaler=35;									/*	��ʱ��16��Ƶ Ƶ��Ϊ1MHz	*/
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;					/*	ʱ�ӷָ�ֵΪ1			*/
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;				/*	���ϼ���ģʽ			*/
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);							/*	��ʼ����ʱ����ֵ		*/
	TIM_ClearFlag(TIM2,TIM_FLAG_Update);									/*	����жϱ�־λ			*/
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);								/*	ʹ���ж�				*/
	TIM_Cmd(TIM2,ENABLE);													/*	���ܶ�ʱ��				*/
}

/*********************************************************************************************************
**	����ԭ��:	void SearchGoAhead(void)
**	��������:	����������ǰ��
**	�������:	��
**	�������:	��
**	���ز���:	��
**	�޸�ʱ��:	2014-4-28 14:30 
*********************************************************************************************************/
void SearchGoAhead(void)		//������������߼���Ҫ��һ��̽��
{
	while (1)
	{
		if ((!(IRSensor.CrosswayStatus[2] & 0x0A)) && (IRSensor.CrosswayStatus[1] & 0x0A))//У������ ��ǽ->��ǽ
		{					//��ǰ�߼����� �������඼��ǽ->����һ����ǽ��ʱ��Ч �Ƿ��޸��д���ȶ���߼��Ժ������ԣ�
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
			if ((IRSensor.CrosswayStatus[1] & 0x80)	&& (!mouse.coorSwt))			//��һ�γ����ж�����г���	
			{																		//û�����·���ظ��ж�
				CoorUpdate();
				break;
			}
			else if ((IRSensor.CrosswayStatus[1] & 0x20) && (!mouse.coorSwt))		//��һ�γ����ж��ұ��г���	
			{																		//��δ����ظ��ж�
				CoorUpdate();
				break;
			}
			else if (!(IRSensor.CrosswayStatus[1] & 0x01))
			{																	//ͬ�������� �ظ��ж���δ���
				CoorUpdate();
				while (IRSensor.CrosswayStatus[1] & 0x10);
				break;
			}
		}
		if(mouse.vertD >= 180000)
		{
			if(mouse.coorSwt != 3)											//��ֱֹ��·��������θ���
				CoorUpdate();
			else
				mouse.coorSwt = 1;
			mouse.vertD -= 180000;
		}
	}
}

/*********************************************************************************************************
**	����ԭ��:	void CoorUpdate(void)
**	��������:	������������º���
**	�������:	��
**	���ز���:	��
**	�޸�ʱ��:	2014-4-4	11:30
*********************************************************************************************************/
void CoorUpdate(void)
{
	switch(mouse.dir)
	{
		case MAZEANGLE0:													/*	��������Ϊ0��			*/
			mouseCoor.Y++;													/*	������ + 1				*/
			break;

		case MAZEANGLE90:													/*	��������Ϊ90��		*/
			mouseCoor.X++;													/*	������ + 1				*/
			break;

		case MAZEANGLE180:													/*	��������Ϊ180��		*/
			mouseCoor.Y--;													/*	������ - 1				*/
			break;

		case MAZEANGLE270:													/*	��������Ϊ270��		*/
			mouseCoor.X--;													/*	������ - 1				*/	
			break;
		default:
			break;
	}
	mouse.coorSwt = 1;
	WallCheck();															/*	ǽ����Ϣ���			*/
}

/*********************************************************************************************************
**	����ԭ��:	void WallCheck(void)
**	��������:	������ǽ�ڼ�⺯��
**	�������:	��
**	���ز���:	��
**	�޸�ʱ��:	2014-4-8	21:30
*********************************************************************************************************/
void WallCheck(void)
{
	u8 mazeBezel = 0;
	mazeBezel |= MOUSEWAY_B;
	
	if (IRSensor.CrosswayStatus[1] & 0x80)   								/*	���ǽ���ж�			*/
		mazeBezel |=  MOUSEWAY_L; 
	else 
		mazeBezel &= ~MOUSEWAY_L ;
	
	if (IRSensor.CrosswayStatus[1] & 0x01)									/*	ǰ��ǽ���ж�			*/
		mazeBezel |=  MOUSEWAY_F;
	else 
		mazeBezel &= ~MOUSEWAY_F;

	if (IRSensor.CrosswayStatus[1] & 0x20) 									/*	�Ҳ�ǽ���ж�			*/
		mazeBezel |=  MOUSEWAY_R;
	else 
		mazeBezel &= ~MOUSEWAY_R;
		
	if (mapBlock[mouseCoor.X][mouseCoor.Y] == 0x00) 
    {
        mapBlock[mouseCoor.X][mouseCoor.Y] = mazeBezel;						/*	ǽ����Ϣ��¼			*/
		DataCache();
    }
}

/*********************************************************************************************************
**	����ԭ��:	u8 CrossWayCheck(u8 mazeX, u8 mazeY)
**	��������:	ͳ��ĳ����㻹δ�߹���֧·��
**	�������:	mazeX:������ĺ�����
				mazeY:�������������
**	���ز���:	unCoverRoad:δ�߹���֧·��
**	�޸�ʱ��:	2014-4-16	19:40
*********************************************************************************************************/
u8 CrossWayCheck(u8 mazeX, u8 mazeY)
{
	u8 unCoverRoad = 0;
	if((mapBlock[mazeX][mazeY] & 0x01) &&									/*	���Է��� �Թ��Ϸ���·	*/
	   (mapBlock[mazeX][mazeY + 1]) == 0x00)								/*	���Է��� �Թ��Ϸ�δ�߹�	*/
		unCoverRoad++;														/*	��ǰ����������1			*/
		
	if((mapBlock[mazeX][mazeY] & 0x02) &&									/*	���Է��� �Թ��ҷ���·	*/
	   (mapBlock[mazeX + 1][mazeY]) == 0x00)								/*	���Է��� �Թ��ҷ�δ�߹�	*/
		unCoverRoad++;														/*	��ǰ����������1			*/
		
	if((mapBlock[mazeX][mazeY] & 0x04) &&									/*	���Է��� �Թ��·���·	*/
	   (mapBlock[mazeX][mazeY - 1]) == 0x00)								/*	���Է��� �Թ��·�δ�߹�	*/
		unCoverRoad++;														/*	��ǰ����������1			*/
		
	if((mapBlock[mazeX][mazeY] & 0x08) &&									/*	���Է��� �Թ�����·	*/
	   (mapBlock[mazeX - 1][mazeY]) == 0x00)								/*	���Է��� �Թ���δ�߹�	*/
		unCoverRoad++;														/*	��ǰ����������1			*/
	
	return unCoverRoad;														/*	���ؿ�ǰ��������		*/
}

/*********************************************************************************************************
**	����ԭ��:	void CrossWayChoice(void)
**	��������:	ѡ��һ��֧·��Ϊǰ������
**	�������:	��
**	���ز���:	��
**	�޸�ʱ��:	2014-4-16	19:57
*********************************************************************************************************/
void CrossWayChoice(void)
{
	if((mapBlock[mouseCoor.X][mouseCoor.Y] & MOUSEWAY_R) &&					/*	��������ұ���·		*/
	   (MapBlockDataGet(MOUSERIGHT) == 0x00))								/*	�������ұ�û���߹�		*/
	{
		SearchTurnRight();													/*	��������ת				*/
		return;
	}
	   
	if((mapBlock[mouseCoor.X][mouseCoor.Y] & MOUSEWAY_F) &&					/*	�������ǰ����·		*/		
	   (MapBlockDataGet(MOUSEFRONT) == 0x00))								/*	������ǰ��û���߹�	*/
	{
	   IRSensor.IsCorrectionAllowed = TRUE;
	   mouse.coorSwt = 3;
	   return;																/*	��������ת��			*/
	}
	
	if((mapBlock[mouseCoor.X][mouseCoor.Y] & MOUSEWAY_L) &&					/*	��������ת				*/
	   (MapBlockDataGet(MOUSELEFT) == 0x00))								/*	���������û���߹�		*/
	{
		SearchTurnLeft();													/*	��������ת				*/
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
**	����ԭ��:	u8 MapBlockDataGet(u8 dirTemp)
**	��������:	���ݵ��������Է���ȡ���÷������Թ����ǽ������
**	�������:	dirTemp�����������Է���
**	���ز���:	��Է����ϵ��Թ���ǽ����Ϣ
**	�޸�ʱ��:	2014-4-16	20:10
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
**	����ԭ��:	void MapStepEdit(u8 mazeX,u8 mazeY)
**	��������:	������Ŀ���Ϊ���ĵȸ�ͼ
**	�������:	Ŀ���ĺ�������
**	�������:	�������ĵȸ�ֵ
**	���ز���:	��
**	�޸�ʱ��:	2014-5-20	11:10
*********************************************************************************************************/
void MapStepEdit(u8 mazeX,u8 mazeY)
{
	MAZECOOR	crossWay[256]		= 	{0};						/*	�ȸ�ͼ��δ��֧·������	*/
	u8			dirStack[16][16]	= 	{0};						/*	���������Ľ�������	*/
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
**	����ԭ��:	void SpurtGoTo(u8 mazeX,u8 mazeY)
**	��������:	����������ֱ���˶���ָ�������
**	�������:	Ŀ���ĺ�������
**	���ز���:	�ޢ
**	�޸�ʱ��:	2014-4-16	20:10
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
**	����ԭ��:	void MouseGoStr(u8 strBlock)
**	��������:	������������ǰֱ��strBlock��
**	�������:	ǰ������
**	���ز���:	��
**	�޸�ʱ��:	2014-5-20	20:10
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
//			else if (IRSensor.CrosswayStatus[1] & 0x80)					//��һ�γ����ж�����г���	
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
//			else if (IRSensor.CrosswayStatus[1] & 0x20)					//��һ�γ����ж�����г���	
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
**	����ԭ��:	void TIM2_IRQHandler(void)
**	��������:	TIM2��ʱ�жϷ����ӳ���
**	�������:	��
**	���ز���:	��
**	�޸�ʱ��:	2014-2-18	17:50
*********************************************************************************************************/
void TIM2_IRQHandler(void)
{
	static u8 timCount = 0;
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)!=RESET)
		TIM_ClearITPendingBit(TIM2,TIM_FLAG_Update);						/*	����жϱ�־λ			*/
	leftMotor_TargetDCalculation();											/*	����Ŀ���ٶ���������	*/
	rightMotor_TargetDCalculation();										/*	�ҵ��Ŀ���ٶ���������	*/
	IRCheck();	
	LeftmotorPID();															/*	�������ƺ���			*/	
	RightmotorPID();														/*	�ҵ�����ƺ���			*/

	if ( timCount == 0 )
	{
//		DataCache();	
	}
	timCount =( timCount + 1 )% 100;
}
