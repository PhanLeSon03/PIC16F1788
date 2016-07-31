#include "ADC.h"

extern volatile int16_t ADC_vADC;
extern volatile int16_t ADC_vADCAry[ADC_LARRAY];
extern volatile bit flgADCAryFull;
extern char ADCON0Tmp;
int32_t Sum_e;
int32_t vAvg, vAvg_Old;

void ADCINTIni(char ANx, char Fosc)
{
    if (ANx<8)
    {
        TRISA = 0b00000001<<ANx;    /* RAx is input pin */
        ANSELA = 0b00000001<<ANx;   /* Analog of ANx is using */
    }
    else
    {
        TRISB = 0b00000001<<(ANx-7);    /* RBx is input pin */
        ANSELB = 0b00000001<<(ANx-7);   /* Analog of ANx is using */
    }
    ADCON1 = 0b10000000; /* Bit 7: ADFM 1 --> 2's complement */
                         /* Bit 6-4: ADCS<2:0> = 000--> Fosc/2 --> 62.5ns */
                         /* Bit 3: always ZERO */
                         /* Bit 2: 0 --> Vref- is connected to Vss */
                         /* Bit 1-0: ADPREF<1:0>=00--> Vref+ is connect to Vdd*/

    ADCON0 = (ANx<<2) & 0b01111111;
                               /* Bit 7:0 --> 12 bits result */
                               /* Bit 6-2: Input channel */
                               /* Bit 1: GO=0 --> not completed */
                               /* Bit 0: ADON        */
                               /*         1: ENABLE  */
                               /*         0: DISABLE */
    
    ADCON2 = 0b00001111;  /* Bit 7-4: Auto-Conversion Trigger  */
                    /*         TRIGSEL<3:0>=0000:Disable */
                    /* Bit 3-0: Negative Differential Input Channel Select bits*/
                    /* CHSN<3:0>:0000 --> AN0 */


    /* clear Interrupt flag */
    ADIF = 0;
    /* ADC interrupt enable*/
    ADIE = 1;   /* ADIF bit is set at the complete of every conversion */

    ADON = 1;
    /* 12-bit 2's compliment */
    //ADFM = 1;
    //ADRMD = 0;

    /* Wait the required acquisition time */
    _delay(100);

    GO=1;
}
#if 0

void ADCIni(char ANx , char Fosc)
{
    if (ANx < 8)
    {
        TRISA = TRISA | (0b00000001<<ANx);     /* RAx is input pin*/
        ANSELA = ANSELA | (0b00000001<<ANx);   /* Analog of ANx is using*/
    }
    else
    {
        TRISB = TRISB | (0b00000001<<(ANx-7));     /* RBx is input pin*/
        ANSELB = ANSELB | (0b00000001<<(ANx-7));   /* Analog of ABx is using*/
    }
    ADCON1 = 0b10000000;/* Vdd and Vss Vref */
                        /* Bit 7: ADFM = 1: 2?s complement */
                        /* ADFM = 0: Sign-magnitude result format */
                        /* Bit 6-4: ADCS<2:0> :ADC Conversion Clock Select bits */
                        /*                     000--> Fosc/2 --> 62.5ns*/
                        /* 111:Frc-->lock supplied from a dedicated FRC oscillato */
                        /* Bit 3: Unimplemented */
                        /* Bit 2: ADNREF: ADC Negative Voltage Reference Configuration bit */
                        /* 1 =VREF- is connected to external VREF- pin */
                        /* 0 =VREF- is connected to VSS */
                        /* Bit 1-0: ADPREF<1:0>: ADC Positive Voltage Reference Configuration bits */
                        /* 11 =VREF+ is connected internally to FVR Buffer 1 */
                        /* 10 = Reserved */
                        /* 01 =VREF+ is connected to VREF+ pin */
                        /* 00 =VREF+ is connected to VDD */



    ADCON2 = 0b00001111;
                       /* Bit 7-4:TRIGSEL<3:0>: ADC Auto-conversion Trigger Source Selection bits */
                       /* 0000: Disable */
                       /* Bit 3-0 CHSN<3:0>: Negative Differential Input Channel Select bits */
                       /* 1111 = ADC Negative reference ? selected by ADNREF */

 }

int ReadADC(char ANx)
{
    int vADC;
    uint8_t Temp;

    /* ADC interrupt disable */
    ADIE = 0;   /* ADIF bit is set at the complete of every conversion */
   
    //Temp = ADCON0;
    ADCON0 = ((ANx<<2) & 0b01111100);
                    /* Bit 0: ADON =1 */
                    /* Bit 1: GO/DONE: ADC Conversion Status bit */
                    /*       1: in progress   */
                    /*       0: completed     */
                    /* Clear by hardware */
                    /* Bit 6-2: CHS<4:0>: Positive Differential Input Channel Select bits */
                    /* 00001-->AN1 */

    ADON = 1;

    /* Wait the required acquisition time */
    _delay(200);

    GO=1;

    while(!GO) ;
    //_delay(200);
    vADC = 0;
    vADC = ADRESH;
    vADC <<=8;
    vADC |=ADRESL;

    
    ADIF = 0; /* The ADIF bit must be cleared in software*/
    ADIE = 1;   /* ADIF bit is set at the complete of every conversion */

    ADCON0 = Temp;
    /* ADC interrupt enable*/

    return vADC;
}

#else
void ADCIni(char ANx , char Fosc)
{
    TRISA = TRISA | (0b00000001<<ANx);     /* RAx is input pin*/
    ANSELA = ANSELA | (0b00000001<<ANx);   /* Analog of ANx is using*/
    ADCON1 = 0b11110000;/* Vdd and Vss Vref */
                        /* Bit 7: ADFM = 1: 2?s complement */
                        /* ADFM = 0: Sign-magnitude result format */
                        /* Bit 6-4: ADCS<2:0> :ADC Conversion Clock Select bits */
                        /* 111:Frc-->lock supplied from a dedicated FRC oscillato */
                        /* Bit 3: Unimplemented */
                        /* Bit 2: ADNREF: ADC Negative Voltage Reference Configuration bit */
                        /* 1 =VREF- is connected to external VREF- pin */
                        /* 0 =VREF- is connected to VSS */
                        /* Bit 1-0: ADPREF<1:0>: ADC Positive Voltage Reference Configuration bits */
                        /* 11 =VREF+ is connected internally to FVR Buffer 1 */
                        /* 10 = Reserved */
                        /* 01 =VREF+ is connected to VREF+ pin */
                        /* 00 =VREF+ is connected to VDD */

    ADCON2 = 0b00001111;
                       /* Bit 7-4:TRIGSEL<3:0>: ADC Auto-conversion Trigger Source Selection bits */
                       /* 0000: Disable */
                       /* Bit 3-0 CHSN<3:0>: Negative Differential Input Channel Select bits */
                       /* 1111 = ADC Negative reference ? selected by ADNREF */

    ADCON0 = (ANx<<2) & 0b01111100;
                      /* Bit 0: ADON =1 */
                      /* Bit 1: GO/DONE: ADC Conversion Status bit */
                      /*       1: in progress   */
                      /*       0: completed     */
                      /* Clear by hardware */
                      /* Bit 6-2: CHS<4:0>: Positive Differential Input Channel Select bits */
                      /* 00001-->AN1 */
    ADON = 1;
    _delay(100);
    GO = 1;
}

int ReadADC(char ANx)
{
    int vADC;

    //ADCON0 = ADCON0Tmp;
    ADCON0 = (ADCON0&0b10000011)|((ANx<<2) & 0b01111100);
    ADON = 1;
    _delay(200);
    GO = 1;
    while (DONE){};

    vADC = ADRESH;
    vADC <<=8;
    vADC |=ADRESL;

    ADIF = true;
    GO=1;

    return vADC;
}

#endif
/* Reading is implemented in interrupt service rountine*/
int16_t ReadADCINT(void)
{

    if (flgADCAryFull==true)
    {
        ADIE = 1; /* Enable Interrupt */

        ADON = 1;

        //flgADCAryFull = false;

        /* Wait the required acquisition time */
        _delay(200);

        GO=1;

        TXIE = 0;/* Disable UART Transmission */
    }
    return ADC_vADC;
}

int16_t ChkPeakVal(void)
{
    int16_t tmpMAX;

    tmpMAX = 0; /* Initialize */

    if (flgADCAryFull==true) /* Data in array are ready*/
    {
        for (char i=0;i<ADC_LARRAY;i++)
        {
            if(ADC_vADCAry[i]>tmpMAX)
            {
                tmpMAX = ADC_vADCAry[i];
            }
        }

        return tmpMAX;
    }
    else    /* data in array are not ready*/
    {
        return 0;
    }
}

int16_t SearchingOffSet(void)
{

    int32_t error, temp;
    static int32_t uVout, error_old;

    //if (flgADCAryFull==true) /* Data in array are ready*/
    //{
    //    flgADCAryFull = false;

        //vAvg = 0;
        
        //for (char i=0;i<ADC_LARRAY;i++)
        //{
        //    vAvg+=ADC_vADCAry[i];
        //}

        //vAvg = vAvg/ADC_LARRAY;

        vAvg = ReadADC(AN9);

        temp = vAvg;

        vAvg = vAvg_Old +(vAvg-vAvg_Old)/10;

        if ((vAvg==vAvg_Old)&&(temp!=vAvg))
        {
            if ((temp>vAvg))
            {
                vAvg++;
            }
            else
            {
                vAvg--;
            }
        }

        error = -(ADC_THRESSENS - vAvg);

        Sum_e = Sum_e + error;

        uVout = error*KD + Sum_e*KI + ((error-error_old)/dT)*KD;

        //if (error < 0)
        //{
        //    uVout++;
        //}
        //else
        //{
        //    uVout--;
        //}
        
        if (uVout > ADC_MAX)
        {
            uVout = ADC_MAX;
        }
        else if (uVout < ADC_MIN)
        {
            uVout = ADC_MIN;
        }
        else
        {
            ;
        }

        error_old = error;
    //}

    return uVout;
}