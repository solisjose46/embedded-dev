/*
 * main.c
 *
 * SER486 Assignment 4
 * Fall 2022
 * Author: Jose Solis Salazar
 * Modified By:
 *
 * This is the main file for hw4 program.
 * Initializes necessary components and reads and outputs the temperature
 */


#include "uart.h"
#include "simpleled.h"
#include "simpledelay.h"
#include "temp.h"

int main(void)
{
    /*Initialize uart, led then temperature*/
    uart_init();
    led_init();
    temp_init();

    /*Write author's name to console*/
    uart_writestr("SER486 HW4 Jose Solis Salazar\n\r");

    /*Loop forever*/
    while(1){
        /*Begin the temp conversion*/
        temp_start();

        /*Wait until data is ready to read*/
        while(!temp_is_data_ready()){
            /*Wait*/
        }

        /*Get reading and print to console*/
        signed int temperature = temp_get();
        uart_writedec32(temperature);
        /*start on new line to make it easier to read on console*/
        uart_writestr("\n\r");
        /*Wait 1000 ms = 1 second*/
        delay(1000);
    }

    return 0;
}
