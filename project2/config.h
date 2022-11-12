#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

/*
* config.h
*
* SER486 Project 2
* Fall 2022
* Author: Jose Solis Salazar
*
* Function declarations for Config class implementation.
*/


#define TOKEN_SIZE 4
#define IP_SIZE 4

typedef struct{
   char token[TOKEN_SIZE];
   unsigned int hi_alarm;
   unsigned int hi_warn;
   unsigned int lo_alarm;
   unsigned int lo_warn;
   char use_static_ip;
   unsigned char static_ip[IP_SIZE];
   unsigned char checksum;

}config_struct;

extern config_struct config;

/* Intitailize config member data from the EEPROM. If data is invalid then default values are used. */
void config_init();

/* Write modified config data to EEPROM when not busy. */
void config_update();

/* Sets the modified flag. */
void config_set_modified();

#endif // CONFIG_H_INCLUDED
