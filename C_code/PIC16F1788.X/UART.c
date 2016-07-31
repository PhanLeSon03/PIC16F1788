#include "UART.h"

#if _POLLING
/* static variable */
static unsigned char cntTxUART;
static unsigned char cntRxUART;
char chRxBuf[10];
//static char chTxBuf[100];


void UARTIni(void)
{
    //char temp;

    /* RB5: Tx : 0utput */
    TRISB5 = 1;
    /* RB2: Rx : Input */
    TRISB2 = 1;
    /* Baund rate: 19200 bps */
    SPBRG = 12; /* System Clock : 4Mhz */

    BRGH = 1; /* High Speed */
              /* Baund Rate = Fosc/(16(X+1))*/

    SYNC = 0; /* Asynchronous */
    SPEN = 1; /* Serial enable*/
    //TXIE = 1; /* Enable Interupte when TXREG --> TSR */
    //TX9 = 1;  /* config 9 bit */

    /* Noise reading */
    //temp = RCREG;  /* RCIF is clear by this action */

    /* start Tranmit data */
    TXEN = true;

}

bool UARTSendByte(char chOneChar)
{
    /* RB5: transmit Pin */

    //_delay(25);

    while (TXIF != true) /* TXREG is not free */
    {
        ;
    }
    /* Copy data to buffer*/
    TXREG = chOneChar ; /* upload data to Transmit Register  */
                        /* TXIF is clear when TXREG <-- data */
}


bool UARTReceiveByte(char * pRxByte)// </editor-fold>
{
    /* RB2: Recieve Pin */
    /* Continueous Receive Enablebit */
    CREN = true;

    if (RCIF = true) /* RCIF=TRUE when RSR --> RCREG */
    {
        *pRxByte = RCREG;  /* RCIF is clear by this action */
        return true;
    }
    else
    {
        return false;
    }

}

/* Send string function */
void UARTSend(char *pData, unsigned char stLen)
{
    if (stLen == 0)
    {
        while (*(pData + cntTxUART) != '\n')
        {
            UARTSendByte(*(pData + cntTxUART)); /* Send Byte by Byte*/
            cntTxUART++;
        }
        UARTSendByte(*(pData + cntTxUART));
        cntTxUART = 0; /* Reset Transmit counter */
    }
    else
    {
        for(int i=0; i<stLen;i++)
        {
            UARTSendByte(*(pData + i));
            cntTxUART++;
        }
        cntTxUART=0; /* Reset Transmit counter */

    }
}

/* Receive string function */
int8_t *UARTReceive(void)
{
    /* code reserve*/


    return chRxBuf;
}

#else
/* Interrupt implementation style will implementation here */

/* global variable */
extern volatile unsigned char UART_cntTxUART;
extern volatile unsigned char UART_cntRxUART;
extern volatile char UART_chRxBuf[20];
extern volatile char UART_chTxBuf[20];

void UARTIni(void)
{
    /* RC6: Tx : 0utput */
    TRISC6 = 1;
    /* RB2: Rx : Input */
    TRISC7 = 1;
    /* Baund rate: 19200 bps */
    /* System Clock : 32Mhz */
    /* Desired Baud Rate = Fosc/(64*[SPBRGH:SPBRGL]+1)*/
    SPBRGH = 0;
    SPBRGL = 68;

    BRGH = 1; /* High Speed */
              /* Baund Rate = Fosc/(4(X+1))*/

    BRG16 = 1; /* 1: 16 bit Baund Rate Generator is used */
               /* 0: 8 bit Baund Rate Generator is used */
    SYNC = 0; /* Asynchronous */
    SPEN = 1; /* Serial enable*/
    //TXIE = 1; /* Enable Interupte when TXREG --> TSR */
    //RCIE = 1;   /* Recieve interrupt enable:*/
    //TX9 = 1;  /* config 9 bit */

    /* The analog I/O must be disable by clearing ANSEL bit*/
    //TODO:

    /* Continueous Receive Enablebit */
    CREN = true;

}

bool UARTReceive(char *pData)
{
    if (UART_cntRxUART != 0)
    {

        for(uint8_t i = 0;i<UART_cntRxUART;i++)
        {
            *pData = UART_chRxBuf[i]; /* Copy data from buffer */
        }

        UART_cntRxUART = 0; /* Restart counter in buffer */

        return true;
    }
    else
    {
        return false;
    }
}

bool UARTSend(unsigned char *pData, unsigned char stLen)
{
    if (UART_cntTxUART ==0)
    {
        if (stLen == 0)
        {
            while (*(pData + UART_cntTxUART) != '\0')
            {
                UART_chTxBuf[UART_cntTxUART]=pData[UART_cntTxUART]; /* Update data to buffer*/
                UART_cntTxUART++;
            }

            //UART_chTxBuf[UART_cntTxUART++] = '\n'; /* Update the last character */
        }
        else
        {
            for(int i=0; i<stLen;i++)
            {
                UART_chTxBuf[i] = pData[i];
                UART_cntTxUART++;
            }
        }

        /* Enable UART transmittion Interrupt */
        TXIE = true;

        /* start Tranmit data */
        TXEN = true;  /* TXIF is set after this action */

        return true;
    }
    else
    {
        return false;
    }
}
#endif