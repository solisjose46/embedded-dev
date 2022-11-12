/*
* main.c
*
* SER486 Project 2
* Fall 2022
* Author: Jose Solis Salazar
*
* Program for reading and writing to the EEPROM in a non blocking manner (interrupts).
*/

#include "vpd.h"
#include "uart.h"
#include "eeprom.h"
#include "config.h"
#include "led.h"
#include "log.h"
#include "rtc.h"

/*
* int main(void)
*
* Initializes hardware, blinks pattern via led and prints EEPROM contents to the console.
*
* arguments:
*   None
*
* returns:
*   Nothing
*
*/

int main(void) {
    /* Hardware init */
    uart_init();
    config_init();
    led_init();
    log_init();
    rtc_init();
    vpd_init();
    /* Set pattern to blink and date string */
    led_set_blink("--- -.-");
    rtc_set_by_datestr("01/01/2019 00:00:00");
    /*  */
    uart_writestr("SER 486 Project 2 - Jose Solis Salazar\n\r");

    /* Print default VPD values */
    uart_writestr(vpd.model);
    uart_writestr("\r\n");
    uart_writestr(vpd.manufacturer);
    uart_writestr("\r\n");
    uart_writestr(vpd.token);
    uart_writestr("\r\n");

    /* Set config static ip and modified state */
    config.use_static_ip = 1;
    config_set_modified();

    /* Clear log and add logs */
    log_clear();
    log_add_record(0xaa);
    log_add_record(0xbb);
    log_add_record(0xcc);

    unsigned char dumped = 0;

    while(1){
        /*update led, log and config */
        led_update();
        log_update();
        config_update();

        /* Print EEPROM contents */
        if ((!eeprom_isbusy()) && (!dumped)) {
          dump_eeprom(0,0x100);
          dumped=1;
        }
    }

}
