#include "stm32f4xx.h"
#include "led.h"
#include "key.h"
#include "exti.h"
#include "delay.h"
#include "tim.h"
#include "pwm.h"
#include "usart.h"
#include "string.h"
#include "sys.h"
#include "stdio.h"
#include "iwdg.h"
#include "rtc.h"
#include "iic.h"
#include "OLED_I2C.h"
#include "mlx90614.h"
#include "dht11.h"
#include <stdlib.h>

#define LED0_ON    GPIO_ResetBits(GPIOF, GPIO_Pin_9)
#define LED0_OFF   GPIO_SetBits(GPIOF, GPIO_Pin_9)



u8 Usart_Data;   //值范围：0~255
u8 rx_flag = 0;  //接受数据完成 rx_flag = 1
u8 buffer[64] = {0};
u8 rx_buffer[64] = {0};
u8 rx_i=0, count = 0;



void USART1_IRQHandler(void)
{
	
	 
   //若是非空，则返回值为1，与RESET（0）判断，不相等则判断为真
   if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
   {	
		//判断为真后，为下次中断做准备，则需要对中断的标志清零
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
		/* DR读取接受到的数据*/
		buffer[count++] = USART_ReceiveData(USART1);	
	   
	   if(buffer[count-1] == ':')
	   {
		   //去除:
			for(rx_i=0; rx_i< (count-1); rx_i++)
			{
				rx_buffer[rx_i] = buffer[rx_i];
			}
	   
			memset(buffer, 0, sizeof(buffer));
			count = 0;  //置为0,下一帧数据从buffer[0]开始存储
			rx_flag = 1; //接受数据完成 rx_flag = 1
	   }
	   
	
   }

}


int Get_SR04_Value(void)
{

	
	u32 echo_time, t = 0;

	//给trig上拉超过10us高电平
	PAout(2) = 0;
	delay_us(5);
	PAout(2) = 1;
	delay_us(15);
	PAout(2) = 0;
	
	//设置定时器CNT值为0
	TIM_SetCounter(TIM3, 0);
	//TIM3->CNT;
	
	//等待echo高电平  做超时等待
	while(PAin(3) == 0)
	{
		t++;
		delay_us(2);
		if( t >= 1000) //2ms
			return -1;
	
	}
	//开定时器
	TIM_Cmd(TIM3, ENABLE);
	//计数直到高电平结束
	
	//做超时等待
	t = 0; 
	while(PAin(3) == 1)
	{
		t++;
		delay_us(2);
		if( t >= 11500) //23.2ms
		{
			TIM_Cmd(TIM3, DISABLE);
			return -2;	
		}				
	}
	//关定时器
	TIM_Cmd(TIM3, DISABLE);

	//读取CNT值
	echo_time = TIM_GetCounter(TIM3);
	//延时 避免干扰
	delay_s(1);
	return echo_time/58.0;
}


void init_func(void)
{
			//NVIC分组（一个工程当中只能配置一次分组）抢占优先级2位,值范围：0~3；响应优先级2位,值范围：0~3；
			NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
			Delay_Init();                      //延时
			Led_Init();                       //LED灯初始化及蜂鸣器初始化	
			Usart1_Init();										//串口
			Pwm_Motor_Init();									//电机
			
			I2C_Configuration();
			OLED_Init();                       //OLED初始化
			SMBus_Init();                      //红外传感器模块初始化
			Rtc_Init();                        //实时时钟初始化
			Dht11_Init();                      //温湿度模块初始化
			
			//中断线
			Exti0_Init();
			Exti2_Init();
			Exti3_Init();
			Exti4_Init();
			
			//超声波和定时器
			PA2_Init();		//trig
			PA3_Init();		//echo
			Tim3_Init();

}	


void OLED_show()
{
		RTC_TimeTypeDef RTC_TimeStruct;
		RTC_DateTypeDef RTC_DateStruct;
		char RTC_time[64];
		char RTC_date[64];
		char temp_humi_l[64];
		char distance[64];
		int to=1;
		float temp;
		int value;
		extern const unsigned char BMP1[]; //图片
		u8 i;
		
		init_func();

		
		OLED_Fill(0x00);//全屏灭
		
		
		while(1)
		{

			//LED_Fill(0xFF);//全屏点亮
			//delay_s(2);
			//OLED_Fill(0x00);//全屏灭
		//	delay_s(2);
		/*	
			for(i=5;i<11;i++)
			{
				OLED_ShowCN((i-5)*16,0,i);//测试显示中文
			}
		*/
			memset(RTC_time,0,sizeof(RTC_time));
			memset(RTC_date,0,sizeof(RTC_date));
			//时间日期
			RTC_GetTime(RTC_Format_BIN, &RTC_TimeStruct);
			RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);
			sprintf(RTC_time, "-Time:%d:%d:%d-", RTC_TimeStruct.RTC_Hours, RTC_TimeStruct.RTC_Minutes, 
																		RTC_TimeStruct.RTC_Seconds);
			sprintf(RTC_date, "-Date:20%d.%d.%d-",RTC_DateStruct.RTC_Year, 
									RTC_DateStruct.RTC_Month, RTC_DateStruct.RTC_Date);
			
			//OLED显示时间日期
			OLED_ShowStr(0,1,(u8 *)RTC_time,1);//测试6*8字符
			OLED_ShowStr(0,3,(u8 *)RTC_date,1);				//测试8*16字符
			
			
			
			//TIM_SetCompare3(TIM2,100);
			
			//TIM_SetCompare4(TIM2,10);	
			
			//超声波
			value = Get_SR04_Value();
			
			//计算距离
			//printf("测量距离为%dcm\n", value);
			//延时 避免干扰
			
			if(value < 10)
						Water_lamp();
			printf("距离%d\r\n",value);
			
			sprintf(distance,"**distance :%d cm**",value);
			OLED_ShowStr(0,7,(u8 *)distance,1);
			//采集温湿度
			u8 data[5] = {0};
			int ret;
			ret = Dht11_Start();
			if(ret == 0)
			{
					Dht11_Read(data);
					
			}
			memset(temp_humi_l,0,sizeof(temp_humi_l));
			sprintf(temp_humi_l,"-T:%d.%d H:%d.%d-",data[2],data[3],data[0],data[1]);
			OLED_ShowStr(0,5,(u8 *)temp_humi_l,1);
			

			//delay_s(2);
			//OLED_CLS();//清屏
			//OLED_OFF();//测试OLED休眠
			//delay_s(2);
			//OLED_ON();//测试OLED休眠后唤醒
			//OLED_DrawBMP(0,0,128,8,(unsigned char *)BMP1);//测试BMP位图显示
			//delay_s(2);
		
			}


}	


int main(void)
{
	OLED_show();
	return 0;
}



