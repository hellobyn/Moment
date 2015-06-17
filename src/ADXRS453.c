
/*********************************************************************************************************
** 
**                               			北京交通大学                                     
**
**--------------------------------------------------------------------------------------------------------
** 	文件名：		ADXRS453.c
** 	创建时间：		2014-5-8 16:00
** 	创建人员： 		赵秉贤
** 	文件描述:  		陀螺仪控制源文件
** 
**--------------------------------------------------------------------------------------------------------
** 	最后修改时间：	2014-5-8 16:00 
** 	最后修改人员：	赵秉贤	
** 	版本号： 	   	V1.0
** 	版本描述： 		电脑鼠迷宫搜索
**
*********************************************************************************************************/

#include "ADXRS453.h"
#include "Delay.h"
#include "Usart.h"

/*********************************************************************************************************
**	函数原型:	void GyroSPIInit(void)
**	函数功能:	陀螺仪SPI通信初始化函数
**	输入参数:	无
**	输出参数:	无
**	返回参数:	无
**	修改时间:	2014-5-9	10:45
*********************************************************************************************************/
void GyroSPIInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE );	

	GPIO_InitStructure.GPIO_Pin = GYROMOSI | GYROMISO | GYROSCLK;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  										//复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GYROCS;									/*	左右电机方向端口配置	*/
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;				 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;                 		/*	开漏输出				*/
    GPIO_Init(GPIOA,&GPIO_InitStructure);

	GPIO_SetBits(GPIOB,GYROMOSI | GYROMISO | GYROSCLK);
	setGyroCS;
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  					//设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;											//设置SPI工作模式:设置为主SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;										//设置SPI的数据大小:SPI发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;												//选择了串行时钟的稳态:时钟悬空高
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;											//数据捕获于第二个时钟沿
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;												//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;						//定义波特率预分频的值:波特率预分频值为8
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;										//指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;												//CRC值计算的多项式
	SPI_Init(SPI3, &SPI_InitStructure);  													//根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器

	SPI_Cmd(SPI3, ENABLE); 																	//使能SPI外设

//	SPI_Write(0xff,1);																		//启动传输	
}

/*********************************************************************************************************
**	函数原型:	u8 SPI_Read(u8* data,u8 bytesNumber)
**	函数功能:	陀螺仪SPI数据读取函数
**	输入参数:	*data: 			待发送的数据
				bytesNumber: 	待发送数据字节数
**	输出参数:	接受到的数据data;
**	返回参数:	return 1:		接收成功
				return 0:		接收失败
**	修改时间:	2014-5-9	10:45
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
		while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET); //检查指定的SPI标志位设置与否:发送缓存空标志位
		SPI_I2S_SendData(SPI3, writeBuffer[byte]); //通过外设SPIx发送一个数据
		while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE) == RESET); //检查指定的SPI标志位设置与否:接受缓存非空标志位
		data[byte] = SPI_I2S_ReceiveData(SPI3); //返回通过SPIx最近接收的数据
	}
	setGyroCS;
	return bytesNumber;
}

/*********************************************************************************************************
**	函数原型:	u8 SPI_Write(u8* data,u8 bytesNumber)
**	函数功能:	陀螺仪SPI数据读取函数
**	输入参数:	*data: 			待发送的数据
				bytesNumber: 	待发送数据字节数
**	输出参数:	无
**	返回参数:	return 1:		发送成功
				return 0:		发送失败
**	修改时间:	2014-5-9	10:45
*********************************************************************************************************/
u8 SPI_Write(u8 *data,u8 bytesNumber)
{		
	u8 byte = 0;
	u8 tempByte = 0;
	resetGyroCS;
	for(byte = 0; byte < bytesNumber; byte++)
	{
		while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET); //检查指定的SPI标志位设置与否:发送缓存空标志位
		SPI_I2S_SendData(SPI3, data[byte]); 									//通过外设SPIx发送一个数据
		while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE) == RESET); //检查指定的SPI标志位设置与否:接受缓存非空标志位
		tempByte = SPI_I2S_ReceiveData(SPI3); //返回通过SPIx最近接收的数据
	}
	setGyroCS;
	return bytesNumber;
}

/*********************************************************************************************************
**	函数原型:	u8 ADXRS453_ParityBit(u32 data)
**	函数功能:	设置或清空奇偶检验位使得data为奇
**	输入参数:	数据data
**	输出参数:	无
**	返回参数:	奇偶检验位
**	修改时间:	2014-5-9	10:45
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
**	函数原型:	u8 ADXRS453_Init(void)
**	函数功能:	陀螺仪初始化函数
**	输入参数:	无
**	输出参数:	无
**	返回参数:	初始化状态
**	修改时间:	2014-5-9	10:45
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
**	函数原型:	u32 ADXRS453_Data(void)
**	函数功能:	读取陀螺仪的数据
**	输入参数:	无
**	输出参数:	无
**	返回参数:	读取的陀螺仪数据
**	修改时间:	2014-5-9	10:45
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
**	函数原型:	u16 ADXRS453_GetRegisterValue(u8 regAddress)
**	函数功能:	读取陀螺仪寄存器的值
**	输入参数:	无
**	输出参数:	无
**	返回参数:	读取的陀螺仪寄存器值
**	修改时间:	2014-5-9	10:45
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
**	函数原型:	void ADXRS453_SetRegisterValue(u8 regAddress, u16 regData)
**	函数功能:	设置陀螺仪寄存器的值
**	输入参数:	无
**	输出参数:	无
**	返回参数:	无
**	修改时间:	2014-5-9	10:45
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
**	函数原型:	u8 ADXRS453_GetTemperature(void)
**	函数功能:	读出陀螺仪的温度
**	输入参数:	无
**	输出参数:	无
**	返回参数:	温度
**	修改时间:	2014-5-9	10:45
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



