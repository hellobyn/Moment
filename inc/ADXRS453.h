
/********************************************************************************************************
** 
**                               			������ͨ��ѧ                                     
**
**-------------------------------------------------------------------------------------------------------
** 	�ļ�����		ADXRS453.h
** 	����ʱ�䣺		2014-5-9 10:35
** 	������Ա�� 		�Ա���
** 	�ļ�����:  		�����ǿ���ͷ�ļ�
** 
**-------------------------------------------------------------------------------------------------------
** 	����޸�ʱ�䣺	2014-5-9 10:35 
** 	����޸���Ա��	�Ա���	
** 	�汾�ţ� 	   	V1.0
** 	�汾������ 		�Թ�����
**
*********************************************************************************************************/

#include "stm32f10x_gpio.h"
#include "stm32f10x_spi.h"

/*********************************************************************************************************
**	�����ǽӿڶ���
*********************************************************************************************************/
#define	GYROMOSI	 		GPIO_Pin_5							/*	������	SPI3_MOSI	PB5				*/
#define	GYROMISO	 		GPIO_Pin_4							/*	������	SPI3_MISO	PB4				*/
#define	GYROSCLK		 	GPIO_Pin_3							/*	������	SPI3_SCLK	PB3				*/
#define	GYROCS		 		GPIO_Pin_15							/*	������	SPI3_CS		PA15			*/

#define	resetGyroCS			GPIOA->BRR  = GPIO_Pin_15			/*	������ʹ�ܶ�	�õ�				*/
#define	setGyroCS			GPIOA->BSRR = GPIO_Pin_15			/*	������ʹ�ܶ�	�ø�				*/

/*********************************************************************************************************
**	�����ǿ������ݶ���
*********************************************************************************************************/
#define ADXRS453_STARTUP_DELAY	0.05 /* sec */

/* The MSB for the spi commands */
#define ADXRS453_SENSOR_DATA	0x20
#define ADXRS453_WRITE_DATA		0x40
#define ADXRS453_READ_DATA		0x80

/* Memory register map */
#define ADXRS453_RATE1			0x00	// Rate Registers
#define ADXRS453_TEMP1			0x02	// Temperature Registers
#define ADXRS453_LOCST1			0x04	// Low CST Memory Registers
#define ADXRS453_HICST1			0x06	// High CST Memory Registers
#define ADXRS453_QUAD1			0x08	// Quad Memory Registers
#define ADXRS453_FAULT1			0x0A	// Fault Registers
#define ADXRS453_PID1			0x0C	// Part ID Register 1
#define ADXRS453_SNH			0x0E	// Serial Number Registers, 4 bytes
#define ADXRS453_SNL			0x10

/* Check bits */
#define ADXRS453_P				0x01	// Parity bit
#define ADXRS453_CHK			0x02	
#define ADXRS453_CST			0x04
#define ADXRS453_PWR			0x08
#define ADXRS453_POR			0x10
#define ADXRS453_NVM			0x20
#define ADXRS453_Q				0x40
#define ADXRS453_PLL			0x80
#define ADXRS453_UV				0x100
#define ADXRS453_OV				0x200
#define ADXRS453_AMP			0x400
#define ADXRS453_FAIL			0x800

#define ADXRS453_WRERR_MASK		(0x7 << 29)
#define ADXRS453_GET_ST(a)		((a >> 26) & 0x3)  // Status bits



/*********************************************************************************************************
**	�����ǿ��ƺ�������
*********************************************************************************************************/
void GyroSPIInit(void);
u8 SPI_Read(u8* data,u8 bytesNumber);
u8 SPI_Write(u8 *data,u8 bytesNumber);
u8 ADXRS453_ParityBit(u32 data);
u8 ADXRS453_Init(void);
int16_t ADXRS453_Data(void);
u16 ADXRS453_GetRegisterValue(u8 regAddress);
void ADXRS453_SetRegisterValue(u8 regAddress, u16 regData);
u8 ADXRS453_GetTemperature(void);
