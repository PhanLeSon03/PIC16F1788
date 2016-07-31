/* 
 * File:   PWM.h
 * Author: PCMART
 *
 * Created on March 29, 2014, 12:17 AM
 */

#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#endif

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

#ifndef PWM_H
#define	PWM_H

void PWMSetConfig(void);
void PWMIni(void);
void PWMSetPeriod(uint16_t cntPeriod );
void PWMSetDutyCycle (uint16_t cntDutyCycle);
void PWMSetPhase(uint16_t cntPhase);
void SetPWM1(unsigned int DutyCycle);
void SetPeriod (unsigned int rPeriod);
void SetDeadTime(unsigned char rDeadTime);

#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

#endif	/* PWM_H */

