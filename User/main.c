#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "DS18B20.h"
#include "My_I2C.h"
#include "MAX30102.h"
#include "MPU6050.h"
#include "Delay.h"

#define MAX_BRIGHTNESS 255
#define INTERRUPT_REG 0X00

/* MAX30102数据缓冲区 */
uint32_t aun_ir_buffer[500];
int32_t  n_ir_buffer_length = 500;	//缓冲区长度为100，可存储以100sps运行的5秒样本
uint32_t aun_red_buffer[500];
int32_t  n_spo2;
int8_t   ch_spo2_valid;
int32_t  n_heart_rate;
int8_t   ch_hr_valid;
uint8_t  dis_hr = 0, dis_spo2 = 0;
uint32_t un_min, un_max, un_prev_data;
int i;
int32_t n_brightness;
float f_temp;
uint8_t temp[6];

uint8_t	id;

int main()
{
	DS18B20_GPIO_Config();
	My_I2C_Init();
	MAX30102_Init();
	OLED_Init();
	MPU6050_Init();

	OLED_Clear();

	un_min=0x3FFFF;
	un_max=0;

	OLED_ShowString(0, 0,  "心率:", OLED_8X16);
    OLED_ShowString(96, 0, "BPM", OLED_8X16);
    OLED_ShowString(0, 16,  "血氧:", OLED_8X16);
    OLED_ShowChar(80, 16, '%', OLED_8X16);
	
	id = MAX30102_ReadReg(MAX30102_PART_ID);
	
	OLED_ShowHexNum(0, 32, id, 2, OLED_8X16); 
	
	OLED_Update();

	for (i = 0; i < n_ir_buffer_length; i++)
    {
        while (MAX30102_INT == 1);
        MAX30102_ReadFIFO(temp, 6);
        aun_red_buffer[i] = (long)((long)((long)temp[0] & 0x03) << 16)
                          | (long)temp[1] << 8 | (long)temp[2];
        aun_ir_buffer[i]  = (long)((long)((long)temp[3] & 0x03) << 16)
                          | (long)temp[4] << 8 | (long)temp[5];

        if (un_min > aun_red_buffer[i]) un_min = aun_red_buffer[i];
        if (un_max < aun_red_buffer[i]) un_max = aun_red_buffer[i];
    }
    un_prev_data = aun_red_buffer[i - 1];

    maxim_heart_rate_and_oxygen_saturation(aun_ir_buffer, n_ir_buffer_length,
        aun_red_buffer, &n_spo2, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid);

    /* ==================== 主循环 ==================== */
    while (1)
    {
        /* ====== 滑动窗口：丢弃100旧 -> 读100新 ====== */
        un_min = 0x3FFFF;
        un_max = 0;

        for (i = 100; i < 500; i++)
        {
            aun_red_buffer[i - 100] = aun_red_buffer[i];
            aun_ir_buffer[i - 100]  = aun_ir_buffer[i];

            if (un_min > aun_red_buffer[i]) un_min = aun_red_buffer[i];
            if (un_max < aun_red_buffer[i]) un_max = aun_red_buffer[i];
        }

        for (i = 400; i < 500; i++)
        {
            un_prev_data = aun_red_buffer[i - 1];
            while (MAX30102_INT == 1);
            MAX30102_ReadFIFO(temp, 6);
            aun_red_buffer[i] = (long)((long)((long)temp[0] & 0x03) << 16)
                              | (long)temp[1] << 8 | (long)temp[2];
            aun_ir_buffer[i]  = (long)((long)((long)temp[3] & 0x03) << 16)
                              | (long)temp[4] << 8 | (long)temp[5];

            if (aun_red_buffer[i] > un_prev_data)
            {
                f_temp = aun_red_buffer[i] - un_prev_data;
                f_temp /= (un_max - un_min);
                f_temp *= 255;
                n_brightness -= (int)f_temp;
                if (n_brightness < 0) n_brightness = 0;
            }
            else
            {
                f_temp = un_prev_data - aun_red_buffer[i];
                f_temp /= (un_max - un_min);
                f_temp *= 255;
                n_brightness += (int)f_temp;
                if (n_brightness > 255) n_brightness = 255;
            }
        }

        maxim_heart_rate_and_oxygen_saturation(aun_ir_buffer, n_ir_buffer_length,
            aun_red_buffer, &n_spo2, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid);

        if (ch_hr_valid == 1 && n_heart_rate < 120)
        {
            dis_hr = n_heart_rate;
            dis_spo2 = n_spo2;
        }
        else
        {
            dis_hr = 0;
            dis_spo2 = 0;
        }
		/* ====== 显示心率血氧 ====== */
        OLED_ShowNum(48, 0, dis_hr, 3, OLED_8X16);
        OLED_ShowNum(48, 16, dis_spo2, 3, OLED_8X16);
		OLED_Update();
    }
}
