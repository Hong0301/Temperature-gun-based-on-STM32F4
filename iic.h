#ifndef __IIC_H
#define __IIC_H
#include "stm32f4xx.h"
#include "sys.h"


#define SCL  		PBout(8)
#define SDA_OUT  	PBout(9)
#define SDA_IN  	PBin(9)

void Iic_Init(void);
void AT24C02_Write(u8 addr, u8 *write_buf, u8 len);
void AT24C02_Read(u8 addr, u8 *read_buf, u8 len);

#endif