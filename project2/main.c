/*
* main.c
*
* SER486 Project 2
* Fall 2022
* Author: Jose Solis Salazar
*
*
*/

#include "vpd.h"
#include "uart.h"
#include "eeprom.h"
#include "config.h"
#include "led.h"
#include "log.h"
#include "rtc.h"

void dump_eeprom(unsigned int addr, unsigned int numbytes);

int main(void) {

    uart_init();
    config_init();
    led_init();
    log_init();
    rtc_init();
    vpd_init();

    led_set_blink("--- -.-");

    rtc_set_by_datestr("01/01/2019 00:00:00");

    uart_writestr("SER 486 Project 2 - Jose Solis Salazar\n\r");

    uart_writestr(vpd.model);
    uart_writestr("\r\n");
    uart_writestr(vpd.manufacturer);
    uart_writestr("\r\n");
    uart_writestr(vpd.token);
    uart_writestr("\r\n");

    config.use_static_ip = 1;
    config_set_modified();

    log_clear();
    log_add_record(0xaa);
    log_add_record(0xbb);
    log_add_record(0xcc);

    unsigned char dumped = 0;

    while(1){
        led_update();
        log_update();
        config_update();

        if ((!eeprom_isbusy()) && (!dumped)) {
          dump_eeprom(0,0x100);
          dumped=1;
        }
    }

}
