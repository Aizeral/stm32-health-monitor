#ifndef __MAX30102_H
#define __MAX30102_H

/*--------------------------------------------------------------------------------------------------------------*/
/*引脚定义*/
#define MAX30102_INT_CLK            RCC_APB2Periph_GPIOA
#define MAX30102_INT_Port           GPIOA
#define MAX30102_INT_Pin            GPIO_Pin_7
/*--------------------------------------------------------------------------------------------------------------*/

#define MAX30102_INT                GPIO_ReadInputDataBit(MAX30102_INT_Port, MAX30102_INT_Pin)

#define MAX30102_WR_ADDR            0xAE
#define MAX30102_RD_ADDR            0xAF

#define MAX30102_INT_STATUS1        0x00
#define MAX30102_INT_STATUS2        0x01
#define MAX30102_INT_ENABLE1        0x02
#define MAX30102_INT_ENABLE2        0x03
#define MAX30102_FIFO_WR_PTR        0x04
#define MAX30102_FIFO_OVF_CNT       0x05
#define MAX30102_FIFO_RD_PTR        0x06
#define MAX30102_FIFO_DATA          0x07
#define MAX30102_FIFO_CONFIG        0x08
#define MAX30102_MODE_CONFIG        0x09
#define MAX30102_PILOT_PA 			0x10
#define MAX30102_SPO2_CONFIG        0x0A
#define MAX30102_LED1_PA            0x0C
#define MAX30102_LED2_PA            0x0D
#define MAX30102_TEMP_INTG          0x1F
#define MAX30102_TEMP_FRAC          0x20
#define MAX30102_TEMP_CONFIG        0x21
#define MAX30102_REV_ID             0xFE
#define MAX30102_PART_ID            0xFF

uint8_t MAX30102_WriteReg(uint8_t RegAddr, uint8_t Data);
uint8_t MAX30102_ReadReg(uint8_t RegAddr);
void MAX30102_ReadFIFO(uint8_t *buf, uint8_t len);
void MAX30102_Init(void);

//官方心率血氧算法所有函数
void maxim_heart_rate_and_oxygen_saturation(uint32_t *pun_ir_buffer ,  int32_t n_ir_buffer_length, uint32_t *pun_red_buffer ,   int32_t *pn_spo2, int8_t *pch_spo2_valid ,  int32_t *pn_heart_rate , int8_t  *pch_hr_valid);
void maxim_find_peaks( int32_t *pn_locs, int32_t *pn_npks,  int32_t *pn_x, int32_t n_size, int32_t n_min_height, int32_t n_min_distance, int32_t n_max_num );
void maxim_peaks_above_min_height( int32_t *pn_locs, int32_t *pn_npks,  int32_t *pn_x, int32_t n_size, int32_t n_min_height );
void maxim_remove_close_peaks( int32_t *pn_locs, int32_t *pn_npks,   int32_t  *pn_x, int32_t n_min_distance );
void maxim_sort_ascend( int32_t *pn_x, int32_t n_size );
void maxim_sort_indices_descend(  int32_t  *pn_x, int32_t *pn_indx, int32_t n_size);

#endif
