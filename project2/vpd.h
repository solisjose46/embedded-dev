#ifndef VPD_H_INCLUDED
#define VPD_H_INCLUDED

/*
* vpd.h
*
* SER486 Project 2
* Fall 2022
* Author: Jose Solis Salazar
*
* Function declarations for writing VPD to the EEPROM are declared in this file.
*
*/

/* Array sizes as variables */
#define TOKEN_SIZE 4
#define MODEL_SIZE 12
#define MANUFACTURER_SIZE 12
#define SERIAL_SIZE 12
#define MAC_ADDR_SIZE 6
#define COUNTRY_ORIGIN_SIZE 4

typedef struct{
   char token[TOKEN_SIZE];
   char model[MODEL_SIZE];
   char manufacturer[MANUFACTURER_SIZE];
   char serial_number[SERIAL_SIZE];
   unsigned long manufacture_date;
   unsigned char mac_address[MAC_ADDR_SIZE];
   char country_of_origin[COUNTRY_ORIGIN_SIZE];
   unsigned char checksum;
}vpd_struct;

extern vpd_struct vpd;

/* Initialize vpd member data from the EEPROM. */
void vpd_init();

#endif // VPD_H_INCLUDED
