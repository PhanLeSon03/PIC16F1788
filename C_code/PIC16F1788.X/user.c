/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#endif

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */
//#include <plib/adc.h>       /* ADC Library*/
//#include <plib/pconfig.h>

#include "user.h"
#include "ADC.h"
#include "PWM.h"
#include "UART.h"
#include "dio.h"
#include "DAC.h"
/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/

/* <Initialize variables in user.h and insert code for user algorithms.> */

void InitApp(void)
{
    /* TODO Initialize User Ports/Peripherals/Project here */
    TMR0 = 205;  /* TMR0 internal value */
    INTCON = 0b10100000; /* INTERRUPT CONTROL REGISTER    */
                         /* Global Interrupt Enable:  BIT0*/
                         /* Timer 0 Interrupt Enable: BIT2*/

    //T0CS = 0; /* select source clock: internal clock*/
    //PSA = 0;  /* Timer clock source links with prescale*/
    OPTION_REG = 0b10000011; /* 011: rate 1:16 */
                             /* BIT7 : port B pull-ups are disable here*/

        //TRISA  = 0xFF;
    // PORTA  = 0b00000000;


    /* Setup analog functionality and port direction */
    //TRISB  = 0b00000000; /* All pins in Port B are output*/
    TRISC2 = 0;
    /* Analog configuration*/
    ADCIni(AN1, Fosc_2);
    //ADCINTIni(AN9, Fosc_2);
    DACInit();

    /* inititalize PWM */
    PWMIni();

    /* Initial DIO*/
    DioIni();

    /* iniitialize UART */
    UARTIni();
    
    /* Initialize peripherals */ 
    
    /* Enable interrupts */
    TMR0IE = 1;  /* Timer 0 Interrupt Enable */
    ei();      /* Global Interrupt Enable  */
}

