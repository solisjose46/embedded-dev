/*
* util.c
*
* SER486 Project 3
* Fall 2022
* Author: Jose Solis Salazar
*
* Implementation for updating the alarm and warning ranges of  temperature sensor.
*
*/

#include "util.h"
#include "config.h"

config_struct config;

/*
* int update_tcrit_hi(int value)
*
* Updates tcrit_hi with specified value.
*
* arguments:
*   int value
*
* returns:
*   1 if error updating 0 if success
*/

int update_tcrit_hi(int value) {
    if(value <= config.hi_warn || value > 0x3FF){
        return 1;
    }
    config.hi_alarm = value;
    config_set_modified();
    config_update();

    return 0;

}

/*
* int update_twarn_hi(int value)
*
* Updates twarn_hi with specified value.
*
* arguments:
*   int value
*
* returns:
*   1 if error updating 0 if success
*/
int update_twarn_hi(int value) {
    if(value < config.lo_warn || value > config.hi_alarm){
        return 1;
    }

    config.hi_warn = value;
    config_set_modified();
    config_update();
    return 0;

}

/*
* int update_tcrit_lo(int value)
*
* Updates tcrit_lo with specified value.
*
* arguments:
*   int value
*
* returns:
*   1 if error updating 0 if success
*/
int update_tcrit_lo(int value) {
    if(value >= config.lo_alarm){
        return 1;
    }

    config.lo_alarm = value;
    config_set_modified();
    return 0;
}

/*
* int update_twarn_lo(int value)
*
* Updates twarn_lo with specified value.
*
* arguments:
*   int value
*
* returns:
*   1 if error updating 0 if success
*/

int update_twarn_lo(int value) {
    if(value <= config.lo_alarm || value >= config.hi_warn){
        return 1;
    }

    config.lo_warn = value;
    config_set_modified();
    config_update();
    return 0;
}
