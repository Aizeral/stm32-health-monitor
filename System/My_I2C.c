#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "My_I2C.h"

/*引脚定义*/
#define MyI2C_Port         GPIOB
#define MyI2C_SCL_Pin      GPIO_Pin_6
#define MyI2C_SDA_Pin      GPIO_Pin_7

void MyI2C_W_SCL(uint8_t BitValue)
{
	GPIO_WriteBit(MyI2C_Port, MyI2C_SCL_Pin, (BitAction)BitValue);
}

void MyI2C_W_SDA(uint8_t BitValue)
{
	GPIO_WriteBit(MyI2C_Port, MyI2C_SDA_Pin, (BitAction)BitValue);
}

/*I2C引脚初始化*/
void My_I2C_Init(void)
{
    /*开启时钟*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    /*SCL引脚配置*/
    GPIO_InitTypeDef GPIO_Init_Structure;
    GPIO_Init_Structure.GPIO_Pin = MyI2C_SCL_Pin;
    GPIO_Init_Structure.GPIO_Mode = GPIO_Mode_Out_OD;   //开漏输出
    GPIO_Init_Structure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(MyI2C_Port, &GPIO_Init_Structure);

    /*SDA引脚配置*/
    GPIO_Init_Structure.GPIO_Pin = MyI2C_SDA_Pin;
    GPIO_Init(MyI2C_Port, &GPIO_Init_Structure);

    /*释放总线*/
    MyI2C_W_SCL(1);
    MyI2C_W_SDA(1);
}

/**
 * @brief   I2C起始时序
 * @param   无
 * @retval  无
 */
void MyI2C_Start(void)
{
	MyI2C_W_SDA(1);
    MyI2C_W_SCL(1);
	Delay_us(4);
    MyI2C_W_SDA(0);
	Delay_us(4);
    MyI2C_W_SCL(0);
}

/**
 * @brief   I2C停止时序
 * @param   无
 * @retval  无
 */
void MyI2C_Stop(void)
{
	MyI2C_W_SCL(0);
    MyI2C_W_SDA(0);
	Delay_us(4);
    MyI2C_W_SCL(1);
    MyI2C_W_SDA(1);
	Delay_us(4);
}

/**
 * @brief   读取SDA引脚电平
 * @param   无
 * @retval  SDA引脚电平，范围0~1
 */
uint8_t MyI2C_R_SDA(void)
{
    uint8_t BitValue;
    BitValue = GPIO_ReadInputDataBit(MyI2C_Port, MyI2C_SDA_Pin);
    return BitValue;
}

/**
 * @brief   I2C发送应答位
 * @param   Ackbit:ACK      应答为0；
 *                 DISACK   非应答为1；
 * @retval  无
 */
void MyI2C_SendAck(AckState Ackbit)
{
	MyI2C_W_SCL(0);
	MyI2C_W_SDA(Ackbit);
	Delay_us(2);
	MyI2C_W_SCL(1);
    Delay_us(2);
    MyI2C_W_SCL(0);
}

/**
 * @brief   I2C等待应答
 * @param   无
 * @retval  1   接收应答失败；
 *          0   接收应答成功；
 */
uint8_t MyI2C_WaitAck(void)
{
//    uint8_t AckBit;							//定义应答位变量
//	MyI2C_W_SDA(1);							//接收前，主机先确保释放SDA，避免干扰从机的数据发送
//	MyI2C_W_SCL(1);							//释放SCL，主机机在SCL高电平期间读取SDA
//	AckBit = MyI2C_R_SDA();					//将应答位存储到变量里
//	MyI2C_W_SCL(0);							//拉低SCL，开始下一个时序模块
//	return AckBit;							//返回定义应答位变量
	
	uint8_t ucErrTime = 0;

    MyI2C_W_SDA(1);
    Delay_us(1);
    MyI2C_W_SCL(1);	
    Delay_us(1);

    while (MyI2C_R_SDA())    /* 等待从机拉低SDA（应答）*/
    {
        ucErrTime++;
        if (ucErrTime > 250)
        {
            MyI2C_Stop();
            return 1;   /* 应答失败 */
        }
    }
    MyI2C_W_SCL(0);        /* 拉低SCL，准备下一操作 */
    return 0;           /* 应答成功 */
}

/**
 * @brief   I2C发送一个字节
 * @param   Byte:一个字节的数据
 * @retval  无
 */
void MyI2C_SendByte(uint8_t Byte)
{
    uint8_t i;
	
	MyI2C_W_SCL(0);
	for (i = 0; i < 8; i ++)				//循环8次，主机依次发送数据的每一位
	{
		/*两个!可以对数据进行两次逻辑取反，作用是把非0值统一转换为1，即：!!(0) = 0，!!(非0) = 1*/
		MyI2C_W_SDA(!!(Byte & (0x80 >> i)));//使用掩码的方式取出Byte的指定一位数据并写入到SDA线
		Delay_us(2);
		MyI2C_W_SCL(1);						//释放SCL，从机在SCL高电平期间读取SDA
		Delay_us(2);
		MyI2C_W_SCL(0);						//拉低SCL，主机开始发送下一位数据
		Delay_us(2);
	}
}

/**
 * @brief   I2C接收一个字节
 * @param   无
 * @retval  接收的一个字节的数据
 */
uint8_t MyI2C_ReceiveByte(void)
{
    uint8_t i, Byte = 0x00;					//定义接收的数据，并赋初值0x00，此处必须赋初值0x00，后面会用到
	MyI2C_W_SDA(1);							//接收前，主机先确保释放SDA，避免干扰从机的数据发送
	for (i = 0; i < 8; i ++)				//循环8次，主机依次接收数据的每一位
	{
		MyI2C_W_SCL(0);						//拉低SCL，从机在SCL低电平期间写入SDA
		Delay_us(2); 
		MyI2C_W_SCL(1);						//释放SCL，主机机在SCL高电平期间读取SDA
		if (MyI2C_R_SDA()){Byte |= (0x80 >> i);}	//读取SDA数据，并存储到Byte变量
													//当SDA为1时，置变量指定位为1，当SDA为0时，不做处理，指定位为默认的初值0
		Delay_us(1);
	}
	return Byte;							//返回接收到的一个字节数据
}
