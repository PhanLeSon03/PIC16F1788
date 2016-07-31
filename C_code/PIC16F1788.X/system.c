/******************************************************************************/
/*Files to Include                                                            */
/******************************************************************************/

#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#endif

#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */

#include "system.h"



/* Refer to the device datasheet for information about available
oscillator configurations. */
void ConfigureOscillator(void)
{
    /* TODO Add clock switching code if appropriate.  */
    OSCCON= 0b11111100;    /* Bit 7:  Software PLL Enable bit : 1 --> Enable */
                           /* Bit 6-3: IRCF<3:0>: Internal Oscillator Frequency Select bits */
                           /* 1101 Internal clock 4MHz HF */
                           /* 1111 16 MHz HF or 32 MHz HF */
                           /* Bit 2: undefined */
                           /* Bit 1-0: SCS<1:0>: System Clock Select bits */
                           /* 00: Clock determined by FOSC<2:0> in Configuration Words */

    /* Typical actions in this function are to tweak the oscillator tuning
    register, select new clock sources, and to wait until new clock sources
    are stable before resuming execution of the main project. */
}
