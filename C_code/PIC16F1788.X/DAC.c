#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#endif

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */
#include "DAC.h"

char ADCON0Tmp;

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

void DACInit(void)
{
    TRISA2 = false;  /* Output */
    ANSA2 = true;    /* Analog ???????*/

    DAC1NSS = true; /* Vref- = Vss */


    /*  DAC1CON0: VOLTAGE REFERENCE CONTROL REGISTER 0
        bit 7 DAC1EN: DAC1 Enable bit
            1 = DAC is enabled
            0 = DAC is disabled
        bit 6 Unimplemented: Read as ?0?
        bit 5 DAC1OE1: DAC1 Voltage Output 1 Enable bit
            1 = DAC voltage level is also an output on the DAC1OUT1 pin
            0 = DAC voltage level is disconnected from the DAC1OUT1 pin
        bit 4 DAC1OE2: DAC1 Voltage Output 2 Enable bit
            1 = DAC voltage level is also an output on the DAC1OUT2 pin
            0 = DAC voltage level is disconnected from the DAC1OUT2 pin
        bit 3-2 DAC1PSS<1:0>: DAC1 Positive Source Select bits
            11 = Reserved, do not use
            10 = FVR Buffer2 output
            01 = VREF+ pin
            00 = VDD
        bit 1 Unimplemented: Read as ?0?
        bit 0 DAC1NSS: DAC1 Negative Source Select bits
            1 = VREF- pin
            0 = VSS
     */
    /* DAC1PSS :bit 2-3 of DAC1CON0 */
    DAC1CON0 = DAC1CON0 & (0b11110010);

    DAC1OE1 = true;

    DAC1EN = true;
    
    DAC1CON1 = 0;
    
}

void DACSet(uint8_t Val)
{
    ADCON0Tmp = ADCON0;

    ADCON0 = (((ADCON0)&0b1000001) | 0b0111100);
    _delay(20);
    DAC1CON1 = Val;
}