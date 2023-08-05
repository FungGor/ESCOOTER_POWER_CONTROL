/*
 * POWER_CONTROL_LL.c
 *
 *  Created on: 27 Jul 2023
 *      Author: TerenceLeung
 */

#include "POWER_CONTROL_LL.h"
#include "POWER_CONTROL.h"
#include "escooter_control.h"
#include "motorcontrol.h"
#include "main.h"
#include "cmsis_os.h"
#include "stm32f4xx.h"

osThreadId PowerControlHandler;

static void SYSTEM_POWER_OFF();
static void SYSTEM_BOOT();
static void SYSTEM_POWER_ON();
static void SYSTEM_INDICATOR_ON();
static void SYSTEM_INDICATOR_OFF();
static void SYSTEM_PERIPHERAL_DISABLE();
static void SYSTEM_PERIPHERAL_SLEEP_DISABLE();
static void SYSTEM_START_RETRANSMISSION_TIMER();
static void SYSTEM_STOP_RETRANSMISSION_TIMER();
void TIM3_IRQHandler(void);

static POWER_Control power_control =
{
		SYSTEM_POWER_OFF,
		SYSTEM_BOOT,
		SYSTEM_POWER_ON
};

static Power_Status_Indicator power_status_indicator =
{
		SYSTEM_INDICATOR_ON,
		SYSTEM_INDICATOR_OFF
};

static Power_sysProtocol_Handler protocol_control =
{
		SYSTEM_START_RETRANSMISSION_TIMER,
		SYSTEM_STOP_RETRANSMISSION_TIMER
};

/*This function must be called before starting Power Monitoring*/
void POWER_CONTROL_Init()
{
	POWER_CONTROL_CONFG(&power_control);
	POWER_INDICATOR_CONFG(&power_status_indicator);
	POWER_RETRANSMIT_CTL_CONFG(&protocol_control);
	POWER_SET_DEFAULT_STATE(POWER_ON);
}

void POWER_CONTROL_START_MONITORING()
{
    /*This thread must be run forever even if the system is in Low Power Mode*/
	osThreadDef(PowerStateMachine, POWER_CTL_MONITORING, osPriorityBelowNormal,0,128);
	PowerControlHandler = osThreadCreate(osThread(PowerStateMachine),NULL);
}

static void SYSTEM_POWER_OFF()
{
	Peripheral_DeInit();
	HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON,PWR_SLEEPENTRY_WFI);
}

static void SYSTEM_BOOT()
{
	/* Reset all the system peripherals ! Reset the program counter!
	 * Resume RTOS tasks !
	 * */
	HAL_NVIC_SystemReset();
}

uint8_t power = 0;
static void SYSTEM_POWER_ON()
{
	//HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin,GPIO_PIN_SET);
	power = 1;
}

static void SYSTEM_INDICATOR_ON()
{

}

static void SYSTEM_INDICATOR_OFF()
{

}

static void SYSTEM_PERIPHERAL_DISABLE()
{
	/*But you can use DeInit Function*/
     __HAL_RCC_GPIOA_CLK_DISABLE();
     __HAL_RCC_GPIOB_CLK_DISABLE();
     __HAL_RCC_GPIOC_CLK_DISABLE();
     __HAL_RCC_TIM1_CLK_DISABLE();
     __HAL_RCC_TIM2_CLK_DISABLE();
     __HAL_RCC_ADC1_CLK_DISABLE();
     __HAL_RCC_PWR_CLK_DISABLE();
     __HAL_RCC_SYSCFG_CLK_DISABLE();
}

static void SYSTEM_PERIPHERAL_SLEEP_DISABLE()
{
	/*But you can use DeInit Function*/
    __HAL_RCC_GPIOA_CLK_SLEEP_DISABLE();
    __HAL_RCC_GPIOB_CLK_SLEEP_DISABLE();
    __HAL_RCC_GPIOC_CLK_SLEEP_DISABLE();
    __HAL_RCC_TIM1_CLK_SLEEP_DISABLE();
    __HAL_RCC_TIM2_CLK_SLEEP_DISABLE();
    __HAL_RCC_ADC1_CLK_SLEEP_DISABLE();
    __HAL_RCC_PWR_CLK_SLEEP_DISABLE();
    __HAL_RCC_SYSCFG_CLK_DISABLE();
}

static void SYSTEM_START_RETRANSMISSION_TIMER()
{
	//Enable the TIM3 CLOCK
   RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

   /*Config for the prescalar + auto reload register*/
   TIM3->PSC = 10000;  /*Pre-scalar*/
   TIM3->ARR = 8400;   /*Auto reload register*/

   /*Set up the CounterMode: Up*/
   TIM3->CR1 &= ~TIM_CR1_DIR;

   /*Setup the clock division as 1*/
   TIM3->CR1 |= TIM_CR1_CKD_1;

   /*Auto-Reload Pre-load Disable! */
   TIM3->CR1 &= ~TIM_CR1_ARPE;

   /*Setup the Clock Source as Internal Clock*/
   TIM3->SMCR &= ~TIM_SMCR_SMS;

   /*Enable the Interrupt*/
   TIM3->DIER |= TIM_DIER_UIE;

   /*Set the Interrupt Priority*/
   NVIC_SetPriority(TIM3_IRQn, 10);

   NVIC_EnableIRQ(TIM3_IRQn);

   /*Start the timer*/
   TIM3->CR1 |= TIM_CR1_CEN;
}

static void SYSTEM_STOP_RETRANSMISSION_TIMER()
{
   /*Stop the timer*/
   TIM3->CR1 &= ~TIM_CR1_CEN;

   /*Disable the Interrupt*/
   TIM3->DIER &= ~TIM_DIER_UIE;
}

void TIM3_IRQHandler(void)
{
	if(TIM3->SR & TIM_SR_UIF)
	{
		PacketLossCount();
		TIM3->SR &= ~TIM_SR_UIF;
	}
}