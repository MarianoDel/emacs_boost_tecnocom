//---------------------------------------------
// #### PROYECTO BOOST TECNOCOM - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### MAIN.C ################################
//---------------------------------------------

/* Includes ------------------------------------------------------------------*/
#include "hard.h"
#include "stm32f0xx.h"
#include "gpio.h"
#include "uart.h"

#include "core_cm0.h"
#include "adc.h"
#include "tim.h"

//#include <stdio.h>
//#include <string.h>




//--- VARIABLES EXTERNAS ---//
// ------- Externals del ADC -------
volatile unsigned short adc_ch [3];
volatile unsigned char seq_ready;

// ------- Externals de los timers -------
volatile unsigned short timer_led = 0;
volatile unsigned short timer_standby;
volatile unsigned short wait_ms_var = 0;


// ------- Externals del USART -------



//--- VARIABLES GLOBALES ---//


// ------- de los timers -------

//--- FUNCIONES DEL MODULO ---//
void TimingDelay_Decrement(void);



//-------------------------------------------//
// @brief  Main program.
// @param  None
// @retval None
//------------------------------------------//
int main(void)
{
    unsigned short i = 0;

    //GPIO Configuration.
    GPIO_Config();

    //TIM Configuration.
    // TIM_3_Init();
    // TIM_14_Init();

    //ACTIVAR SYSTICK TIMER
    if (SysTick_Config(48000))
    {
        while (1)	/* Capture error */
        {
            if (LED)
                LED_OFF;
            else
                LED_ON;

            for (i = 0; i < 255; i++)
            {
                asm (	"nop \n\t"
                        "nop \n\t"
                        "nop \n\t" );
            }
        }
    }

    //prueba led y wait
    // while (1)
    // {
    //     if (LED)
    //         LED_OFF;
    //     else
    //         LED_ON;

    //     Wait_ms(1000);
    // }
    //fin prueba led y wait

    //prueba modulo adc.c tim.c e int adc
    TIM_3_Init();
    Update_TIM3_CH2 (25);

    AdcConfig();
    ADC1->CR |= ADC_CR_ADSTART;
    
    while (1)
    {
        if (seq_ready)
        {
            seq_ready = 0;
            // if (LED)
                // LED_OFF;
            // else
                // LED_ON;
            LED_OFF;
        }
    }               
    //fin prueba modulo adc.c tim.c e int adc
    

    // AdcConfig();
    // ADC1->CR |= ADC_CR_ADSTART;
    
    // TIM_14_Init();
    // UpdateLaserCh1(0);
    // UpdateLaserCh2(0);
    // UpdateLaserCh3(0);
    // UpdateLaserCh4(0);

    // USART1Config();

    
    // while (1)
    // {        
    //     TreatmentManager();
    //     UpdateCommunications();
    //     UpdateLed();
    //     UpdateBuzzer();
    // }
    // //fin prueba modulo signals.c comm.c tim.c adc.c

    //prueba modulo adc.c tim.c e int adc
    // TIM_3_Init();
    // Update_TIM3_CH1(511);
    // Update_TIM3_CH2(0);
    // Update_TIM3_CH3(0);
    // Update_TIM3_CH4(0);

    // AdcConfig();
    // ADC1->CR |= ADC_CR_ADSTART;
    
    // while (1)
    // {
    //     if (seq_ready)
    //     {
    //         seq_ready = 0;
    //         if (LED)
    //             LED_OFF;
    //         else
    //             LED_ON;
    //     }
    // }               
    //fin prueba modulo adc.c tim.c e int adc



    // Update_TIM3_CH1(511);
    // Update_TIM3_CH2(511);
    // Update_TIM3_CH3(511);
    // Update_TIM3_CH4(511);
    // while (1);
    





    return 0;
}
//--- End of Main ---//

void TimingDelay_Decrement(void)
{
    if (wait_ms_var)
        wait_ms_var--;

    if (timer_standby)
        timer_standby--;

    if (timer_led)
        timer_led--;

}

//--- end of file ---//

