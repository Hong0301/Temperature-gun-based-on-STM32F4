#include "exti.h"
#include "mlx90614.h"
u8 flag=1;
u8 on_in=1;
/*
引脚：PA0：作为外部中断输入

*/

//初始化PA0
void Gpio_A_Pin0_Init(void)
{
	GPIO_InitTypeDef GPIOA_InitStruct;
	
	//开启GPIOA时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	GPIOA_InitStruct.GPIO_Pin  = GPIO_Pin_0;	//端口位PA0
	GPIOA_InitStruct.GPIO_Mode = GPIO_Mode_IN;	//输入模式
	GPIOA_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;	//上拉
	
	//初始化
	GPIO_Init(GPIOA, &GPIOA_InitStruct);
	
}

u8 PWR_Check_Standby(void)
{
      u8 t = 0;
      u8 tx = 0;
      while(1)
      {
					if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 0) //检测到按键按下
          {
									t++;
                  tx = 0;
					}
          else
          {
									tx++;
                  if(tx > 3)
                  {
                        
                        return 0;      //错误按键或者按键时间不够
                  }                        
           }
           delay_ms(30);
           if(t > 100)       //100*30ms = 3s
           {
                   return 1;  // 3s
						}
    }
}

//初始化外部中断Exti0
void Exti0_Init(void)
{
	EXTI_InitTypeDef EXTI0_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	//开启系统时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	//初始化PA0端口位输入
	Gpio_A_Pin0_Init();
	
	//映射PA0为EXTI0
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);
	
	EXTI0_InitStruct.EXTI_Line 	  = EXTI_Line0;				//外部中断线EXTI0
	EXTI0_InitStruct.EXTI_Mode 	  = EXTI_Mode_Interrupt;	//中断屏蔽
	EXTI0_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;	//上升沿触发
	EXTI0_InitStruct.EXTI_LineCmd = ENABLE;					//使能
	
	//初始化EXTI0
	EXTI_Init(&EXTI0_InitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannel					  = EXTI0_IRQn;	//选择外部中断线
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;		//设置抢占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority 		  = 0x01;		//响应优先级
	NVIC_InitStruct.NVIC_IRQChannelCmd 				  = ENABLE;		//使能
	
	//初始化NVIC_InitStruct
	NVIC_Init(&NVIC_InitStruct);
	
}


//外部中断Exti0响应函数
void EXTI0_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line0) == SET)	//获取EXTI0的状态
	{

		if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == Bit_RESET)
		{
				delay_ms(15); //延时消抖
			
				//判断是否按下
				if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 0)
				{
					on_in=0;
					//按键按下,蜂鸣器不响
					GPIO_ResetBits(GPIOF, GPIO_Pin_8);		
					// K1 键长按进入待机模式
					if(PWR_Check_Standby())  //PWR_Check_Standby()函数是用来判断按键时长的，如果按键时间长超过3S就返回1，否则返回0
					{
								//使能WKUP引脚的唤醒功能
								PWR_WakeUpPinCmd (ENABLE);

								//进入待机模式
								PWR_EnterSTANDBYMode();
					}
			
				}	
			//清空标志位
			EXTI_ClearITPendingBit(EXTI_Line0);		
		}		
			
	}
}


/*
引脚：PE2：作为外部中断输入
			Exti2

*/

//初始化PE2
void Gpio_E_Pin2_Init(void)
{
	GPIO_InitTypeDef GPIOE_InitStruct;
	
	//开启GPIOE时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	
	GPIOE_InitStruct.GPIO_Pin  = GPIO_Pin_2;	//端口位PE2
	GPIOE_InitStruct.GPIO_Mode = GPIO_Mode_IN;	//输入模式
	GPIOE_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;	//上拉
	
	//初始化PE2
	GPIO_Init(GPIOE, &GPIOE_InitStruct);
	
}

//初始化外部中断Exti2
void Exti2_Init(void)
{
	EXTI_InitTypeDef EXTI2_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	//开启系统时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	//初始化PE2	
	Gpio_E_Pin2_Init();
	
	//映射中断Exti2
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource2);
	
	EXTI2_InitStruct.EXTI_Line 	  = EXTI_Line2;				//外部中断线EXTI
	EXTI2_InitStruct.EXTI_Mode 	  = EXTI_Mode_Interrupt;	//中断屏蔽
	EXTI2_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;	//下降延触发
	EXTI2_InitStruct.EXTI_LineCmd = ENABLE;					//使能
	
	//初始化EXTI2
	EXTI_Init(&EXTI2_InitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannel 					= EXTI2_IRQn;   //选择外部中断线
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority 	= 0x01;			//设置抢占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority 			= 0x01;			//响应优先级
	NVIC_InitStruct.NVIC_IRQChannelCmd 					= ENABLE;		//使能
	
	//初始化NVIC_InitStruct
	NVIC_Init(&NVIC_InitStruct);
	
}

//外部中断Exti2响应函数
void EXTI2_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line2) == SET)	//获取EXTI2的状态
	{
		
		if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_2) == Bit_RESET)
		{
				delay_ms(15); //延时消抖
			
				//判断是否按下
				if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_2) == 0)
				{
								if(flag == 1)
											flag=0;
								else
											flag=1;
								OLED_CLS();//清屏	
								//红外测温
								SMBus_Init();									//红外传感器模块初始化
								//temp=SMBus_ReadTemp();
								char dstr[20];
								sprintf(dstr,"Tmp:%.2f C",SMBus_ReadTemp()); //浮点型转字符型	
								OLED_ShowStr(0,3,(u8 *)dstr,2);
								if(SMBus_ReadTemp() > 37.3)							//报警
								{
										on_in=1;
										if(on_in ==1)
										{
												GPIO_SetBits(GPIOF, GPIO_Pin_8); //蜂鸣器响	
										}	
											 	
								}	
								delay_s(3);
								
								OLED_CLS();//清屏	
														
				}	
			//清空标志位
			EXTI_ClearITPendingBit(EXTI_Line2);		
		}
		
	}
}



/*
引脚：PE3：作为外部中断输入
			Exti3

*/

//初始化PE3
void Gpio_E_Pin3_Init(void)
{
	GPIO_InitTypeDef GPIOE_InitStruct;
	
	//开启GPIOE时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	
	GPIOE_InitStruct.GPIO_Pin  = GPIO_Pin_3;		//选择PE3
	GPIOE_InitStruct.GPIO_Mode = GPIO_Mode_IN;		//设置为输入
	GPIOE_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;		//上拉
	
	//初始化GPIOE
	GPIO_Init(GPIOE, &GPIOE_InitStruct);
	
}

//初始化外部中断Exti2
void Exti3_Init(void)
{
	EXTI_InitTypeDef EXTI3_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	//开启时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	//初始化端口PE3
	Gpio_E_Pin3_Init();
	
	//映射端口EXTI3
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource3);
	
	EXTI3_InitStruct.EXTI_Line    = EXTI_Line3;				//设置外部中断线
	EXTI3_InitStruct.EXTI_Mode    = EXTI_Mode_Interrupt;	//外部中断
	EXTI3_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;	//下降沿触发
	EXTI3_InitStruct.EXTI_LineCmd = ENABLE;					//使能
	
	//初始化EXTI3
	EXTI_Init(&EXTI3_InitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannel 					= EXTI3_IRQn;	//选择外部中断线
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority 	= 0x01;			//设置抢占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority 			= 0x01;			//响应优先级
	NVIC_InitStruct.NVIC_IRQChannelCmd 					= ENABLE;		//使能
	
	//初始化初始化NVIC_InitStruct
	NVIC_Init(&NVIC_InitStruct);
	
}

//外部中断Exti0响应函数
void EXTI3_IRQHandler(void)
{
	char RTC_time[128];
	if(EXTI_GetITStatus(EXTI_Line3) == SET)	//获取EXTI3的状态
	{
		
		if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_3) == Bit_RESET)
		{
				delay_ms(15); //延时消抖
			
				//判断是否按下
				if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_3) == 0)
				{
						if(flag==1)
						{	
								//按键按下 小时++
								RTC_TimeTypeDef RTC_TimeStruct;
								RTC_GetTime(RTC_Format_BIN, &RTC_TimeStruct);
								RTC_TimeStruct.RTC_Hours++;
								if(RTC_TimeStruct.RTC_Hours == 24)
												RTC_TimeStruct.RTC_Hours=0;
								RTC_SetTime(RTC_Format_BIN, &RTC_TimeStruct);
						}
						if(flag==0)
						{	
								//按键按下 小时--
								RTC_TimeTypeDef RTC_TimeStruct;
								RTC_GetTime(RTC_Format_BIN, &RTC_TimeStruct);
								RTC_TimeStruct.RTC_Hours--;
								if(RTC_TimeStruct.RTC_Hours < 0)
												RTC_TimeStruct.RTC_Hours= 23;
								RTC_SetTime(RTC_Format_BIN, &RTC_TimeStruct);
						}
							
				}	
			//清空标志位
			EXTI_ClearITPendingBit(EXTI_Line3);		
		}		
		
	}
}



/*
引脚：PE4：作为外部中断输入
			Exti4

*/

//初始化PE4
void Gpio_E_Pin4_Init(void)
{
	GPIO_InitTypeDef GPIOE_InitStruct;
	
	//开启GPIOE时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	
	GPIOE_InitStruct.GPIO_Pin  = GPIO_Pin_4;	//选择PE4端口
	GPIOE_InitStruct.GPIO_Mode = GPIO_Mode_IN;	//输入模式
	GPIOE_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;	//上拉
	
	//初始化GPIOE
	GPIO_Init(GPIOE, &GPIOE_InitStruct);
	
}

//初始化外部中断Exti2
void Exti4_Init(void)
{
	EXTI_InitTypeDef EXTI4_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	//开启系统时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	//初始化PE4
	Gpio_E_Pin4_Init();
	
	//映射端口EXTI4
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource4);
	
	EXTI4_InitStruct.EXTI_Line 	  = EXTI_Line4;				//设置外部中断线
	EXTI4_InitStruct.EXTI_Mode 	  = EXTI_Mode_Interrupt;	//外部中断
	EXTI4_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;	//下降沿触发
	EXTI4_InitStruct.EXTI_LineCmd = ENABLE;					//使能
	
	//初始化EXTI4
	EXTI_Init(&EXTI4_InitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannel 				  	= EXTI4_IRQn;	//选择外部中断线
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority 	= 0x01;			//设置抢占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority 			= 0x01;			//响应优先级
	NVIC_InitStruct.NVIC_IRQChannelCmd					= ENABLE;		//使能
	
	//初始化初始化NVIC_InitStruct
	NVIC_Init(&NVIC_InitStruct);
	
}

//外部中断Exti4响应函数
void EXTI4_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line4) == SET)	//获取EXTI4的状态
	{
		
		if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4) == Bit_RESET)
		{
				delay_ms(15); //延时消抖
			
				//判断是否按下
				if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4) == 0)
				{
						if(flag==1)
						{
									//按键按下  分++
						RTC_TimeTypeDef RTC_TimeStruct;
						RTC_GetTime(RTC_Format_BIN, &RTC_TimeStruct);
						RTC_TimeStruct.RTC_Minutes++;
						if(RTC_TimeStruct.RTC_Minutes ==60)
									RTC_TimeStruct.RTC_Minutes=0;
						RTC_SetTime(RTC_Format_BIN, &RTC_TimeStruct);
						}
						if(flag==0)
						{
									//按键按下  分--
						RTC_TimeTypeDef RTC_TimeStruct;
						RTC_GetTime(RTC_Format_BIN, &RTC_TimeStruct);
						RTC_TimeStruct.RTC_Minutes--;
						if(RTC_TimeStruct.RTC_Minutes < 0)
									RTC_TimeStruct.RTC_Minutes=59;
						RTC_SetTime(RTC_Format_BIN, &RTC_TimeStruct);
						}
					
				}	
			//清空标志位
			EXTI_ClearITPendingBit(EXTI_Line4);		
		}		
		
	}
}