/*
 * main.c
 *
 * SER486 Assignment 4
 * Fall 2022
 * Author: Jose Solis Salazar
 * Modified By:
 *
 * This is the main file for project 1.
 * Initializes necessary components and blinks "ok" and prints date string to console.
 */

#include "delay.h"
#include "led.h"
#include "rtc.h"
#include "timer1.h"
#include "uart.h"

int main(void){

    /* Hardware init */
    uart_init();
    led_init();
    rtc_init();

    led_set_blink("--- -.-");
    rtc_set_by_datestr("01/01/2019 00:00:00");

    uart_writestr("SER486 Project 1 - Jose Solis Salazar\n\r");

     /* get baseline performance */
    signed long c1=0;
    delay_set(1,100); while (!delay_isdone(1)) { c1++; }

    /* measure performance with led_update */
    signed long c2=0;
    delay_set(1,100); while (!delay_isdone(1)) { led_update(); c2++; }

    /* display the results */
    uart_writedec32(c1); uart_writestr(" ");
    uart_writedec32(c2); uart_writestr("\r\n");

    while(1){

        led_update();

        delay_set(1, 500);
        while(!delay_isdone(1)){}
        uart_writestr(rtc_get_date_string());
        uart_writestr("\r");
    }
}
