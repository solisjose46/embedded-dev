/*
* vpd.c
*
* SER486 Project 2
* Fall 2022
* Author: Jose Solis Salazar
*
* 
*
*/

#include "eeprom.h"
#include "util.h"
#include "vpd.h"

static vpd_struct defaults = {
   "SER",
   "Jose",
   "Solis",
   "Arizona",
   0,
   {'j' & 0xFE, 'o', 's', 's', 'o', 'l'},
   "USA",
   0
};

vpd_struct vpd;

/*
 * TODO: Review MEMBER FUNCTION NAMES
* int vpd_is_data_valid()
*
*
*/
static int vpd_is_data_valid() {

    int result = 1;

    if(vpd.token != defaults.token ){
           result = 0;
    }
    else if (is_checksum_valid((unsigned char*)&vpd, sizeof(struct vpd_struct))) {

       result = 0;
    }

    return result;

}

/*
* write_defaults()

*
*/
static void write_defaults() {
   update_checksum((unsigned char*) &defaults, sizeof(struct vpd_struct));
   eeprom_writebuf(0x00, (unsigned char*)&defaults, sizeof(struct vpd_struct));

}

/*
* vpd_init()
*
*/
void vpd_init() {
    while (eeprom_isbusy());
    eeprom_readbuf(0x0, (unsigned char*)&vpd, sizeof(struct vpd_struct));

    if(!vpd_is_data_valid()){
       write_defaults();
       eeprom_readbuf(0x0, (unsigned char*)&vpd, sizeof(struct vpd_struct));

    }
}
