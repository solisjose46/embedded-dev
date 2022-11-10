/*
* config.c
*
* SER486 Project 2
* Fall 2022
* Author: Jose Solis Salazar
*
*
*/

#include "eeprom.h"
#include "config.h"
#include "util.h"

#define CONFIG_ADDR (0x0040)

static unsigned char modified;

// TODO: add defaults
static struct config_struct defaults = {};


struct config_struct config;

static void config_write_defaults();

/*
* int config_is_data_valid()
*/
static int config_is_data_valid() {

   int result = 1;

   if (config.token != defaults.token || !(is_checksum_valid((unsigned char*)&config, sizeof(struct config_struct)))) {

       result = 0;

   }

   return result;

}

/*
* config_init()
*
*/
void config_init() {

   while (eeprom_isbusy());

   eeprom_readbuf(CONFIG_ADDR, (unsigned char*)&config, sizeof(struct config_struct));

   if (!config_is_data_valid()) {

       config_write_defaults();

       eeprom_readbuf(CONFIG_ADDR, (unsigned char*)&config, sizeof(struct config_struct));

       modified = 0;

   }

}

/*
* config_update()
*
*/
void config_update() {

   if (eeprom_isbusy() || !modified ) {

       return;

   }

   update_checksum((unsigned char*)&config, sizeof(struct config_struct));


   eeprom_writebuf(CONFIG_ADDR, (unsigned char*)&config, sizeof(struct config_struct));

   modified = 0;

}

/*
* config_set_modified()
*/
void config_set_modified() {

   modified = 1;

}

/*
* config_write_defaults()
*/
static void config_write_defaults() {
   update_checksum((unsigned char*) &defaults, sizeof(struct config_struct));
   eeprom_writebuf(CONFIG_ADDR, (unsigned char*)&defaults, sizeof(struct config_struct));

}
