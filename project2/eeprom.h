#ifndef EEPROM_H_INCLUDED
#define EEPROM_H_INCLUDED

/*
* eeprom.h
*
* SER486 Project 2
* Fall 2022
* Author: Jose Solis Salazar
*
* Function declarations for EEPROM read/wrie operations.
*
*/


void eeprom_writebuf(unsigned int addr, unsigned char* buf, unsigned char size);


void eeprom_readbuf(unsigned int addr, unsigned char* buf, unsigned char size);


int eeprom_isbusy();

#endif // EEPROM_H_INCLUDED
