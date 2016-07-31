#include "App.h"

#define APP_VTHRES1    4096
#define APP_VTHRES2    3000
#define APP_VTHRES3    2000

int16_t cntBlink;

static int16_t vSenOld;


bool ScanFrq(void)
{
    int16_t vSen;
    /* Set Initial Frequency */

    /* Delay */

    /* Read ADC */
    vSen = ReadADC(AN1);

    /* check valid rannge */

    if ((0<=vSen)&&(vSen<=4096))
    {
        if (vSen>vSenOld)
        {
            do
            {
                /* Update the old value of ADC */
                vSenOld = vSen;

                /* Increase frequency */

                /* Delay  */

                /* Read ADC*/
                vSen = ReadADC(AN1);
            }
            while (vSen>vSenOld);
        }
        else
        {
            do
            {
                /* Update the old value of ADC */
                vSenOld = vSen;

                /* Decrease frequency */

                /* Delay  */

                /* Read ADC*/
                vSen = ReadADC(AN1);
            }
            while (vSen>vSenOld);
        }

        return true;
    }
    else
    {
        return false;
    }
}

void Blinking(int vSenFlt)
{

    if ((vSenFlt <= APP_VTHRES1))
    {
        /* No Blinking*/
        Blink(0);
    }
    else if ((vSenFlt <= APP_VTHRES2))
    {
        /* Blinking 2s */
        Blink(2);
    }
    else if ((vSenFlt <= APP_VTHRES2))
    {
        /* Blinking 1s */
        Blink(1);
    }
    else
    {
        /*Bkiling 0.5s */
        Blink(0.5);
    }
}

void Blink(float tiPerd)
{
    cntBlink = (int16_t)50*tiPerd; /* 1000ms/10ms/2 */
}