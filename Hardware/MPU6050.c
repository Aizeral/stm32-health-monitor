#include "stm32f10x.h"                  // Device header
#include "My_I2C.h"
#include "MPU6050.h"
#include "Delay.h"

#define MPU6050_ADDRESS		0x68		//MPU6050的I2C从机地址

/**
  * 函    数：MPU6050写寄存器
  * 参    数：RegAddress 寄存器地址，范围：参考MPU6050手册的寄存器描述
  * 参    数：Data 要写入寄存器的数据，范围：0x00~0xFF
  * 返 回 值：无
  */
void MPU6050_WriteReg(uint8_t RegAddress, uint8_t Data)
{
	MyI2C_Start();						//I2C起始
	MyI2C_SendByte((MPU6050_ADDRESS << 1) | 0x00);	//发送从机地址，读写位为0，表示即将写入
	MyI2C_WaitAck();					//接收应答
	MyI2C_SendByte(RegAddress);			//发送寄存器地址
	MyI2C_WaitAck();					//接收应答
	MyI2C_SendByte(Data);				//发送要写入寄存器的数据
	MyI2C_WaitAck();					//接收应答
	MyI2C_Stop();						//I2C终止
}

/**
  * 函    数：MPU6050读寄存器
  * 参    数：RegAddress 寄存器地址，范围：参考MPU6050手册的寄存器描述
  * 返 回 值：读取寄存器的数据，范围：0x00~0xFF
  */
uint8_t MPU6050_ReadReg(uint8_t RegAddress)
{
	uint8_t Data;
	
	MyI2C_Start();						//I2C起始
	MyI2C_SendByte((MPU6050_ADDRESS << 1) | 0x00);	//发送从机地址，读写位为0，表示即将写入
	MyI2C_WaitAck();					//接收应答
	MyI2C_SendByte(RegAddress);			//发送寄存器地址
	MyI2C_WaitAck();					//接收应答
	
	MyI2C_Start();						//I2C重复起始
	MyI2C_SendByte((MPU6050_ADDRESS << 1) | 0x01);	//发送从机地址，读写位为1，表示即将读取
	MyI2C_WaitAck();					//接收应答
	Data = MyI2C_ReceiveByte();			//接收指定寄存器的数据
	MyI2C_SendAck(ACK);					//发送应答，给从机非应答，终止从机的数据输出
	MyI2C_Stop();						//I2C终止
	
	return Data;
}

/**
  * 函    数：MPU6050初始化
  * 参    数：无
  * 返 回 值：无
  */
void MPU6050_Init(void)
{
	MPU6050_WriteReg(MPU6050_PWR_MGMT_1, 0x80);		//复位MPU6050
	Delay_ms(100);
	MPU6050_WriteReg(MPU6050_PWR_MGMT_1, 0x01);		//电源管理寄存器1，取消睡眠模式，选择时钟源为X轴陀螺仪
	MPU6050_WriteReg(MPU6050_PWR_MGMT_2, 0x00);		//电源管理寄存器2，保持默认值0，所有轴均不待机
	MPU6050_WriteReg(MPU6050_SMPRT_DIV, 0x09);		//采样率分频寄存器，配置采样率
	MPU6050_WriteReg(MPU6050_CONFIG, 0x06);			//配置数字低通滤波器
	MPU6050_WriteReg(MPU6050_GYRO_CONFIG, 0x18);	//陀螺仪配置寄存器，选择满量程为±2000°/s
	MPU6050_WriteReg(MPU6050_ACCEL_CONFIG, 0x18);	//加速度计配置寄存器，选择满量程为±16g
	MPU6050_WriteReg(MPU6050_FIFO_EN, 0x00);		//关闭FIFO
	MPU6050_WriteReg(MPU6050_USER_CTRL, 0x00);		//关闭I2C主模式和FIFO缓冲
	Delay_ms(200);
}

/**
  * 函    数：MPU6050获取ID号
  * 参    数：无
  * 返 回 值：MPU6050的ID号
  */
uint8_t MPU6050_GetID(void)
{
	return MPU6050_ReadReg(MPU6050_WHO_AM_I);		//返回WHO_AM_I寄存器的值
}

void MPU6050_GetAccelData(int16_t* ax, int16_t* ay, int16_t* az)
{
	uint8_t DataH, DataL;								//定义数据高8位和低8位的变量

	DataH = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_H);		//读取加速度计X轴的高8位数据
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_L);		//读取加速度计X轴的低8位数据
	*ax = (DataH << 8) | DataL;						//数据拼接，通过输出参数返回

	DataH = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_H);		//读取加速度计Y轴的高8位数据
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_L);		//读取加速度计Y轴的低8位数据
	*ay = (DataH << 8) | DataL;						//数据拼接，通过输出参数返回

	DataH = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_H);		//读取加速度计Z轴的高8位数据
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_L);		//读取加速度计Z轴的低8位数据
	*az = (DataH << 8) | DataL;						//数据拼接，通过输出参数返回
}

void MPU6050_GetGyroData(int16_t* gx, int16_t* gy, int16_t* gz)
{
	uint8_t DataH, DataL;								//定义数据高8位和低8位的变量

	DataH = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_H);		//读取加速度计X轴的高8位数据
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_L);		//读取加速度计X轴的低8位数据
	*gx = (DataH << 8) | DataL;						//数据拼接，通过输出参数返回

	DataH = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_H);		//读取加速度计Y轴的高8位数据
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_L);		//读取加速度计Y轴的低8位数据
	*gy = (DataH << 8) | DataL;						//数据拼接，通过输出参数返回

	DataH = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_H);		//读取加速度计Z轴的高8位数据
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_L);		//读取加速度计Z轴的低8位数据
	*gz = (DataH << 8) | DataL;						//数据拼接，通过输出参数返回
}
