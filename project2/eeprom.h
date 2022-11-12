#ifndef EEPROM_H_INCLUDED
#define EEPROM_H_INCLUDED

/*
* eeprom.h
*
* SER486 Project 2
* Fall 2022
* Author: Jose Solis Salazar
*
* Function declarations for EEPROM read/write operations.
*
*/

/* Places data into into the buffer for writing to the EEPROM at some later time. */
void eeprom_writebuf(unsigned int addr, unsigned char* buf, unsigned char size);

/* Read specific amount of data from the EEPROM starting at a specific address and place it in a specified buffer. */
void eeprom_readbuf(unsigned int addr, unsigned char* buf, unsigned char size);

/* Checks internal state write_busy. */
int eeprom_isbusy();

#endif // EEPROM_H_INCLUDED
