/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#endif

#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */
#include <string.h>
#include <stdio.h>
#include <eeprom_routines.h>


//#include <plib/adc.h>       /* ADC Library*/

#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "user.h"          /* User funct/params, such as InitApp */
#include "interrupts.h"    /* Interrupt service rountine is defined here */
#include "PWM.h"
#include "UART.h"
#include "ADC.h"
#include "App.h"
#include "EEPROM.h"
#include "DAC.h"


/*Define MACRO*/
#define _XTAL_FREQ 1000000 /*1Mhz FRC internal osc */

__CONFIG(FOSC_INTOSC & WDTE_OFF & PWRTE_ON & MCLRE_OFF & BOREN_OFF & LVP_OFF
        & CPD_OFF & CP_OFF & PLLEN_ON & CLKOUTEN_OFF);

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/
extern int16_t cntBlink;
extern volatile bit flgTicTac;
extern volatile bit flgPSMC;
extern uint8_t cntPerd;
extern volatile unsigned char UART_cntTxUART;
char chBufADC[20];
int vADCAN0;
int cntPWM;
int vADCANFlt;
int vPWM = 3084;;
int vADCANOld;
int vADCANOld1;
int cntCur;
int16_t vPeakVld;
char RA7Old;
char bFlg;
char cntUARTSend;
/* i.e. uint8_t <variable_name>; */


/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/
void main(void)
{
    
    //char chRxByte;
    //int tDuty; /* storage value of Duty Cycle */
    int16_t vPeak;

    /* Configure the oscillator for the device */
    ConfigureOscillator();

    //OSCCON =  0b11110000; // 32MHz Internal Osc, SCS bits 00 for 4xPL
    /* Initialize I/O and Peripherals for application */
    InitApp();
    //vPWM = EEPROMRead(0);
    //vPWM = EEPROM_READ(0);
    vPWM = eeprom_read(0);
    DACSet(100);
    //RB1 = 1;

    //TRISA5=1;

    /* Scanning */
    //ScanFrq();

    while(1)
    {
        /* TODO <INSERT USER APPLICATION CODE HERE> */
        if (flgTicTac == true)
        {
            flgTicTac = false;

            //if(cntCur==cntPerd)
            //{
            //    //RC2 = !RC2; // TURN ON LED TO CHECK IF THE INTERRUPT WAS SERVED
            //    cntCur = 0;
            //}

            RC2 = !RC2; // TURN ON LED TO CHECK IF THE INTERRUPT WAS SERVED
            cntCur++;
            
            vADCAN0 = ReadADC(AN1);

            //vADCAN0=vADCAN0;
            
            vADCANFlt = vADCANOld+(vADCAN0-vADCANOld)/20;

            /* Blinking */
            //Blinking(vADCANFlt);

            if ((vADCANFlt==vADCANOld)&&(vADCAN0!=vADCANOld))
            {
                if ((vADCAN0>vADCANOld))
                {
                    vADCANFlt++;
                }
                else
                {
                    vADCANFlt--;
                }
            }

            if ((RA7==1)&&(RA7Old==0)) //button Press is 1, not press is 0
            {
                //PWMSetPeriod (8000-((uint32_t)7360*vADCANFlt)/4096);
                //PWMSetDutyCycle (4000-((uint32_t)7360*vADCANFlt)/8192);
                //PSMC1INT = 0x00;
                ////PWMSetPeriod (20);
                ////PWMSetDutyCycle (10);
                //(void)EEPROMWrite(vPWM,0);
                eeprom_write(0,vPWM);
            }
            else
            {
                 PSMC1INT = 0x10;
            }

            vADCANOld = vADCANFlt;
            vPWM = vADCANFlt;

            /* send ADC data to UART */
            cntUARTSend++;
            if ((cntUARTSend==50)) //1s will send data to UART
            {
               
               
               sprintf(chBufADC,"ADC:%d\r\n",vADCANFlt);
               UARTSend(chBufADC,0);
               cntUARTSend = 0;
            }
            else if ((cntUARTSend)%2==1)
            {
                /* ADC with interrupt at AN9*/
                //(void)ReadADCINT();
            }
            else
            {
                ;
            }

             /* Check Array of ADC values */
             //vPeak = ChkPeakVal();

             //if (vPeak > 0) /* ZERO is not updated */
             //{
             //    vPeakVld = vPeak;
             //}
             RA7Old = RA7;


        }


        //     if (RA5 ==1)

        
        //    PWMSetPeriod (25600-((uint32_t)24960*vADCANFlt)/4096);
        //   PWMSetDutyCycle (12800-((uint32_t)24960*vADCANFlt)/8192);

        

        //if (flgPSMC==true)
        //{
        //    PWMSetPeriod (0);
        //    PWMSetDutyCycle (0);
        //}
        //else
        //{
        //    PWMSetPeriod (25600-((uint32_t)24960*vADCANFlt)/4096);
        //    PWMSetDutyCycle (12800-((uint32_t)24960*vADCANFlt)/8192);
        //}

   
        
    
    }//while
}

