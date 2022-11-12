/*
* config.c
*
* SER486 Project 2
* Fall 2022
* Author: Jose Solis Salazar
*
* Functions and internal state for Configuration class implementation.
*/

#include "eeprom.h"
#include "util.h"
#include "config.h"

#define CONFIG_ADDR 0x0040

config_struct config;
static unsigned char modified;
static config_struct defaults = {
   "ASU",
   0x3FF,
   0x3FE,
   0x0000,
   0x0001,
   0,
   {
       192, 168, 1, 100
   },
   0

};

/*
* static int is_data_valid()
*
*   Config data validation.
*
* arguments:
*   None
*
* returns:
*   1 if valid else 0
*
*/
static int is_data_valid() {
    int result = 1;

    /* If config token != "ASU" or checksum not valid return 0 */
    if (config.token != defaults.token) {
        result = 0;
    }
    else if(!(is_checksum_valid((unsigned char*)&config, sizeof(config_struct)))){
        result = 0;
    }

    return result;

}

/*
* static void write_defaults()
*
* Configure factory defaults to EEPROM.
*
* arguments:
*   None
*
* returns:
*   Nothing
*
*/
static void write_defaults() {
    update_checksum((unsigned char*) &defaults, sizeof(config_struct));
    eeprom_writebuf(CONFIG_ADDR, (unsigned char*)&defaults, sizeof(config_struct));
}

/*
* void config_init()
*
* Initializes Configuration data from EEPROM if valid else writes factory defaults.
*
* arguments:
*   None
*
* returns:
*   Nothing
*
*/
void config_init() {
    while (eeprom_isbusy());

    /* Read data, if not valid then write factory defaults */
    eeprom_readbuf(CONFIG_ADDR, (unsigned char*)&config, sizeof(config_struct));
    if (!is_data_valid()) {
        write_defaults();
        eeprom_readbuf(CONFIG_ADDR, (unsigned char*)&config, sizeof(config_struct));
        modified = 0;
    }

}

/*
* void config_update()
*
* Update Configuration data and write it to the write buffer.
*
* arguments:
*   None
*
* returns:
*   Nothing
*
*/
void config_update() {
    /* Write to the buffer if not busy and if modified flag is set */
    if (eeprom_isbusy() || !modified ) return;
    update_checksum((unsigned char*)&config, sizeof(config_struct));
    eeprom_writebuf(CONFIG_ADDR, (unsigned char*)&config, sizeof(config_struct));
    /* Reset the flag */
    modified = 0;

}

/*
* config_set_modified()
*
* Set the modified flag.
*
* arguments:
*   None
*
* returns:
*   Nothing
*
*/
void config_set_modified() {
    /* Set the flag */
    modified = 1;
}
