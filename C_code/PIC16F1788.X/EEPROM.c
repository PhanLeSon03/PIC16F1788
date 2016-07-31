
/* There are six SFRs used to access there memory:
 * EECON1
 * EECON2
 * EEDATL : hold 8 bits data for read/write
 * EEDATH
 * EEADRL : hold the address of EEDATL
 * EEASRH
 */
/******************************************************************************/
/*Files to Include                                                            */
/******************************************************************************/

#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#endif

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */
#include "EEPROM.h"

#define EEPROM_HIADDRESS 0x20
#define EEPROM_LOADDRESS 0x00

/*
 * EEPGD:0 -> EEPROM
 *       1 -> Program memory
 * RD and WR: can not be cleared by SW
 * WREN: allow a write operation occur
 * WRERR: is set when the write operation is interrupted by a reset during
 * normal mode. can check to execute the approriate error handling
 * EEIIF (PIR2): interrupt flag -> is set when write complete, must be clear by
 * SW
 *
 */
char EEPROMWrite(uint16_t Data, uint8_t Address)
{
    static char cntOverTime;

    //EEADRH = EEPROM_HIADDRESS;
    //EEADRL = EEPROM_LOADDRESS;
    /* write the address to the EEADRL register and the datato the EEDATL
     * register*/
    //EEADRL = Address; /* Low byte of address */
    EEDATA = Address;
    //EEDATL = (uint8_t)(Data|0x00FF);
    //EEDATH = (uint8_t)(Data>>8);
    EEADR = Data;

 
    CFGS = 0;                  /* Deselect Config state */
    EEPGD = 0;                 /* Pointer to DATA memory*/
    WREN = true ;              /* Write Enable */

    di();
    /* Sequence of Write :
     * Write 55h to EECON2
     * Write AAh to EECON2
     * Set WR bit
     */
    EECON2 = 0x55;
    EECON2 = 0xAA;
    WR = true;
//#asm
//     MOVLW 0x55
//     MOVWF EECON2
//     MOVLW 0xAA
//     MOVWF EECON2
//     BSF EEC0N1,WR
//#endasm



    
    ei();

    WREN = false;   /*Write Disable */

    while(WR)
    {
    }

    return true;

    _delay(2000);
    if (WR)
    {
        cntOverTime++;
        if (cntOverTime > 2)
        {
            cntOverTime = 0;
            return 0; /* Write Failed */
        }
        else
        {
            return 2; /* writing */
        }
    }
    else
    {
        EEIF = false;
        cntOverTime = 0;
        return true; /*Finish */
    }


}

uint16_t EEPROMRead(uint8_t Address)
{
    //EEADRH = EEPROM_HIADDRESS;
    //EEADRL = EEPROM_LOADDRESS;

    //EEADRL = Address; /* Low byte of address */
    EEADR = Address;
    CFGS = 0;                  /* Deselect Config state */
    EEPGD = 0;                 /* Pointer to DATA memory*/
    RD = 1;                    /* Read Request */
    _delay(20);
    //return ((uint16_t)(EEDATL))|((uint16_t)(EEDATH<<8));
    //return EEDATL;
    return EEDATA;
}

