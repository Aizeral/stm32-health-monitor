#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "DS18B20.h"

/*--------------------------------------------------------------------------------------------------------------*/
/*引脚定义*/
#define DS18B20_DQ_CLK      RCC_APB2Periph_GPIOA
#define DS18B20_DQ_Port     GPIOA
#define DS18B20_DQ_Pin      GPIO_Pin_0
/*--------------------------------------------------------------------------------------------------------------*/

/*IO口配置*/
void DS18B20_GPIO_Config(void)
{
    /*开启时钟*/
    RCC_APB2PeriphClockCmd(DS18B20_DQ_CLK, ENABLE);

    /*配置引脚为开漏输出*/
    GPIO_InitTypeDef GPIO_Init_Structure;

    GPIO_Init_Structure.GPIO_Pin = DS18B20_DQ_Pin;
    GPIO_Init_Structure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_Init_Structure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(DS18B20_DQ_Port, &GPIO_Init_Structure);
}

/**
 * @brief   DS18B20初始化
 * @param   无
 * @retval  0   初始化成功；
 *          1   初始化失败；
 */
uint8_t DS18B20_Init(void)
{
    /*初始化标志，0成功，1失败*/
    uint8_t InitFlag;

    /*初始化*/
    /*空闲时高电平*/
    GPIO_SetBits(DS18B20_DQ_Port, DS18B20_DQ_Pin);
    Delay_us(10);

    /*拉低电平*/
    GPIO_ResetBits(DS18B20_DQ_Port, DS18B20_DQ_Pin);
    Delay_us(500);

    /*释放总线*/
    GPIO_SetBits(DS18B20_DQ_Port, DS18B20_DQ_Pin);
    Delay_us(60);   //等待应答

    InitFlag = GPIO_ReadInputDataBit(DS18B20_DQ_Port, DS18B20_DQ_Pin);
    Delay_us(240);  //等待应答结束

    return InitFlag;
}

/**
 * @brief   读取DS18B20的数据
 * @param   无
 * @retval  DS18B20的8位数据
 */
uint8_t DS18B20_Read(void)
{
    uint8_t Data = 0, i;

    /*空闲时高电平*/
    GPIO_SetBits(DS18B20_DQ_Port, DS18B20_DQ_Pin);
    Delay_us(10);

    /*一次读一位，循环8次拼接数据*/
    for(i = 0; i < 8; i++)
    {
        /*拉低电平*/
        GPIO_ResetBits(DS18B20_DQ_Port, DS18B20_DQ_Pin);
        Delay_us(1);

        /*释放总线*/
        GPIO_SetBits(DS18B20_DQ_Port, DS18B20_DQ_Pin);
        Delay_us(10);       //等待10us后采样

        Data >>= 1;         //右移一位，最高位置0接收数据
        
        /*写入数据*/
        if(GPIO_ReadInputDataBit(DS18B20_DQ_Port,DS18B20_DQ_Pin) == 1)
        {
            Data |= 0x80;   //读1，最高位置1
        }
        Delay_us(45);       //等待结束
    }
    return Data;
}

/**
 * @brief   向DS18B20写入数据
 * @param   WriteData:DS18B20指令码
 * @retval  无
 */
void DS18B20_Write(uint8_t WriteData)
{
    uint8_t i;

    /*空闲时高电平*/
    GPIO_SetBits(DS18B20_DQ_Port, DS18B20_DQ_Pin);
    Delay_us(10);

    for(i = 0; i < 8; i++)
    {
        /*拉低电平*/
        GPIO_ResetBits(DS18B20_DQ_Port, DS18B20_DQ_Pin);
        Delay_us(5);
        
        /*读取写入数据最低位，读1拉高电平，读0拉低电平*/
        if(WriteData & 0x01)
        {
            GPIO_SetBits(DS18B20_DQ_Port, DS18B20_DQ_Pin);
        }
        else
        {
            GPIO_ResetBits(DS18B20_DQ_Port, DS18B20_DQ_Pin);
        }
        Delay_us(60);       //等待结束

        /*释放总线*/
        GPIO_SetBits(DS18B20_DQ_Port, DS18B20_DQ_Pin);
        Delay_us(1);

        WriteData >>= 1;    //右移进行下一位写入
    }
}

/**
 * @brief   获取DS18B20温度
 * @param   无
 * @retval  温度值
 */
float DS18B20_GetTemp(void)
{
    uint8_t TL, TH;
    int16_t Temp;
    float Temperature;

    /*启动温度转换*/
    DS18B20_Init();                     //复位
    DS18B20_Write(DS18B20_SKIP_ROM);
    DS18B20_Write(DS18B20_CONVERT_T);   //启动温度转换    

    /*等待温度转换*/
    Delay_us(500);

    /*读取温度*/
    DS18B20_Init();
    DS18B20_Write(DS18B20_SKIP_ROM);
    DS18B20_Write(DS18B20_READ_SCRATCHPAD);
    TL = DS18B20_Read();
    TH = DS18B20_Read();

    Temp = (TH << 8) | TL;
    Temperature = Temp * 0.0625;
    return Temperature;
}
