/*
 * POWER_CONTROL.h
 *
 *  Created on: 27 Jul 2023
 *      Author: TerenceLeung
 */

#ifndef POWER_CONTROL_POWER_CONTROL_H_
#define POWER_CONTROL_POWER_CONTROL_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

typedef enum
{
	POWER_OFF  =  0,
	POWER_ON =  1,
	WAKEUP   = 2
}POWER_State_t;

typedef void (*system_sleep)(void);
typedef void (*system_wake)(void);
typedef void (*system_power_on)(void);
typedef struct
{
    system_sleep sleep;
    system_wake wake;
    system_power_on powerOn;
}POWER_Control;

typedef void (*indicator_on)(void);
typedef void (*indicator_off)(void);
typedef struct
{
	indicator_on  switch_on;
	indicator_off switch_off;
}Power_Status_Indicator;

void POWER_CONTROL_CONFG(POWER_Control *cmd);
void POWER_SET_DEFAULT_STATE(POWER_State_t state);
void POWER_CHANGE_STATE(POWER_State_t state);
void POWER_SLEEP();
void POWER_WAKEUP();
void POWER_INDICATOR_CONFG(Power_Status_Indicator *indicator);
void POWER_CTL_MONITORING(void const *argument);

#ifdef __cplusplus
}
#endif
#endif /* POWER_CONTROL_POWER_CONTROL_H_ */
