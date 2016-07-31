/* 
 * File:   App.h
 * Author: sop1hc
 *
 * Created on June 30, 2014, 12:23 PM
 */

#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#endif

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */
#include "ADC.h"

#ifndef APP_H
#define	APP_H

#ifdef	__cplusplus
extern "C" {
#endif

 bool ScanFrq(void);
 void Blinking(int vSenFlt);
 void Blink(float tiPerd);



#ifdef	__cplusplus
}
#endif

#endif	/* APP_H */

