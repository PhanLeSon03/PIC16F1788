/*
 * File:   UART.h
 * Author: PCMART
 *
 * Created on March 31, 2014, 8:34 AM
 */



#ifndef UART_H
#define	UART_H

#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#endif

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

#define _POLLING  0

#if _POLLING
void UARTIni(void);
bool UARTSendByte(char chOneChar);
bool UARTReceiveByte(char * pRxByte);
void UARTSend(char *pData, unsigned char stLen);
int8_t *UARTReceive(void);
#else
/* Interrupt implementation style will implementation here */
void UARTIni(void);
bool UARTReceive(char *pData);
bool UARTSend(unsigned char *pData, unsigned char stLen);

#endif
#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

#endif	/* UART_H */

