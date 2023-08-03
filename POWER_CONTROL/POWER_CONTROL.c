/*
 * POWER_CONTROL.c
 *
 *  Created on: 27 Jul 2023
 *      Author: TerenceLeung
 */
#include "POWER_CONTROL.h"
#include "POWER_CONTROL_LL.h"
#include "main.h"
#include <stdint.h>

static POWER_Control *pwrControl;
static Power_Status_Indicator *tailLightControl;
POWER_State_t state_Handler;

void POWER_CONTROL_CONFG(POWER_Control *cmd)
{
	pwrControl = cmd;
}

void POWER_INDICATOR_CONFG(Power_Status_Indicator *indicator)
{
	tailLightControl = indicator;
}

void POWER_SET_DEFAULT_STATE(POWER_State_t state)
{
	state_Handler = state;
}

void POWER_CHANGE_STATE(POWER_State_t state)
{
	state_Handler = state;
}

void POWER_SLEEP()
{
	pwrControl->sleep();
	//Peripheral_DeInit();
}

void POWER_WAKEUP()
{
	pwrControl->wake();
}

void POWER_CTL_MONITORING(void const *argument)
{
	for(;;)
	{
		switch(state_Handler)
		{
		    case POWER_OFF:
		    	pwrControl -> sleep();
		    	break;

		    case POWER_ON:
		    	pwrControl -> powerOn();
		    	break;

		    case WAKEUP:
		    	pwrControl -> wake();

		    default:
		    	break;
		}
	}
}
