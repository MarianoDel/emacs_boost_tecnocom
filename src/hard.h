/*
 * hard.h
 *
 *  Created on: 28/11/2013
 *      Author: Mariano
 */

#ifndef _HARD_H_
#define _HARD_H_

#include "stm32f0xx.h"


//-- Defines For Configuration -------------------
//---- Configuration for Hardware Versions -------
#define HARDWARE_VERSION_1_0


#define SOFTWARE_VERSION_1_0
// #define SOFTWARE_VERSION_1_1

//---- Features Configuration ----------------


//------ Configuration for Firmware-Channels -----


//---- End of Features Configuration ----------



//--- Hardware Welcome Code ------------------//
#ifdef HARDWARE_VERSION_1_0
#define HARD "Hardware Version: 1.0\n"
#endif
#ifdef HARDWARE_VERSION_2_0
#define HARD "Hardware Version: 2.0\n"
#endif

//--- Software Welcome Code ------------------//
#ifdef SOFTWARE_VERSION_1_2
#define SOFT "Software Version: 1.2\n"
#endif
#ifdef SOFTWARE_VERSION_1_1
#define SOFT "Software Version: 1.1\n"
#endif
#ifdef SOFTWARE_VERSION_1_0
#define SOFT "Software Version: 1.0\n"
#endif

//-------- Configuration for Outputs-Channels -----


//---- Configuration for Firmware-Programs --------


//-------- Configuration for Outputs-Firmware ------


//-- End Of Defines For Configuration ---------------

//GPIOA pin0
//GPIOA pin1
//GPIOA pin2    3 ADC channels

//GPIOA pin3
#define LED ((GPIOA->ODR & 0x0008) != 0)
#define LED_ON GPIOA->BSRR = 0x00000008
#define LED_OFF GPIOA->BSRR = 0x00080000

//GPIOA pin4

//GPIOA pin5
#define RELAY ((GPIOA->ODR & 0x0020) != 0)
#define RELAY_ON GPIOA->BSRR = 0x00000020
#define RELAY_OFF GPIOA->BSRR = 0x00200000

//GPIOA pin6
//GPIOA pin7
//GPIOB pin0
//GPIOB pin1 

//GPIOA pin8    
//GPIOA pin9
//GPIOA pin10    
//GPIOA pin11   
//GPIOA pin12

//GPIOA pin13
//GPIOA pin14    
//GPIOA pin15    

//GPIOB pin3     JUMPER NO GEN
#define JUMPER_NO_GEN ((GPIOB->IDR & 0x0008) == 0)

//GPIOB pin4

//GPIOB pin5     TIM3_CH2

//GPIOB pin6
//GPIOB pin7    NC


//ESTADOS DEL LED
typedef enum
{    
    START_BLINKING = 0,
    WAIT_TO_OFF,
    WAIT_TO_ON,
    WAIT_NEW_CYCLE
} led_state_t;


//Estados Externos de LED BLINKING
#define LED_NO_BLINKING               0
#define LED_STANDBY                   1
#define LED_GENERATING                2
#define LED_VIN_ERROR                 5
#define LED_OVERCURRENT_ERROR         6

#define Vin_Sense      adc_ch[0]
#define Boost_Sense    adc_ch[1]
#define Vout_Sense     adc_ch[2]


/* Module Functions ------------------------------------------------------------*/
void ChangeLed (unsigned char);
void UpdateLed (void);

#endif /* HARD_H_ */
