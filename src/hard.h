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
#define IN_6V        335
#define IN_10V       559
#define IN_11V       614
#define IN_12V       670
#define IN_16V       894

#define BAT_9_5V     531
#define BAT_11_5V    643

#define OUT_69V      628
#define OUT_75V      683
#define OUT_80V      747    //ajustada 30-9-19 tiene 82V salida
#define OUT_90V      840

#define CURR_25MA    24
#define CURR_30MA    33
#define CURR_49MA    44
#define CURR_50MA    45
#define CURR_60MA    54
#define CURR_65MA    59
#define CURR_70MA    63

// where to go?
#define VOUT_SETPOINT    OUT_75V
#define VOUT_MAX_THRESHOLD    OUT_90V
#define VOUT_MIN_THRESHOLD    OUT_69V
#define UNDERSAMPLING_TICKS    6

#define IOUT_SETPOINT    CURR_60MA
#define MAINS_TO_RECONNECT    IN_12V
#define MAINS_MIN_VALID_VOLTAGE    IN_11V
#define BATTERY_TO_RECONNECT    BAT_11_5V
#define BATTERY_LOW    BAT_9_5V

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
#define CTRL_BOOST ((GPIOB->ODR & 0x0020) != 0)
#define CTRL_BOOST_ON GPIOB->BSRR = 0x00000020     //esto es estado alto Hi-z
#define CTRL_BOOST_OFF GPIOB->BSRR = 0x00200000    //esto es estado bajo 0V

//GPIOB pin6
//GPIOB pin7    NC

//ESTADOS DEL PROGRAMA PRINCIPAL
typedef enum {
    INIT = 0,
    STAND_BY,
    TO_GEN,
    GENERATING,
    TO_STAND_BY,
    LOW_BAT,
    OVERCURRENT,
    OVERVOLTAGE
    
} main_state_t;

//ESTADOS DEL BOOST
typedef enum {
    VOLTAGE_MODE = 0,
    CURRENT_MODE

} boost_state_t;


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
#define LED_LOW_VOLTAGE               3
#define LED_PROTECTED                 4
#define LED_VIN_ERROR                 5
#define LED_OVERCURRENT_ERROR         6
#define LED_OVERVOLTAGE             7


/* Module Functions ------------------------------------------------------------*/
void ChangeLed (unsigned char);
void UpdateLed (void);

#endif /* HARD_H_ */
