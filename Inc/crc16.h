#include "stm32f4xx_hal.h"

uint8_t CRC_OK(uint8_t* cmd);

unsigned short CRC16_MODBUS(unsigned char *puchMsg, unsigned int usDataLen);   

