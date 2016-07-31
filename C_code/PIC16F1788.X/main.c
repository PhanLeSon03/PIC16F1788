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
#define MAIN_STATE1 50     /*  1.0 s  */
#define MAIN_STATE2 25     /* 0.25 s  */
#define MAIN_STATE3 12     /* 0.125 s */
#define MAIN_STATE4 6      /*  */
#define MAIN_THRES4 (900) /* very near*/
#define MAIN_THRES3 (400)  /* near */
#define MAIN_THRES2 (150)  /* far  */
#define MAIN_THRES1 (40)   /* present*/
#define MAIN_THRES11  (90) /* To check threshold */
#define MAIN_STEPDWN  (60) /* step down */
#define MAIN_DIFF   100
#define MAX   4095
#define THRES 100
#define READADC                                                                \
{                                                                              \
                 Vin = ReadADC(AN9);                                           \
                 for (char i=0; i<8;i++)                                       \
                 {                                                             \
                     Vin8 = Vin7;                                              \
                     Vin7 = Vin6;                                              \
                     Vin6 = Vin5;                                              \
                     Vin5 = Vin4;                                              \
                     Vin4 = Vin3;                                              \
                     Vin3 = Vin2;                                              \
                     Vin2 = Vin;                                               \
                     Vin = ReadADC(AN9);                                       \
                     _delay(1000);                                             \
                 }                                                             \
               Vavr = (Vin + Vin2 + Vin3 + Vin4 + Vin5 + Vin6 + Vin7 + Vin8)/8;\
}                                                                              \

#define PWMUPDATE                                                              \
{                                                                              \
        PWMSetPeriod (64000000/(vPWM+100));                                    \
        PWMSetDutyCycle(32000000/(vPWM+100));                                  \
        _delay(200000);                                                        \
}                                                                              \


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
extern char ADCON0Tmp;
extern int32_t vAvg;


unsigned char chBufADC[20];
int vADCAN0;
int cntPWM,cntStepDwn;
int vADCANFlt,vSenOff;
uint16_t vPWM, vPWM_old;
int vADCANOld;
int vADCANOld1;
int cntCur,cntBIAS,cntNoPres;
int16_t vPeakVld;
char RA7Old;
char bFlg,flgOffRls,flgStepDwn;
char cntUARTSend,cntDiff;
static char Voff,Voff_old;
int Vin, Vavr_max, Vavr_old, Vavr, Vin2, Vin3, Vin4, Vin5, Vin6, Vin7, Vin8;
int vSenDiff, Voff_Old;
char stMAX,flgTurnOnDly_Offset, cntTurnOnDly_Offset;
uint32_t cntOS;
/* i.e. uint8_t <variable_name>; */


/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/
void main(void)
{
    
    //char chRxByte;
    char stBlinking = 0;
    char stFinish = 0;

    //int tDuty; /* storage value of Duty Cycle */
    int16_t vPeak;
    int16_t vDAC;
    uint16_t temp;

    static int16_t vADCAN9,vADCAN9_Old,vADCAN9_mark,vUART,vUART_off;

    /* Configure the oscillator for the device */
    ConfigureOscillator();

    //OSCCON =  0b11110000; // 32MHz Internal Osc, SCS bits 00 for 4xPL
    /* Initialize I/O and Peripherals for application */
    InitApp();

    ADCON0Tmp = ADCON0;

    
    //TRISA5=1;

    /* Scanning */
    //ScanFrq();

    DACSet(0);

    //if (RA7==0)
    {
        //while((vAvg<=(ADC_THRESSENS-5))||(vAvg>=(ADC_THRESSENS+5)))
#if 1

        Voff = 0;
        DACSet(Voff);
        vPWM = 0;
        PWMUPDATE

        //if (Voff<250)    Voff +=10;

        DACSet(Voff);
         _delay(200000);
        
         while (!stFinish)
         {  
             READADC
             if (Vavr < MAX - THRES)  //done for finding thredholf
             {
                 if (Vavr < Vavr_old)  // in other side of the hill
                 {
                    vPWM +=100;
                 }
                 else
                 {
                    vPWM +=10;
                 }
                 PWMUPDATE
                 if (vPWM > 20000)
                 {
                     vPWM = vPWM_old;
                     Voff = Voff_old;
                     DACSet(Voff);
                     PWMUPDATE
                     stFinish = 1;
                 }
                 else
                 {
                     //vPWM+=10;
                     //PWMUPDATE
                 }
                 if (stMAX==false)
                 {
                     if ((Vavr > Vavr_max)&&(Vavr_max<MAX - THRES)) /* use for small signal */
                     {
                         Vavr_max = Vavr;
                         vPWM_old = vPWM;
                     }
                 }
                 
                 
             }
             else // in MAX value
             {
                  while (Vavr > ADC_THRESSENS) //pull the curve down
                  {
                         if (Voff<250) Voff +=1;
                         DACSet(Voff);
                         _delay(30000);
                         READADC
                  }

                  if (Voff > Voff_old)
                  {
                     Voff_old = Voff;
                     vPWM_old = vPWM;
                  }
                 
                 vPWM+=10;
                 PWMUPDATE

                 stMAX = true;
             }

             Vavr_old = Vavr; /* Used for Increae the speed of searching */
             //sprintf(chBufADC,"PWM:%d\r\n",vPWM);
             //UARTSend(chBufADC,0);

             //sprintf(chBufADC,"V Offset:%d\r\n",Voff);
             //while(!UARTSend(chBufADC,0));

             sprintf(chBufADC,"%d\r\n\n",Vavr);
             while(!UARTSend(chBufADC,0));

         }


#endif
    }
    
    while(1)
    {

        /* TODO <INSERT USER APPLICATION CODE HERE> */
        if (flgTicTac == true)
        {
            flgTicTac = false;
       
            cntCur++;
            cntOS++;
            
            vADCAN0 = ReadADC(AN1);
            vADCAN9 =   ReadADC(AN9);

            temp = vADCAN9;

            /* Filtering the ADC sensor input ------------------------------- */
            vADCAN9 = vADCAN9_Old +(vADCAN9-vADCAN9_Old)/15;

            if ((vADCAN9==vADCAN9_Old)&&(temp!=vADCAN9))
            {
                if ((temp>vADCAN9))
                {
                    vADCAN9++;
                }
                else
                {
                    vADCAN9--;
                }
            }
            /*----------------------------------------------------------------*/


            /* Offset value release to update whenever flgTurnOnDly_Offset==true -------------*/
            if ((flgOffRls==true)&&(flgTurnOnDly_Offset==true)) /* 100*5000/4095 ~ 125mV */
            {
                //vSenOff = vADCAN9_mark;
                flgOffRls = false;
            }
            /* ---------------------------------------------------------------*/

            /* Threshold update ------------------------------------------------*/
            if ((((vADCAN9 > vSenOff + (MAIN_DIFF))&&(vADCAN9_Old > vSenOff + (MAIN_DIFF))))||(cntNoPres>63))
            {
               vSenOff = vADCAN9;
               cntNoPres = 0;
               cntDiff = 0;
               vSenDiff = 0;
            }
            
            /* Detect the change value in Sensor Value -----------------------*/
           cntDiff++;
           if (cntDiff == 8)
           {
               if (bFlg) /* Compensation the Bias */
               {
                   bFlg = false;
                   vSenOff = vADCAN9;
               }
               vSenDiff = vSenOff - vADCAN9;
               cntDiff = 0;
           }
           /*-----------------------------------------------------------------*/

            if (vADCAN9 > vSenOff - (MAIN_DIFF))
            {
                flgOffRls = true;
            }

           /* Deboucing for different vSenDiff > MAIN_THRES1------------------*/
           /* In case the object present in very slow speed ------------------*/
           if ((vSenDiff > MAIN_THRES1))
            {
                if (cntTurnOnDly_Offset<255) cntTurnOnDly_Offset++;
            }
            else
            {
                cntTurnOnDly_Offset = 0;
                vADCAN9_mark = vADCAN9;
            }

            if (cntTurnOnDly_Offset>5)
            {
                flgTurnOnDly_Offset = true;
            }
            else
            {
                flgTurnOnDly_Offset = false;
            }
            /*----------------------------------------------------------------*/

            /* ------ Check for Step Down ------------------------------------*/
           if (vSenDiff > MAIN_STEPDWN)
           {
               if (cntStepDwn<255)cntStepDwn++;
           }

           if (vSenDiff < MAIN_STEPDWN/2)
           {
               cntStepDwn = 0;
           }

           if (cntStepDwn>4)
           {
               flgStepDwn = true;
           }

            /* ---------------------------------------------------------------*/

           /* Update Offset value here ---------------------------------------*/
            if ((vADCAN9> ADC_THRESSENS+3*THRES)
                 &&(vADCAN9_Old> ADC_THRESSENS+3*THRES)
                 &&(bFlg==false))
            {
                    if (Voff<250) Voff +=1;
                    DACSet(Voff);
                    //READADC
                    //vSenOff = vADCAN9 -300;
                    bFlg = true;
                    cntDiff =-50;
                    cntNoPres =-10;
                    vSenDiff = 0;
                    stBlinking = 0; /* not present */
            }
            /* LED Blinking Judgment------------------------------------------*/
            else
            {
                if ((vSenDiff > MAIN_THRES4))
                {
                    stBlinking = 4; /* very near*/
                }
                else if ((vSenDiff > MAIN_THRES3))
                {
                    stBlinking = 3; /* near */
                }
                else if ((vSenDiff > MAIN_THRES2))
                {
                    stBlinking = 2; /* far */
                }
                else if ((vSenDiff > MAIN_THRES11)||(flgTurnOnDly_Offset==true)||(flgStepDwn))
                {
                    stBlinking = 1; /* present */
                }
                else
                {
                    stBlinking = 0; /* not present */
                }
            }
           /*-----------------------------------------------------------------*/


            /* Blinking handle*/
           switch (stBlinking)
           {
                case 1:/* Present */
                {
                    if (cntCur > MAIN_STATE1)
                    {
                        RC2 = !RC2; // TURN ON LED TO CHECK IF THE INTERRUPT WAS SERVED
                        cntCur = 0;
                    }
                    cntNoPres=0;
                    flgStepDwn = 0;
                    flgTurnOnDly_Offset = 0;
                    cntTurnOnDly_Offset = 0;
                    cntStepDwn = 0;
                    break;
                }
                case 2: /* far */
                {
                    if (cntCur > MAIN_STATE2)
                    {
                        RC2 = !RC2; // TURN ON LED TO CHECK IF THE INTERRUPT WAS SERVED
                        cntCur = 0;
                    }
                    cntNoPres=0;
                    break;
                }
                case 3: /* near*/
                {
                    if (cntCur > MAIN_STATE3)
                    {
                        RC2 = !RC2; // TURN ON LED TO CHECK IF THE INTERRUPT WAS SERVED
                        cntCur = 0;
                    }
                    cntNoPres=0;
                    break;
                }
                case 4:/* very near*/
                {
                    if (cntCur > MAIN_STATE4)
                    {
                        RC2 = !RC2; // TURN ON LED TO CHECK IF THE INTERRUPT WAS SERVED
                        cntCur = 0;
                    }
                    cntNoPres=0;
                    break;
                }
                default: /* metal in far field */
                {
                    RC2 = 0;
                    cntCur = 0;
                    cntNoPres++;
                }
           }

           if ((vADCAN9 < THRES)&&(vADCAN9_Old<THRES)) //pull the curve up
           {
                     //if (Voff>0) Voff-=1;
                     //DACSet(Voff);
           }

            vADCANFlt = vADCANOld+(vADCAN0-vADCANOld)/15;

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

            vADCANOld = vADCANFlt;
            //vPWM = vADCANFlt;
            //if ((vADCANFlt >= (vPWM+3))||((vADCANFlt <= (vPWM-3))))
            //{
            //vPWM = vADCANFlt;
                           /* Update Frequency only when PA1 is pressed*/
            //}



            if (RA7==1)
            {
                PWMSetPeriod (64000-((uint32_t)63872*vPWM)/2048);//1Khz -> 50Khz
                PWMSetDutyCycle (32000-((uint32_t)63872*vPWM)/4096);
            }


            //DACSet((uint8_t)(vPWM>>4));


            /* send ADC data to UART */
            cntUARTSend++;
            if (cntUARTSend%4 ==0)
            {
               vUART = vADCAN9;
               vUART_off = vSenOff -MAIN_THRES11;
               sprintf(chBufADC,"%0.4dA%0.4d\r\n",vUART,vUART_off);
               UARTSend(chBufADC,0);
            }
            else if (cntUARTSend%3 == 0)
            {
               sprintf(chBufADC,"%0.4dA%0.4d\r\n",vUART,vUART_off);
               UARTSend(chBufADC,0);
            }
            else if (cntUARTSend%2 == 0)
            {
               sprintf(chBufADC,"%0.4dA%0.4d\r\n",vUART,vUART_off);
               UARTSend(chBufADC,0);
            }
            else
            {
               sprintf(chBufADC,"%0.4dA%0.4d\r\n",vUART,vUART_off);
               UARTSend(chBufADC,0);
            }

            if ((RA7==0)&&(RA7Old==1)) //button Press is 1, not press is 0
            {
                (void)eeprom_write(0x06,vPWM);/* Write to EEPROM*/
                (void)eeprom_write(0x07,(vPWM>>8));/* Write to EEPROM*/
            }
           
           RA7Old = RA7;
           Voff_Old = Voff;
           vADCAN9_Old = vADCAN9;

       }//(flgTicTac==1)

        /* Update Frequency*/
        if ((cntOS%1000==0)&&(stBlinking==0))
        {
            Vavr_max = vADCAN9;
            for (char i=-10;i<10;i++)
            {
                vPWM= vPWM+i;
                PWMUPDATE
                READADC
                if (Vavr > Vavr_max)
                {
                    Vavr_max = Vavr;
                    vPWM_old = vPWM;
                }
            }
            vPWM = vPWM_old;
            PWMUPDATE
//            while (Vavr > ADC_THRESSENS) //pull the curve down
//            {
//                if (Voff<250) Voff +=1;
//                DACSet(Voff);
//                _delay(30000);
//                READADC
//            }
            vSenOff = Vavr;
            vSenDiff = 0;
        }

    } //while(1)
}

