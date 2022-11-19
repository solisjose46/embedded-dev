/*
* wdt.c
*
* SER486 Project 3
* Fall 2022
* Author: Jose Solis Salazar
*
* ATMEGA 328P watchdog timer class implementation.
*
*/

#include "wdt.h"
#include "led.h"
#include "log.h"
#include "config.h"

#define WDTCSR (*((volatile unsigned char*) 0x60))
#define SREG (*((volatile unsigned char*) 0x5F ))

#define WDIE 6
#define WDCE 4
#define WDE 3
#define WDP2 2
#define WDP1 1
#define WDP0 0

#pragma GCC push_options
#pragma GCC optimize ("Os")

/*
* void wdt_init()
*
* Initalize watchdog timer for 2 sec timeout.
*
* arguments:
*   None
*
* returns:
*   Nothing
*/
void wdt_init() {
    __builtin_avr_cli();
    WDTCSR = (1 << WDCE) | (1 << WDE);
    WDTCSR = 1 << WDIE | 1 << WDE | 1 << WDP2 | 1 << WDP1 | 1 << WDP0;
    __builtin_avr_sei();
}

/*
* void wdt_reset()
*
* Reset watchdog timeout to prevent a timeout.
*
* arguments:
*   None
*
* returns:
*   Nothing
*/
void wdt_reset() {
   __builtin_avr_wdr();

}

/*
* void wdt_force_restart()
*
* Disbales watchdog timer and waits for timeout.
*
* arguments:
*   None
*
* returns:
*   Nothing
*/
void wdt_force_restart() {

    WDTCSR &= (0 << WDIE);
    while(1);

}
#pragma GCC pop_options

/*
* void __vector_6()
*
* Turns on the LED and adds an EVENT_WDT to system event log.
*
* arguments:
*   None
*
* returns:
*   Nothing
*/
void __vector_6(void) __attribute__ ((signal,used, externally_visible));
void __vector_6() {
    led_on();
    log_add_record(EVENT_WDT);
    for(int i = 0; i < 16; i++){

       log_update_noisr();

    }
    config_update_noisr();
    while(1);
}
