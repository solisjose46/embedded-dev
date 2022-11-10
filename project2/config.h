#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

/*
* config.h
*
* SER486 Project 2
* Fall 2022
* Author: Jose Solis Salazar
*
*
*/


#define TOKEN_SIZE (4)
#define IP_SIZE (4)

struct config_struct {

   char token[TOKEN_SIZE];
   unsigned int hi_alarm;
   unsigned int hi_warn;
   unsigned int lo_alarm;
   unsigned int lo_warn;
   char use_static_ip;
   unsigned char static_ip[IP_SIZE];
   unsigned char checksum;

};

extern struct config_struct config;

void config_init();

void config_update();

void config_set_modified();

#endif // CONFIG_H_INCLUDED
