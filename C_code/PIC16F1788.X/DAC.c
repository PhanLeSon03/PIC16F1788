#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#endif

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */
#include "DAC.h"

/* General configuration :
 * DAC can be connected to:
 *  - External Vref pins
 *  - Vdd Supply
 *  - FVR (Fixed Voltage Reference)
 * enabled by
 * setting the DAC1EN bit of the DAC1CON0 register
 *
 * Vout = {(V+ - V-)*DAC1R[7:0]/2^8 + V-}
 *
 * DAC1NSS: 1 -> VSS
 *          0 -> Vref-
 *
 * DAC1EN : Enable
 *
 * DAC1PSS<1:0>: 00 -> VDD
 *
 * DAC1OE1: 1= DAC voltage level is also an output on the DAC1OUT1 pin
 *
 */

/* HW connect to RA3 : DAC1OUT1*/

uint8_t ADCON0Tmp;
void DACInit(void)
{
    TRISA2 = 0;
    ANSA2 = 0;

    DAC1NSS = true; /* Vref- = Vss */
    //DAC1PSS = 0;

    /* DAC1PSS :bit 2-3 of DAC1CON0 --> Vdd */
    DAC1CON0 = DAC1CON0 & (0b11110011);

    DAC1OE1 = true;

    DAC1OE2 = false;

    DAC1EN = true;
    
}

void DACSet(uint8_t Val)
{


    ADCON0Tmp = ADCON0;

    ADCON0 = (ADCON0&0b10000011)|0b01111000;
    DAC1CON1 = Val;

}