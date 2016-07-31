/* 
 * File:   dio.h
 * Author: sop1hc
 *
 * Created on June 6, 2014, 2:31 PM
 */

#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#endif

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

#ifndef DIO_H
#define	DIO_H
void DioIni(void);

#ifdef	__cplusplus
extern "C" {
#endif



#ifdef	__cplusplus
}
#endif

#endif	/* DIO_H */

