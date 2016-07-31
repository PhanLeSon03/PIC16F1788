/******************************************************************************/
/*Files to Include                                                            */
/******************************************************************************/

#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#endif

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */
#include "interrupts.h"
#include "PWM.h"

#define PSMC_NUMPERD_ON 40     /* number of period to stop PSMC function*/
#define PSMC_NUMPERD_OFF 200

/******************************************************************************/
/* Interrupt Routines                                                         */
/******************************************************************************/

/* Baseline devices don't have interrupts. Note that some PIC16's 
 * are baseline devices.  Unfortunately the baseline detection macro is 
 * _PIC12 */
#ifndef _PIC1
#if _POLLING
#else
volatile char UART_chRxBuf[20];
volatile char UART_chTxBuf[20];
volatile unsigned char UART_cntTxUART;
volatile unsigned char UART_cntRxUART;
#endif

unsigned volatile int counter;
volatile int16_t ADC_vADC;
volatile int16_t ADC_vADCAry[ADC_LARRAY];
uint8_t idxADC;
uint8_t cntPerd;
volatile bit flgPSMC;
volatile bit flgTicTac;
volatile bit flgADCAryFull;

extern uint16_t vPWM;

/* static variable */
#if _POLLING
#else
static unsigned char stLen;
#endif

void interrupt isr(void)
{
    /* This code stub shows general interrupt handling.  Note that these
    conditional statements are not handled within 3 seperate if blocks.
    Do not use a seperate if block for each interrupt flag to avoid run
    time errors. */

#if 0
    
    /* TODO Add interrupt routine code here. */

    /* Determine which flag generated the interrupt */
    if(<Interrupt Flag 1>)
    {
        <Interrupt Flag 1=0>; /* Clear Interrupt Flag 1 */
    }
    else if (<Interrupt Flag 2>)
    {
        <Interrupt Flag 2=0>; /* Clear Interrupt Flag 2 */
    }
    else
    {
        /* Unhandled interrupts */
    }

#endif
    /* Disable Interrupt */
    di();
    //Check if it is TMR0 Overflow ISR
    if(TMR0IE && TMR0IF)
    {
           if (counter == 100) //1000*100 us = 0.1s
           {
               counter = 0;
               flgTicTac = true;
           }
           ++counter;
           TMR0IF=0;
           TMR0 = 205;
     }

    /* Check if it is Period Flag of PSMC  */
    if (P1TPRIE && P1TPRIF)
    {
        //P1TPRIE = 0; /* Disable Interrupt */
        cntPerd++;//after 10 period -->
        if (flgPSMC==false) /*ON cycle*/
        {
            if (cntPerd == PSMC_NUMPERD_ON)
            {
               flgPSMC = true;
               cntPerd = 0;
            }
            ////PWMSetPeriod (64000-((uint32_t)63990*vPWM)/4096);
            ////PWMSetDutyCycle (32000-((uint32_t)63990*vPWM)/8192);//old:60800
            PWMSetPeriod (16000-((uint32_t)15600*vPWM)/4096);
            PWMSetDutyCycle (8000-((uint32_t)15600*vPWM)/8192);
            //PWMSetDutyCycle (8000-((uint32_t)7360*vPWM)/4096);
            //64 MHz
        }
        else /*OFF cycle*/
        {
            if (cntPerd == PSMC_NUMPERD_OFF)
            {
               flgPSMC = false;
               cntPerd = 0;
            }
            //PWMSetPeriod (0);
            PWMSetDutyCycle (0);
        }
        
        P1TPRIF = 0; /* Clear interrupt class*/
        //P1TPRIE = 1; /*  Enable Interrupt */
    }

#if _POLLING
#else

    /* UART receive data */
    if (RCIE && RCIF)  /* RCIF=TRUE when RSR --> RCREG*/
    {
        UART_chRxBuf[UART_cntRxUART++] = RCREG;
    }

    /* UART transmit data */
    if (TXIE && TXIF)  /* TXREG is free*/
                       /* TXIF couldn't be clear by SW */
    {
        if ((UART_cntTxUART!=0))  /* Thera are send command */
        {
            TXREG = UART_chTxBuf[stLen]; /* upload data to Transmit Register  */
                                           /* TXIF is clear when TXREG <-- data */

            stLen++;

            if (stLen == UART_cntTxUART)    /* transmit complete the string */
            {
                UART_cntTxUART = 0;
                stLen = 0;
                TXEN = false;
                TXIE = false;

            }
        }
        else /* Transmit Finish */
        {
            /* start Tranmit data */
            TXEN = false;
            TXIE = false;
        }
    }
#endif


    /* ADC interrupt */
    if (ADIE&&ADIF)
    {

        if (idxADC<ADC_LARRAY)
        {
            idxADC++;
            flgADCAryFull = false;
        }
        else
        {
            idxADC = 0;
            flgADCAryFull = true;
            ADIE = 0; /* Disable ADC interrupt */

             TXIE = 1;/* Enable UART Transmission */
        }

        ADC_vADCAry[idxADC] = ADRESH<<8;
        //ADC_vADCAry[idxADC] <<=8;
        ADC_vADCAry[idxADC] |=ADRESL;

        ADIF = 0;
        GO=1;
    }

    /* Enable Interrupt */
    ei();


}
#endif


