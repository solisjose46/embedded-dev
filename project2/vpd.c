/*
* vpd.c
*
* SER486 Project 2
* Fall 2022
* Author: Jose Solis Salazar
*
* Function definitions for writing VPD to the EEPROM are defined here.
*
*/

#include "eeprom.h"
#include "util.h"
#include "vpd.h"

vpd_struct vpd;

/* Default VPD values */
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

/*
* static int is_data_valid()
*
* Private function for data validation.
*
* arguments:
*   None
*
* returns:
*   1 if data valid else returns 0
*/

static int is_data_valid() {
    int result = 1;

    /* If vpd.token != "SER" or checksum not valid return 0 */
    if(vpd.token != defaults.token ){
           result = 0;
    }
    else if (is_checksum_valid((unsigned char*)&vpd, sizeof(vpd_struct))) {

       result = 0;
    }

    return result;

}

/*
* static void write_defaults()
*
* Private function for writing VPD defaults to the EEPROM.
*
* arguments:
*   None
*
* returns:
*   Nothing
*/
static void write_defaults() {
    update_checksum((unsigned char*) &defaults, sizeof(vpd_struct));
    eeprom_writebuf(0x00, (unsigned char*)&defaults, sizeof(vpd_struct));

}

/*
* void vpd_init()
*
* Initialize VPD member data from the EEPROM.
*
* arguments:
*   None
*
* returns:
*   Nothing
*/
void vpd_init() {
    while (eeprom_isbusy());
    eeprom_readbuf(0x0, (unsigned char*)&vpd, sizeof(vpd_struct));

    if(!is_data_valid()){
       write_defaults();
       eeprom_readbuf(0x0, (unsigned char*)&vpd, sizeof(vpd_struct));

    }
}
