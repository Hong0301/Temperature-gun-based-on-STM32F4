#include "tim.h"

/*
定时器说明

TIM3 -- APB1
定时器TIM3：84MHZ

16位定时器：值范围：0~65535

*/


void Tim3_Init(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseInitStruct;
	
	
	//1、能定时器时钟。
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	
	TIM_TimeBaseInitStruct.TIM_Prescaler    = 84-1;    //84分频 84000 000/84 = 1MHZ  1us
	TIM_TimeBaseInitStruct.TIM_Period		= 50000-1;  //计50000个数 在10000HZ下，用时1s
	TIM_TimeBaseInitStruct.TIM_CounterMode  = TIM_CounterMode_Up; //向上计数
	TIM_TimeBaseInitStruct.TIM_ClockDivision= TIM_CKD_DIV1; //分频因子
	//2、初始化定时器，配置ARR,PSC。
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);		
	
	//5、使能定时器。
	TIM_Cmd(TIM3, DISABLE);
	
}


//编写中断服务函数。这个函数不需要程序员在主函数调用，满足条件CPU自行调用的函数
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //溢出中断
	{
		/*处理的程序段*/
		GPIO_ToggleBits(GPIOE, GPIO_Pin_14);
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update); //清除中断标志位
}







