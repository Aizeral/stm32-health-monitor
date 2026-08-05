#ifndef __MY_I2C_H
#define __MY_I2C_H

typedef enum{ACK = 0, DISACK = 1} AckState;

void My_I2C_Init(void);
void MyI2C_Start(void);
void MyI2C_Stop(void);
uint8_t MyI2C_R_SDA(void);
void MyI2C_SendAck(AckState Ackbit);
uint8_t MyI2C_WaitAck(void);
void MyI2C_SendByte(uint8_t Byte);
uint8_t MyI2C_ReceiveByte(void);

#endif
