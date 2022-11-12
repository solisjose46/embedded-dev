/*
* eeprom.c
*
* SER486 Project 2
* Fall 2022
* Author: Jose Solis Salazar
*
* Read/Write EEPROM operations functions defined in this file.
*/

#include "eeprom.h"

#define BUF_SIZE 64

/* Internal state */
static unsigned char writebuf[BUF_SIZE];
static unsigned char bufidx;
static unsigned char writesize;
static unsigned int writeaddr;
static volatile unsigned char write_busy;

/* Register Macros */
#define EECR (*((volatile unsigned char*) 0x3F ))
#define EEDR (*((volatile unsigned char*) 0x40 ))
#define EEARL (*((volatile unsigned char*) 0x41 ))
#define EEARH (*((volatile unsigned char*) 0x42 ))
#define SREG (*((volatile unsigned char*) 0x5F ))

/*
* void eeprom_writebuf(unsigned int addr, unsigned char* buf, unsigned char size)
*
* Places data into into the buffer for writing to the EEPROM at some later time.
*
* arguments:
*   unsigned int addr, unsigned char* buf, unsigned char size
*
* returns:
*   Nothing
*
* changes:
*   writeaddr, write_busy, bufidx, writebuf, writesize, SREG, EECR
*
*/
void eeprom_writebuf(unsigned int addr, unsigned char* buf, unsigned char size) {

    /* wait for eeprom*/
    while (eeprom_isbusy());

    writeaddr = addr;
    write_busy = 1;
    bufidx = 0;

    /* copy data to buffer*/
    for (int i = 0; i < size-1; i++) {

        writebuf[i] = buf[i];

    }

    /* set data size and enable interrupts */
    writesize = size;
    SREG |= 0x80;
    EECR = 0x8;

}

/*
* void eeprom_readbuf(unsigned int addr, unsigned char* buf, unsigned char size)
*
* Read specific amount of data from the EEPROM starting at a specific address and place it in a specified buffer.
*
* arguments:
*   unsigned int addr, unsigned char* buf, unsigned char size
*
* returns:
*   Nothing
*
* changes:
*   EEARH, EEARL, EECR, EEDR
*
*/
void eeprom_readbuf(unsigned int addr, unsigned char* buf, unsigned char size) {
    /* wait for eeprom */
    while (eeprom_isbusy());

    for (int i = 0; i < size; i++){
        /* Block */
        while ((EECR & 0x10) == 1);

        /* set read address, enable read then read into buffer */
        EEARH = ((addr + i) >> 4);
        EEARL = ((addr + i) & 0x0F);
        EECR |= 0x1;
        buf[i] = EEDR;
    }

}

/*
* int eeprom_isbusy()
*
* Checks internal state write_busy.
*
* arguments:
*   None
*
* returns:
*   1 if busy writing else 0
*
* changes:
*   Nothing
*
*/
int eeprom_isbusy(){
    return write_busy;
}

#pragma GCC push_options
#pragma GCC optimize ("Os")
/*
* void __vector_22(void)
*
* ISR for writing to the EEPROM one byte at t time.
*
* arguments:
*   None
*
* returns:
*   Nothing
*
* changes:
*   EEPROM
*
*/
void __vector_22(void) __attribute__ ((signal,used, externally_visible));
void __vector_22(void) {

    if (bufidx < writesize) {
         /* block */
        while((EECR & 0x10) == 1);

        /* Set address and byte to be written */
        EEARH = ((writeaddr) >> 4);
        EEARL = ((writeaddr) & 0x0F);
        EEDR = writebuf[bufidx];

        /* Disable interrupts */
        unsigned char sreg_state = SREG;
        __builtin_avr_cli();

        /* Enable write then write */
        EECR |= 0b100;
        EECR |= 0b10;

        /* Enable interrupts and increment address for next byte */
        SREG = sreg_state;
        writeaddr++;
        bufidx++;

    }
    else {

        EECR &= (~(1 << 3));
        write_busy = 0;

    }

}

#pragma GCC pop_options
