#ifndef __DS18B20_H
#define __DS18B20_H

/*DS18B20指令码*/
#define DS18B20_Search_ROM          0xF0
#define DS18B20_READ_ROM            0x33
#define DS18B20_MATH_ROM            0x55
#define DS18B20_SKIP_ROM            0xCC
#define DS18B20_ALARM_SEARCH        0xEC
#define DS18B20_CONVERT_T           0x44
#define DS18B20_WRITE_SCRATCHPAD    0x4E
#define DS18B20_READ_SCRATCHPAD     0xBE
#define DS18B20_COPY_SCRATCHPAD     0x48
#define DS18B20_RECALL_E2           0xB8
#define DS18B20_READ_POWER_SUPPLY   0xB4

void DS18B20_GPIO_Config(void);
uint8_t DS18B20_Init(void);
uint8_t DS18B20_Read(void);
void DS18B20_Write(uint8_t WriteData);
float DS18B20_GetTemp(void);

#endif
