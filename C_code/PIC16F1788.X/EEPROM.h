/* 
 * File:   EEPRAM.h
 * Author: PCMART
 *
 * Created on October 14, 2014, 8:20 AM
 */

#ifndef EEPRAM_H
#define	EEPRAM_H
uint16_t EEPROMRead(uint8_t Address);
char EEPROMWrite(uint16_t Data, uint8_t Address);


#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

#endif	/* EEPRAM_H */

