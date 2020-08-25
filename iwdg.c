#include "iwdg.h"


void Iwdg_Init(void)
{

	//1、 取消寄存器写保护：
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	//2、设置独立看门狗的预分频系数，确定时钟:125HZ
	IWDG_SetPrescaler(IWDG_Prescaler_256);
	//3、设置看门狗重装载值，确定溢出时间:2s
	IWDG_SetReload(250);
	//4、使能看门狗
	IWDG_Enable();
	
	IWDG_ReloadCounter();


}