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
#include "dma.h"
#include "tim.h"
#include "dsp.h"

//#include <stdio.h>
//#include <string.h>




//--- VARIABLES EXTERNAS ---//
// ------- Externals del ADC -------
volatile unsigned short adc_ch [ADC_CHANNEL_QUANTITY];

// ------- Externals de los timers -------
volatile unsigned short timer_led = 0;
volatile unsigned short timer_standby;
volatile unsigned short timer_filters;
volatile unsigned short wait_ms_var = 0;


// ------- Definiciones para los filtros -------


// Globals ---------------------------------------------------------------------
// for the pid controllers
pid_data_obj_t voltage_pid;

// for circular filters
ma16_u16_data_obj_t sense_batt_data_filter;
ma16_u16_data_obj_t sense_boost_data_filter;
ma16_u16_data_obj_t sense_mains_data_filter;

// ------- de los timers -------

//--- FUNCIONES DEL MODULO ---//
void TimingDelay_Decrement(void);
unsigned short Control_Voltage_Mode (short, unsigned short);
unsigned short Control_Current_Mode (short, unsigned short);
void Set_Control_Voltage_Mode (void);
void Set_Control_Current_Mode (void);

//-------------------------------------------//
// @brief  Main program.
// @param  None
// @retval None
//------------------------------------------//
int main(void)
{
    main_state_t main_state = INIT;
    unsigned char protected = 0;
    unsigned char undersampling = 0;

    unsigned short sense_curr_filtered = 0;
    unsigned short sense_batt_filtered = 0;
    unsigned short sense_mains_filtered = 0;
    short d = 0;

    boost_state_t boost_state = VOLTAGE_MODE;
        

    //GPIO Configuration.
    GPIO_Config();

    //ACTIVAR SYSTICK TIMER
    if (SysTick_Config(48000))
    {
        while (1)	/* Capture error */
        {
            if (LED)
                LED_OFF;
            else
                LED_ON;

            for (unsigned char i = 0; i < 255; i++)
            {
                asm (	"nop \n\t"
                        "nop \n\t"
                        "nop \n\t" );
            }
        }
    }

    // Timers Configuration
    TIM_3_Init();
    Update_TIM3_CH2 (0);

    //ADC and DMA configuration
    AdcConfig();
    DMAConfig();
    DMA1_Channel1->CCR |= DMA_CCR_EN;
    ADC1->CR |= ADC_CR_ADSTART;
    //end of ADC & DMA

    ChangeLed(LED_STANDBY);
    RELAY_ON;

    timer_standby = 1000;

    //start the circular filters
    MA16_U16Circular_Reset(&sense_batt_data_filter); 
    MA16_U16Circular_Reset(&sense_boost_data_filter);
    MA16_U16Circular_Reset(&sense_mains_data_filter);
    
    //start the pid data for controller
    PID_Flush_Errors(&voltage_pid);
    Set_Control_Voltage_Mode();

    while (1)
    {
        //the most work involved is sample by sample
        if (sequence_ready)
        {
            sequence_ready_reset;

            //filters
            sense_batt_filtered = MA16_U16Circular(&sense_batt_data_filter, VBatt_Sense);
            sense_curr_filtered = MA16_U16Circular(&sense_boost_data_filter, Boost_Sense);
            sense_mains_filtered = MA16_U16Circular(&sense_mains_data_filter, Vmains_Sense);
            
            switch (main_state)
            {
            case INIT:
                if (!timer_standby)
                    main_state++;

                break;

            case STAND_BY:
                //tengo baja tension de 220, me paso a bateria
                if (sense_mains_filtered < MAINS_MIN_VALID_VOLTAGE)
                {
                    main_state = TO_GEN;
                    RELAY_OFF;
                    timer_standby = 1000;
                }
                break;

            case TO_GEN:
                if (!timer_standby)
                {
                    //paso a generar
                    main_state = GENERATING;
                    ChangeLed(LED_GENERATING);
                    d = 0;
                    PID_Flush_Errors(&voltage_pid);
                }
                break;

            case GENERATING:
                if (!protected)
                {
                    if (!JUMPER_NO_GEN)
                    {              
                        if (undersampling < (UNDERSAMPLING_TICKS - 1))
                            undersampling++;
                        else
                        {
                            switch (boost_state)
                            {
                            case VOLTAGE_MODE:
                                d = Control_Voltage_Mode (d, Vout_Sense);

                                Update_TIM3_CH2 (d);

                                if (sense_curr_filtered > CURR_70MA)
                                {
                                    Set_Control_Current_Mode();
                                    boost_state = CURRENT_MODE;
                                }
                                break;

                            case CURRENT_MODE:
                                d = Control_Current_Mode (d, sense_curr_filtered);

                                Update_TIM3_CH2 (d);

                                if ((sense_curr_filtered < CURR_25MA) ||
                                    (Vout_Sense < VOUT_MIN_THRESHOLD))
                                {
                                    Set_Control_Voltage_Mode();
                                    boost_state = VOLTAGE_MODE;
                                }
                                break;

                            default:
                                boost_state = VOLTAGE_MODE;
                                break;
                            }
                        }
                    }    //cierra jumper protected
                    else
                    {
                        //me piden que no envie senial y proteja
                        Update_TIM3_CH2 (0);
                        protected = 1;
                        ChangeLed(LED_PROTECTED);
                    }
                }    //cierra variable protect
                else
                {
                    //estoy protegido reviso si tengo que salir
                    if (!JUMPER_NO_GEN)
                    {
                        protected = 0;
                        ChangeLed(LED_GENERATING);
                        d = 0;
                        PID_Flush_Errors(&voltage_pid);
                    }
                }

                //reviso si vuelven los 220
                if (sense_mains_filtered > MAINS_TO_RECONNECT)
                {
                    //dejo de generar y vuelvo a 220
                    Update_TIM3_CH2(0);
                    main_state = TO_STAND_BY;
                    timer_standby = 500;
                }

                //reviso si esta ya muy baja la bateria
                if (sense_batt_filtered < BAT_9_5V)
                {
                    Update_TIM3_CH2(0);
                    ChangeLed(LED_LOW_VOLTAGE);
                    main_state = LOW_BAT;
                }
                break;

            case TO_STAND_BY:
                if (!timer_standby)
                {
                    RELAY_ON;
                    ChangeLed(LED_STANDBY);
                    main_state = STAND_BY;
                    timer_standby = 1500;
                }
                break;

            case LOW_BAT:
                //si se recupera la bateria vuelvo a generar
                if (sense_batt_filtered > BATTERY_TO_RECONNECT)
                {
                    timer_standby = 1000;
                    main_state = TO_GEN;
                }

                //si tengo 220 voy a stand-by
                if (sense_mains_filtered > MAINS_TO_RECONNECT)
                {
                    ChangeLed(LED_STANDBY);
                    main_state = TO_STAND_BY;
                    timer_standby = 500;
                }
                break;

            case OVERCURRENT:
                break;

            case OVERVOLTAGE:
                if (Vout_Sense < VOUT_MIN_THRESHOLD)
                {
                    main_state = TO_GEN;
                }
                break;
                
            default:
                break;
            }
        }

        if ((main_state != OVERVOLTAGE) &&
            (Vout_Sense > VOUT_MAX_THRESHOLD))
        {
            d = 0;
            PID_Flush_Errors(&voltage_pid);
            Update_TIM3_CH2(0);
            ChangeLed(LED_OVERVOLTAGE);
        }
        
        UpdateLed();
    }

    return 0;
}
//--- End of Main ---//
inline void Set_Control_Voltage_Mode (void)
{
    voltage_pid.kp = 32;
    voltage_pid.ki = 8;
    voltage_pid.kd = 0;
}

inline void Set_Control_Current_Mode (void)
{
    voltage_pid.kp = 0;
    voltage_pid.ki = 42;    //necesito error mayor a 3 por definicion en el pwm    
    voltage_pid.kd = 0;
}

inline unsigned short Control_Voltage_Mode (short duty, unsigned short meas)
{
    voltage_pid.setpoint = VOUT_SETPOINT;
    voltage_pid.sample = meas;
    duty = PID(&voltage_pid);

    if (duty > 0)
    {
        if (duty > DUTY_FOR_DMAX)	//no pasar del 90%
        {
            duty = DUTY_FOR_DMAX;
            voltage_pid.last_d = DUTY_FOR_DMAX;
        }                                
    }
    else
    {
        duty = 0;
        voltage_pid.last_d = 0;
    }

    return (unsigned short) duty;
}

unsigned char current_undersampling = 0;
inline unsigned short Control_Current_Mode (short duty, unsigned short meas)
{
    if (current_undersampling)
        current_undersampling--;
    else
    {
        current_undersampling = 100;
        voltage_pid.setpoint = IOUT_SETPOINT;
        voltage_pid.sample = meas;
        duty = PID(&voltage_pid);

        if (duty > 0)
        {
            if (duty > DUTY_FOR_DMAX)	//no pasar del 90%
            {
                duty = DUTY_FOR_DMAX;
                voltage_pid.last_d = DUTY_FOR_DMAX;
            }                                
        }
        else
        {
            duty = 0;
            voltage_pid.last_d = 0;
        }
    }
    return (unsigned short) duty;
}

void TimingDelay_Decrement(void)
{
    if (wait_ms_var)
        wait_ms_var--;

    if (timer_standby)
        timer_standby--;

    if (timer_filters)
        timer_filters--;
    
    if (timer_led)
        timer_led--;

}

//--- end of file ---//

