
/*********************************************************************************************************
** 
**                               			������ͨ��ѧ                                     
**
**--------------------------------------------------------------------------------------------------------
** 	�ļ�����		ADXRS453.c
** 	����ʱ�䣺		2014-5-8 16:00
** 	������Ա�� 		�Ա���
** 	�ļ�����:  		�����ǿ���Դ�ļ�
** 
**--------------------------------------------------------------------------------------------------------
** 	����޸�ʱ�䣺	2014-5-8 16:00 
** 	����޸���Ա��	�Ա���	
** 	�汾�ţ� 	   	V1.0
** 	�汾������ 		�������Թ�����
**
*********************************************************************************************************/

#include "ADXRS453.h"
#include "Delay.h"
#include "Usart.h"

/*********************************************************************************************************
**	����ԭ��:	void GyroSPIInit(void)
**	��������:	������SPIͨ�ų�ʼ������
**	�������:	��
**	�������:	��
**	���ز���:	��
**	�޸�ʱ��:	2014-5-9	10:45
*********************************************************************************************************/
void GyroSPIInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE );	

	GPIO_InitStructure.GPIO_Pin = GYROMOSI | GYROMISO | GYROSCLK;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  										//�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GYROCS;									/*	���ҵ������˿�����	*/
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;				 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;                 		/*	��©���				*/
    GPIO_Init(GPIOA,&GPIO_InitStructure);

	GPIO_SetBits(GPIOB,GYROMOSI | GYROMISO | GYROSCLK);
	setGyroCS;
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  					//����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;											//����SPI����ģʽ:����Ϊ��SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;										//����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;												//ѡ���˴���ʱ�ӵ���̬:ʱ�����ո�
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;											//���ݲ����ڵڶ���ʱ����
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;												//NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;						//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ8
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;										//ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
	SPI_InitStructure.SPI_CRCPolynomial = 7;												//CRCֵ����Ķ���ʽ
	SPI_Init(SPI3, &SPI_InitStructure);  													//����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���

	SPI_Cmd(SPI3, ENABLE); 																	//ʹ��SPI����

//	SPI_Write(0xff,1);																		//��������	
}

/*********************************************************************************************************
**	����ԭ��:	u8 SPI_Read(u8* data,u8 bytesNumber)
**	��������:	������SPI���ݶ�ȡ����
**	�������:	*data: 			�����͵�����
				bytesNumber: 	�����������ֽ���
**	�������:	���ܵ�������data;
**	���ز���:	return 1:		���ճɹ�
				return 0:		����ʧ��
**	�޸�ʱ��:	2014-5-9	10:45
*********************************************************************************************************/
u8 SPI_Read(u8 *data,u8 bytesNumber)
{	
	u8 byte = 0;
	u8 writeBuffer[4]  = {0, 0, 0, 0};	

	for(byte = 0; byte < bytesNumber; byte++)
	{
		 writeBuffer[byte] = data[byte];
	}
	resetGyroCS;
	for(byte = 0; byte < bytesNumber; byte++)
	{
		while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET); //���ָ����SPI��־λ�������:���ͻ���ձ�־λ
		SPI_I2S_SendData(SPI3, writeBuffer[byte]); //ͨ������SPIx����һ������
		while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE) == RESET); //���ָ����SPI��־λ�������:���ܻ���ǿձ�־λ
		data[byte] = SPI_I2S_ReceiveData(SPI3); //����ͨ��SPIx������յ�����
	}
	setGyroCS;
	return bytesNumber;
}

/*********************************************************************************************************
**	����ԭ��:	u8 SPI_Write(u8* data,u8 bytesNumber)
**	��������:	������SPI���ݶ�ȡ����
**	�������:	*data: 			�����͵�����
				bytesNumber: 	�����������ֽ���
**	�������:	��
**	���ز���:	return 1:		���ͳɹ�
				return 0:		����ʧ��
**	�޸�ʱ��:	2014-5-9	10:45
*********************************************************************************************************/
u8 SPI_Write(u8 *data,u8 bytesNumber)
{		
	u8 byte = 0;
	u8 tempByte = 0;
	resetGyroCS;
	for(byte = 0; byte < bytesNumber; byte++)
	{
		while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET); //���ָ����SPI��־λ�������:���ͻ���ձ�־λ
		SPI_I2S_SendData(SPI3, data[byte]); 									//ͨ������SPIx����һ������
		while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE) == RESET); //���ָ����SPI��־λ�������:���ܻ���ǿձ�־λ
		tempByte = SPI_I2S_ReceiveData(SPI3); //����ͨ��SPIx������յ�����
	}
	setGyroCS;
	return bytesNumber;
}

/*********************************************************************************************************
**	����ԭ��:	u8 ADXRS453_ParityBit(u32 data)
**	��������:	���û������ż����λʹ��dataΪ��
**	�������:	����data
**	�������:	��
**	���ز���:	��ż����λ
**	�޸�ʱ��:	2014-5-9	10:45
*********************************************************************************************************/
u8 ADXRS453_ParityBit(u32 data)
{
	u8 parityBit = 0;
    u8 bitIndex  = 0;
    u8 sum       = 0;

    for(bitIndex = 0; bitIndex < 32; bitIndex++)
    {
        sum += ((data >> bitIndex) & 0x1);
    }
    if (! (sum % 2))
    {
        parityBit |= 0x1;
    }

    return parityBit;
}

/*********************************************************************************************************
**	����ԭ��:	u8 ADXRS453_Init(void)
**	��������:	�����ǳ�ʼ������
**	�������:	��
**	�������:	��
**	���ز���:	��ʼ��״̬
**	�޸�ʱ��:	2014-5-9	10:45
*********************************************************************************************************/
u8 ADXRS453_Init(void)
{
	u8 status  = 0x1;
	u8 data[4] = {0, 0, 0, 0};

	GyroSPIInit();
	data[3] = 3;
	data[2] = 0;
    data[1] = 0;
	data[0] = 2;
	SPI_Write(data, 4);
	Delayms(50);
	data[3] = 0;
	data[2] = 0;
    data[1] = 0;
	data[0] = 2;
	SPI_Write(data, 4);
	Delayms(50);
	data[3] = 0;
	data[2] = 0;
    data[1] = 0;
	data[0] = 2;
	SPI_Write(data, 4);
    Delayms(50);
	data[3] = 0;
	data[2] = 0;
    data[1] = 0;
	data[0] = 2;
	SPI_Write(data, 4);
    Delayms(100);
	if((ADXRS453_GetRegisterValue(ADXRS453_PID1) >> 8) != 0x52)
	{
		status = 0x0;
	}
	
	return status;	
}

/*********************************************************************************************************
**	����ԭ��:	u32 ADXRS453_Data(void)
**	��������:	��ȡ�����ǵ�����
**	�������:	��
**	�������:	��
**	���ز���:	��ȡ������������
**	�޸�ʱ��:	2014-5-9	10:45
*********************************************************************************************************/
int16_t ADXRS453_Data(void)
{
	u8 data[4]      = {0, 0, 0, 0};
	u32 receivedData = 0x00;
	data[0] = ADXRS453_SENSOR_DATA;
	SPI_Read(data, 4);
	receivedData += ((u32)data[0] << 24);
	receivedData += ((u32)data[1] << 16);
	receivedData += (data[2] << 8);
	receivedData = ((receivedData >> 10) & 0xFFFF);
	
	return receivedData;	
}

/*********************************************************************************************************
**	����ԭ��:	u16 ADXRS453_GetRegisterValue(u8 regAddress)
**	��������:	��ȡ�����ǼĴ�����ֵ
**	�������:	��
**	�������:	��
**	���ز���:	��ȡ�������ǼĴ���ֵ
**	�޸�ʱ��:	2014-5-9	10:45
*********************************************************************************************************/
u16 ADXRS453_GetRegisterValue(u8 regAddress)
{
	u8 data[4]      = {0, 0, 0, 0};
	u32 receivedData = 0x00;	
	u32 dataToSend   = 0;
	
	data[0] = ADXRS453_READ_DATA | (regAddress >> 7);
	data[1] = regAddress << 1;
	dataToSend += ((u32)data[0] << 24);
	dataToSend += ((u32)data[1] << 16);
	data[3] = ADXRS453_ParityBit(dataToSend);
	SPI_Write(data, 4);
	SPI_Read(data, 4);									// because the device answers only on the next SPI word
	receivedData += ((u32)data[1] << 16);
	receivedData += ((u32)data[2] << 8);
	receivedData += (data[3] << 0);
	receivedData = ((receivedData >> 5) & 0xFFFF);
	
	return (u16)receivedData;
}

/*********************************************************************************************************
**	����ԭ��:	void ADXRS453_SetRegisterValue(u8 regAddress, u16 regData)
**	��������:	���������ǼĴ�����ֵ
**	�������:	��
**	�������:	��
**	���ز���:	��
**	�޸�ʱ��:	2014-5-9	10:45
*********************************************************************************************************/
void ADXRS453_SetRegisterValue(u8 regAddress, u16 regData)
{
	u8 data[4]    = {0, 0, 0, 0};
	u32 dataToSend = 0;
	
	data[0] = ADXRS453_READ_DATA | (regAddress >> 7);
	data[1] = (regAddress << 1) | (regData >> 15);
	data[2] = (regData & 0x7F80) >> 7;
	data[3] = (regData & 0xFF) << 1;
	dataToSend += ((u32)data[0] << 24);
	dataToSend += ((u32)data[1] << 16);
	data[3] = ADXRS453_ParityBit(dataToSend);
	SPI_Write(data, 4);
}

/*********************************************************************************************************
**	����ԭ��:	u8 ADXRS453_GetTemperature(void)
**	��������:	���������ǵ��¶�
**	�������:	��
**	�������:	��
**	���ز���:	�¶�
**	�޸�ʱ��:	2014-5-9	10:45
*********************************************************************************************************/
u8 ADXRS453_GetTemperature(void)
{
	u16 temp = 0;
	
	temp = ADXRS453_GetRegisterValue(ADXRS453_TEMP1);
	temp >>= 6;	
	temp -= 0x31f;
	temp /= 5;
	
	return (u8)temp;
}



