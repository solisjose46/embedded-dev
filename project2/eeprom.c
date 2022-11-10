/*
* eeprom.c
*
* SER486 Project 2
* Fall 2022
* Author: Jose Solis Salazar
*
*
*/

#include "eeprom.h"
#include "uart.h"

#define WRITE_BUF_SIZE (64)

static unsigned char writebuf[WRITE_BUF_SIZE];
static unsigned char bufidx;
static unsigned char writesize;
static unsigned int writeaddr;
static volatile unsigned char write_busy;

#define EECR (*((volatile unsigned char*) 0x3F ))
#define EEDR (*((volatile unsigned char*) 0x40 ))
#define EEARL (*((volatile unsigned char*) 0x41 ))
#define EEARH (*((volatile unsigned char*) 0x42 ))
#define SREG (*((volatile unsigned char*) 0x5F ))


/*
* writebuf (unsigned int addr, unsigned char* buf, unsigned char size)
*
*/
void eeprom_writebuf(unsigned int addr, unsigned char* buf, unsigned char size) {
   while (eeprom_isbusy());

   writeaddr = addr;
   write_busy = 1;
   bufidx = 0;

   for (int i = 0; i < size-1; i++) {

       writebuf[i] = buf[i];

   }

   writesize = size;

   SREG |= 0x80;

   EECR = 0x8;

}

/*
* readbuf(unsigned int addr, unsigned char* buf, unsigned char size)
*
*/
void eeprom_readbuf(unsigned int addr, unsigned char* buf, unsigned char size) {

   while (eeprom_isbusy());

   for (int i = 0; i < size; i++){

       while ((EECR & 0x10) == 1);

       EEARH = ((addr + i) >> 4);
       EEARL = ((addr + i) & 0x0F);

       EECR |= 0x1;

       buf[i] = EEDR;
   }

}

/*
* int eeprom_isbusy()
*
*/
int eeprom_isbusy(){
   return (write_busy == 1);

}

#pragma GCC push_options
#pragma GCC optimize ("Os")
/**********************************
* vector_22()
*/
void __vector_22(void) __attribute__ ((signal,used, externally_visible));
void __vector_22(void) {

   if (bufidx < writesize) {

       while((EECR & 0x10) == 1);

       EEARH = ((writeaddr) >> 4);
       EEARL = ((writeaddr) & 0x0F);

       EEDR = writebuf[bufidx];

       unsigned char sreg_state = SREG;

       __builtin_avr_cli();

       EECR |= 0b100;

       EECR |= 0b10;

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
