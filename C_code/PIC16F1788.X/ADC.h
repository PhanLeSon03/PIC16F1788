/* 
 * File:   ADC.h
 * Author: sop1hc
 *
 * Created on March 28, 2014, 5:15 PM
 */
#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#endif

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */
#include "interrupts.h"

#ifndef ADC_H
#define	ADC_H

#define Fosc_2  0
#define Fosc_8  1
#define Fosc_32 2
#define FRC     3
#define AN0     0
#define AN1     1
#define AN2     2
#define AN3     3
#define AN4     4
#define AN5     5
#define AN6     6
#define AN9     9
#define AN10    10

#define ADC_THRESSENS 3000
#define KD 0
#define KI 0.000


#define KP 0.001
#define dT 1

#define ADC_MAX 0x0FFF
#define ADC_MIN 0x0000










void ADCIni(char ANx , char Fosc);
void ADCINTIni(char ANx, char Fosc);
int ReadADC(char ANx);
int16_t ReadADCINT(void);
int16_t ChkPeakVal(void);
int16_t SearchingOffSet(void);

#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

#endif	/* ADC_H */

