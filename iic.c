#include "iic.h"
#include "delay.h"

/*
引脚说明：

PB8 -- SCL
PB9 -- SDA


*/

void Iic_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	
	GPIO_InitStruct.GPIO_Pin	= GPIO_Pin_8|GPIO_Pin_9;		//引脚8 9
	GPIO_InitStruct.GPIO_Mode	= GPIO_Mode_OUT;	//输出模式
	GPIO_InitStruct.GPIO_OType	= GPIO_OType_PP;	//推挽
	GPIO_InitStruct.GPIO_Speed	= GPIO_Speed_50MHz; //快速
	GPIO_InitStruct.GPIO_PuPd	= GPIO_PuPd_UP;     //上拉
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	//总线空闲
	SCL = 1;
	SDA_OUT = 1;
	
}

//SDA模式
void Iic_Sda_Mode(GPIOMode_TypeDef Mode)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	
	GPIO_InitStruct.GPIO_Pin	= GPIO_Pin_9;		//引脚9
	GPIO_InitStruct.GPIO_Mode	= Mode;	
	GPIO_InitStruct.GPIO_OType	= GPIO_OType_PP;	//推挽
	GPIO_InitStruct.GPIO_Speed	= GPIO_Speed_50MHz; //快速
	GPIO_InitStruct.GPIO_PuPd	= GPIO_PuPd_UP;     //上拉
	GPIO_Init(GPIOB, &GPIO_InitStruct);

}


//开始信号
void Iic_Start(void)
{
	Iic_Sda_Mode(GPIO_Mode_OUT);

	//总线空闲
	SCL = 1;
	SDA_OUT = 1;
	delay_us(5);
	
	
	SDA_OUT = 0;
	delay_us(5);
	SCL = 0;  //钳住总线
}

//停止信号
void Iic_Stop(void)
{
	Iic_Sda_Mode(GPIO_Mode_OUT);
	
	SCL = 0;
	SDA_OUT = 0;
	delay_us(5);	
	
	SCL = 1;
	delay_us(5);
	SDA_OUT = 1;
}

//发送一位数据 
void Iic_Send_Ack(u8 ack)
{
	Iic_Sda_Mode(GPIO_Mode_OUT);
	
	SCL = 0;
	//准备数据
	//要发数据1
	if(ack == 1)
	{
		SDA_OUT = 1;	 //引脚输1
	}
	//要发数据0
	if(ack == 0)
	{
		SDA_OUT = 0;	 //引脚输0
	}	
	
	delay_us(5);
	SCL = 1;
	delay_us(5);
	SCL = 0;

}

//发一个字节
void Iic_Send_Byte(u8 data)
{
	u8 i;
	
	Iic_Sda_Mode(GPIO_Mode_OUT);
	
	
	
	
	SCL = 0;
	
	for(i=0; i<8; i++)
	{
		//准备数据 如数据 0x87 1 0 0 0 0 1 1 1 
		if(data & (1<<(7-i)))  
		{
			SDA_OUT = 1;	 //引脚输1
		}
		//要发数据0
		else
		{
			SDA_OUT = 0;	 //引脚输0
		}	
		delay_us(5);
		SCL = 1;
		delay_us(5);
		SCL = 0;
	}

}

//接受一位数据
u8 Iic_Recv_Ack(void)
{
	u8 ack = 0;
	
	Iic_Sda_Mode(GPIO_Mode_IN);
	
	SCL = 0;
	delay_us(5);
	SCL = 1;
	delay_us(5);
	if(SDA_IN == 1) //判断引脚电平是否为高电平
	{
		ack = 1;
	}
	if(SDA_IN == 0) //判断引脚电平是否为低电平
	{
		ack = 0;
	}	
	
	SCL = 0;

	return ack;
}

//接受一个字节
u8 Iic_Recv_Byte(void)
{
	u8 i, data = 0;  //0 0 0 0 0 0 0 0
	
	Iic_Sda_Mode(GPIO_Mode_IN);
	
	SCL = 0;
	//循环8次，接受一个字节
	for(i=0; i<8; i++)
	{	
		delay_us(5);
		SCL = 1;
		delay_us(5);
		if(SDA_IN == 1) //判断引脚电平是否为高电平
		{
			data |= (1<<(7-i));
		}

		SCL = 0;
	}
	
	return data;
}


void AT24C02_Write(u8 addr, u8 *write_buf, u8 len)
{
	u8 ack;
	//开始信号
	Iic_Start();
	
	//发送设置地址，并执行写操作
	Iic_Send_Byte(0xA0);
	ack = Iic_Recv_Ack();
	if(ack == 1)
	{
		printf("ack failure\n");
		Iic_Stop();
		return;
	}
	
	//发送写数据的起始地址
	Iic_Send_Byte(addr);
	ack = Iic_Recv_Ack();
	if(ack == 1)
	{
		printf("ack failure\n");
		Iic_Stop();
		return;
	}	
	
	while(len--)
	{
		//发送数据
		Iic_Send_Byte(*write_buf);
		ack = Iic_Recv_Ack();
		if(ack == 1)
		{
			printf("ack failure\n");
			Iic_Stop();
			return;
		}
		//地址加1
		write_buf++;	
	}
	
	
	Iic_Stop();
	
	printf("write finish\n");

}


void AT24C02_Read(u8 addr, u8 *read_buf, u8 len)
{
	u8 ack;
	//开始信号
	Iic_Start();
	//发送设置地址，并执行写操作
	Iic_Send_Byte(0xA0);
	ack = Iic_Recv_Ack();
	if(ack == 1)
	{
		printf("ack failure\n");
		Iic_Stop();
		return;
	}
	
	//发送读数据的起始地址
	Iic_Send_Byte(addr);
	ack = Iic_Recv_Ack();
	if(ack == 1)
	{
		printf("ack failure\n");
		Iic_Stop();
		return;
	}		
	//开始信号
	Iic_Start();
	
	//发送设置地址，并执行读操作
	Iic_Send_Byte(0xA1);
	ack = Iic_Recv_Ack();
	if(ack == 1)
	{
		printf("ack failure\n");
		Iic_Stop();
		return;
	}	
	
	while(len--)  //len = 5
	{
		
		// len 4 3 2 1 0
		//接受数据
		*read_buf = Iic_Recv_Byte();
		//地址加1
		read_buf++;
		
		if(len > 0)
			Iic_Send_Ack(0);	

	}
	
	//发送非应答
	Iic_Send_Ack(1);	
	Iic_Stop();
	
	printf("read finish\n");	
	

}

