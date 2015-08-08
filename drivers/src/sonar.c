/**
 *    ||          ____  _ __
 * +------+      / __ )(_) /_______________ _____  ___
 * | 0xBC |     / __  / / __/ ___/ ___/ __ `/_  / / _ \
 * +------+    / /_/ / / /_/ /__/ /  / /_/ / / /_/  __/
 *  ||  ||    /_____/_/\__/\___/_/   \__,_/ /___/\___/
 *
 * Crazyflie Firmware
 *
 * Copyright (C) 2011-2012 Bitcraze AB
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, in version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 *
 */
#include <math.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"

#include "config.h"
#include "system.h"
#include "pm.h"
#include "commander.h"
#include "log.h"
#include "param.h"
#include "lps25h.h"
#include "debug.h"




#define SONAR_TIMER		TIM6


static bool isInit;
uint16_t	sonarRange;

static volatile uint16_t isr_counter = 0;


volatile uint16_t pulse_flag = 0;
volatile uint16_t pulse_start;
volatile uint16_t pulse_end;
volatile uint16_t pulse_time;



static void sonarTask(void* param);





void sonarInit(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef extiInit;


	if(isInit)
		return;


	//-- 타이머 설정
	//
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

	//Timer configuration
	TIM_TimeBaseStructure.TIM_Period    		= 0xFFFF;
	TIM_TimeBaseStructure.TIM_Prescaler 		= (84-1);
	TIM_TimeBaseStructure.TIM_ClockDivision 	= 0;
	TIM_TimeBaseStructure.TIM_CounterMode 		= TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(SONAR_TIMER, &TIM_TimeBaseStructure);


	TIM_Cmd( SONAR_TIMER, ENABLE );


	//-- 외부 인터럽트 설
	//
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	bzero(&GPIO_InitStructure, sizeof(GPIO_InitStructure));
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	extiInit.EXTI_Line    = EXTI_Line11;
	extiInit.EXTI_Mode    = EXTI_Mode_Interrupt;
	extiInit.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	extiInit.EXTI_LineCmd = ENABLE;
	EXTI_Init(&extiInit);

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource11);


	NVIC_EnableIRQ(EXTI15_10_IRQn);


	isInit = true;
}


bool sonarTest(void)
{
	bool pass = true;


	return pass;
}


void sonarExtiInterruptHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line11) == SET)
	{
		isr_counter++;


		if( pulse_flag == 0 )
		{
			pulse_start = SONAR_TIMER->CNT;
		}
		else
		{
			pulse_end = SONAR_TIMER->CNT;
			pulse_time = pulse_end-pulse_start;
			sonarRange = pulse_time/57;
		}

		pulse_flag ^= 1;

		EXTI_ClearITPendingBit(EXTI_Line11);
	}
}

#if defined(ACTIVATE_SONAR_SENSOR)
LOG_GROUP_START(sonar)
LOG_ADD(LOG_UINT16, sonarRange, &sonarRange)
LOG_GROUP_STOP(sonar)
#endif
