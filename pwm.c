#include "pwm.h"

/*
引脚说明：

LED0灯连接PF9,TIM14通道1

TIM14 -- APB1,定时器频率：84MHZ


*/

void Pwm_Led0_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;
	
	TIM_OCInitTypeDef  TIM_OCInitStruct; 	//定时一个该结构体
	
	
	//开启 TIM14 和 和 GPIO时钟，配置 PF9选择复用功能 AF9 （TIM14）输出。
	//库函数使能 TIM14 时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14,ENABLE);
	//还需要使用到GPIOF9，所以也要使能该引脚 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE); 
	
	
	//由于使用到PF9的复用功能，利用库函数使其引脚与TIM14定时器
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource9,GPIO_AF_TIM14); 
	
	
	GPIO_InitStruct.GPIO_Pin 	= GPIO_Pin_9; 		//GPIOF9
	GPIO_InitStruct.GPIO_Mode 	= GPIO_Mode_AF; 	//配置引脚为复用功能
	GPIO_InitStruct.GPIO_Speed 	= GPIO_Speed_50MHz; //速度 50MHz
	GPIO_InitStruct.GPIO_OType 	= GPIO_OType_PP; 	//推挽复用输出
	GPIO_InitStruct.GPIO_PuPd 	= GPIO_PuPd_UP; 	//上拉
	GPIO_Init(GPIOF,&GPIO_InitStruct); 	//初始化 PF9，用库函数寄存器


	TIM_TimeBaseStruct.TIM_Prescaler	= 84-1 ; 	//设置定时器的分频值 1MHZ
	TIM_TimeBaseStruct.TIM_Period		= 500-1; 	//设置定时器的的自动重装载的值 周期为500us
	TIM_TimeBaseStruct.TIM_CounterMode	= TIM_CounterMode_Up;//设置定时器为向上计数模式
	TIM_TimeBaseStruct.TIM_ClockDivision= TIM_CKD_DIV1;//设置定时器的时钟分频因子
	TIM_TimeBaseInit(TIM14,&TIM_TimeBaseStruct);//初始化定时器 14
	

	
	TIM_OCInitStruct.TIM_OCMode			= TIM_OCMode_PWM1; 		//选择PWM的模式，选择PWM模式1
	TIM_OCInitStruct.TIM_OCPolarity 	= TIM_OCPolarity_Low; 	//输出的极性，输出是高电平还是低电平，这里选择低电平
	TIM_OCInitStruct.TIM_OutputState 	= TIM_OutputState_Enable; //用于设置输出的使能，使能PWM输出到端口
	//根据设定信息配置TIM14 OC1 -- 通道1
	TIM_OC1Init(TIM14, &TIM_OCInitStruct); 
	
	//使能预装载寄存器：
	TIM_OC1PreloadConfig(TIM14, TIM_OCPreload_Enable); 
	//使能自动重装载的预装载寄存器允许位	
	TIM_ARRPreloadConfig(TIM14,ENABLE);

	//使能定时器。
	TIM_Cmd(TIM14, ENABLE);

}

/*
引脚说明：

电机控制信号连接PB10,TIM2通道3
PB11 TIM2通道4
TIM2 -- APB1,定时器频率：84MHZ
TIM_SetCompare3(TIM2, 0);
TIM_SetCompare4(TIM2, 0);

*/
void Pwm_Motor_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;         	  //GPIO的一个结构体
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;   //定时器结构体
		
	TIM_OCInitTypeDef  TIM_OCInitStruct; 					//定时一个该结构体
	
	
	//开启 TIM4 和 和 GPIO时钟，配置 PF9选择复用功能 AF9 （TIM14）输出。
	//库函数使能 TIM4 时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	//还需要使用到GPIOD13，所以也要使能该引脚 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); 
	
	
	//由于使用到PF9的复用功能，利用库函数使其引脚与TIM4定时器
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_TIM2); 
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_TIM2);
	
	GPIO_InitStruct.GPIO_Pin 	= GPIO_Pin_10 | GPIO_Pin_11; 		  //设置为GPIOD13引脚
	GPIO_InitStruct.GPIO_Mode 	= GPIO_Mode_AF; 	//配置引脚为复用功能
	GPIO_InitStruct.GPIO_Speed 	= GPIO_Speed_50MHz; //速度 50MHz
	GPIO_InitStruct.GPIO_OType 	= GPIO_OType_PP; 	//推挽复用输出
	GPIO_InitStruct.GPIO_PuPd 	= GPIO_PuPd_UP; 	//上拉
	GPIO_Init(GPIOB,&GPIO_InitStruct); 	          //初始化 PF9，用库函数寄存器


	TIM_TimeBaseStruct.TIM_Prescaler	= 84-1 ; 		//设置定时器的分频值 1MHZ
	TIM_TimeBaseStruct.TIM_Period		= 500-1; 	  	//设置定时器的的自动重装载的值 周期为500us
	TIM_TimeBaseStruct.TIM_CounterMode	= TIM_CounterMode_Up;//设置定时器为向上计数模式
	TIM_TimeBaseStruct.TIM_ClockDivision= TIM_CKD_DIV1;//设置定时器的时钟分频因子
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStruct);	//初始化定时器 4
	

	
	TIM_OCInitStruct.TIM_OCMode			= TIM_OCMode_PWM1; 		//选择PWM的模式，选择PWM模式1
	TIM_OCInitStruct.TIM_OCPolarity 	= TIM_OCPolarity_Low; 	//输出的极性，输出是高电平还是低电平，这里选择低电平
	TIM_OCInitStruct.TIM_OutputState 	= TIM_OutputState_Enable; //用于设置输出的使能，使能PWM输出到端口
	//根据设定信息配置TIM2 OC3 -- 通道1
	TIM_OC3Init(TIM2, &TIM_OCInitStruct); 
	TIM_OC4Init(TIM2, &TIM_OCInitStruct); 
	
	//使能预装载寄存器：
	TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable); 
	TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable); 
	//使能自动重装载的预装载寄存器允许位	
	TIM_ARRPreloadConfig(TIM2,ENABLE);

	//使能定时器。
	TIM_Cmd(TIM2, ENABLE);

}

