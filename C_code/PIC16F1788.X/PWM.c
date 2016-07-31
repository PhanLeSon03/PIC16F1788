#include "PWM.h"
#if 0
void PWMIni(void)
{
       /* Setting CCP1 and CCP2 pin output */
       TRISC1=0;
       TRISC2=0;

       /* Setting CCP module to PWM mode */
       CCP1CON = 0x0C;
       CCP2CON = 0x0C;

       /* Following the recommended Setting */
       PR2 = 0xFF;  /* Configure the Timer2 period */
       T2CON = 0x02; /* Set Prescaler to 16 */

       /* Initialize to zero duty cycle */
       SetPWM1(0);
       SetPWM2(0);

       TMR2ON = 1; /* Enable Timer 2, hence enable the PWM*/
}

void SetPWM1(unsigned int DutyCycle)
{
    /* Since the PWM has a resolution of 10 bits ,
       the upper 8 bits is stored in CCP1L and
       the lower 2 bits is stored in bits 5 and 4 of CCP1CON
     */
     CCPR1L = DutyCycle>>2;  /* Put MSB 8 bits in CCPR1L */
     CCP1CON &= 0xCF; /* Make bit 4 and 5 zero */
     CCP1CON |= (0x30&(DutyCycle<<4));
}
#else

void PWMSetConfig(void)
{
     //PSMC1MDLbits.P1MDLEN = true; /* PSMC Periodic Modulation Mode Enable bit */
     //PSMC1MDLbits.P1MDLPOL = true; /* PSMC Periodic Modulation Polarity bit */
     //PSMC1MDLbits.P1MSRC0 = 1; /* PxMSRC<3:0> PSMC Periodic Modulation Source Selection bits */
     //PSMC1MDLbits.P1MSRC1 = 0; /* 0101 = PSMCx Modulation Source is CCP1 */
     //PSMC1MDLbits.P1MSRC2 = 1;
     //PSMC1MDLbits.P1MSRC3 = 0;
     /* Complementary Single-phase PWM PSMC setup */
     /* Full synchronuos operation                */
     /* Period = 10 us --> 100 KHz                */

     /* enable PSMC in Complementary Single Mode: PxMODE<3:0> = 0001 */
     /* loads steering                           */
     /* time buffers                             */
     /* enables rising and falling deadbands: bit 5 and bit 4    */
     //PSMC1CON = 0b11110001;
     ANSELC = ANSELC & 0b00000000;
     PWMSetDutyCycle(32000);
     PWMSetPeriod(64000);
     PWMSetPhase(0);    

     /* No phase offset */
     //PSMC1PHH = 0;
     //PSMC1PHL = 0;

     /* PSMC clock = 64 MHz */
     /* Clock source set either */
     /* Internal 64 MHz clock */
     /* Fosc system clock     */
     /* External clock input pin */
     /* Bit 1-->0: PxCSRC <1:0> */
     /* 01: 64MHz clock in from PLL */
     /* 11: Reserved */
     /* 10: PSMCxCLK pin */
     /* 00: Fosc system clock */
     /* Bit 6-->5: PxCPRE */
     /* Clock Prescale : PxCPRE<1:0> */
     /* 00:Devide by 1 */
     /* 01:Devide by 2 */
     /* 10:Devide by 4 */
     /* 11:Devide by 8 */
     PSMC1CLK = 0x01; //01 --> 64 MHz from PLL
                      //00 --> Fosc system clock

     /* 23-June: Interrupt configuration */
     PSMC1INT = 0x10; // Bit 4: PxTPRIE: PSMC Time Base Period Interrupt Enable bit
                      // Bit 0: PxTPRIF: PSMC Time Base Period Interrupt Flag bit

     /* Output on PortC, normal polarity */
     PSMC1OEN = 0x03;  /*Bit 5->0: Enable F,E,D,C,A,B */
     PSMC1STR0 = 0x03; /*Bit 5->0: Steering Control F,E,D,C,A,B */
     PSMC1POL = 0x00;  /*Bit 5->0: Polarity Control F,E,D,C,A,B */
     //PSMC1STR0bits.P1STRC = 1; /* Steering */
     //PSMC1POLbits.P1POLC = 0; /* Polarity is set with the PSMC Polarity Control */
     //PSMC1OENbits.P1OEC = 1;  /* PSMC Output Enable Control */
     /* RC0 -> PSMC1A */
     /* RC1 -> PSMC1B */

     /* Time base source for all even */
     PSMC1PRSbits.P1PRST = 1;/* Period event will occur and PSMCxTMR will reset when PSMCxTMR = PSMCxPR */
     PSMC1PHSbits.P1PHST = 1;/* Rising edge event will occur when PSMCxTMR = PSMCxPH */
     PSMC1DCSbits.P1DCST = 1;/* Falling edge event will occur when PSMCxTMR = PSMCxDC*/

     /* set rising and falling dead-band times */
     PSMC1DBR = 0; /*63 for rising (x+1)*15.625 */
     PSMC1DBF = 0; /*63 for falling */


     /* Enable PSMC in complementary Single Mode */
     PSMC1CON = 0b11110001;    /* Bit 3->0: Mode */
                               /* 0001: Single PWM with complementary output */
                               /* Bit 4:PxDBRE:PSMC Rising Edge Dead-Band Enable bit */
                               /* Bit 5:PxDBFE:PSMC Falling Edge Dead-Band Enable bit */
                               /* Bit 6:PSMCxLD:PSMC Load Buffer Enable bit */
                               /* Bit 7:PSMCxEN:PSMC Module Enable bit */

     /* Setting CCP0 and CCP1 pin output */
     TRISC0 = 0; /* output PWM */
     TRISC1 = 0; /* output PWM */

}

/* Create Rising edge event */
void PWMSetPhase(uint16_t cntPhase)
{
    PSMC1PHL = cntPhase & 0x00FF;
    PSMC1PHH = cntPhase>>8;

    //PSMC1LD = true;
    /* PxTPHIF is set when comparation is true */
}

void PWMSetPeriod(uint16_t cntPeriod )
{
    /* Period = (PSMCxPR[15:0] + 1)/Fpsmc_clk */
    PSMC1PRL = cntPeriod & 0x00FF;
    PSMC1PRH = cntPeriod>>8;

    //PSMC1LD = true;

    /* PxTPRIF bit is set when comparation is TRUE*/
}

void PWMSetDutyCycle (uint16_t cntDutyCycle)
{
    /* DUTYCYCLE = (PSMCxDC[15:0]-PSMCxPH[15:0])/(PSMCxPR[15:0] + 1) */

    PSMC1DCL = cntDutyCycle & 0x00FF;
    PSMC1DCH = cntDutyCycle>>8;

    PSMC1LD = true;
    /* PxTDCIF is set when comparation is true */
}

void PWMIni(void)
{
    PWMSetConfig();

}

void SetPWM1(unsigned int DutyCycle)
{
     /* Config PWM duty cycle                               */
     /* PWM duty =(CCPRxL:CCPxCON<5:4>)*Tosc*(TM2 prescalse)*/
     /* 1.000us  = (250)*0.25us*16                          */
     CCPR1L = DutyCycle>>2;
     CCP1CON =((DutyCycle<<4)&(0b00110000));
     
     /* Setting CCP module to PWM mode */
     CCP1CON |= 0x0C;             /* Bit3-0:11xx : PWM mode*/
}

void SetPeriod (unsigned int rPeriod)
{
    /* Period = (PSMCxPR[15:0]+1)/Fpsmc */
    /* PSMCxPRL: PSMC PERIOD COUNT LOW BYTE REGISTER    */
    /* PSMCxPRH: PSMC PERIOD COUNT HIGH BYTE REGISTER   */
    PSMC1PRL = rPeriod & 0x00FF;
    PSMC1PRH = rPeriod>>8;

    /* Duty Cycle 50% */
    /* DUTYCYCLE = (PSMCxDC[15:0]- PSMCxPH[15:0])/ (PSMCxPR[15:0]-1)*/
    /* PSMCxDCL: PSMC DUTY CYCLE COUNT LOW BYTE REGISTER            */
    /* PSMCxDCH: PSMC DUTY CYCLE COUNT HIGH REGISTER                */
     PSMC1DCL = (rPeriod>>2)& 0x00FF;
     PSMC1DCH = rPeriod>>9;
}

void SetDeadTime(unsigned char rDeadTime)
{
     /* set rising and falling dead-band times */
     PSMC1DBR = rDeadTime; /* for rising (x+1)*15.625 */
     PSMC1DBF = rDeadTime; /* for falling */

     PSMC1LD = true;
    /* PxTDCIF is set when comparation is true */
}
#endif