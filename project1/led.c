/*
 * led.c
 *
 * SER486 Assignment 4
 * Fall 2022
 * Author: Jose Solis Salazar
 * Modified By:
 *
 * Provides non blocking led blinking functionality
 *
 */

#include "led.h"
#include "delay.h"

/* Internal State */
static char* blink_msg;
static unsigned int blink_pos;
static unsigned char blink_state;

/* States as enums */
enum state {PHASE1, PHASE2};

/* Delay Macros */
#define BLANK_SPACE 1000
#define DASH 750
#define DOT 250
#define OTHER 0
#define PHASE2_DELAY 100


/*
 * led_set_blink(char* msg)
 *
 * Sets the morse code message to be blinked and initialize state.
 *
 * arguments:
 *     message to blink in morse code.
 *
 * returns:
 *     Nothing
 *
 * Changes:
 *     blink_msg, blink_pos, blink_state
 *
 */
void led_set_blink(char* msg) {

    /* blink_msg = msg */
    blink_msg = msg;

    /* blink_pos = 0 */
    blink_pos = 0;

    /* blink_state = PHASE1 */
    blink_state = PHASE1;

    /* delay_set(0,0) */
    delay_set(0,0);

    /* led_off */
    led_off();

}/*
 * led_update()
 *
 * Updates the state of the led based on hardware timer.
 *
 * arguments:
 *     None
 *
 * returns:
 *     Nothing
 *
 * Changes:
 *     blink_state
 *
 */
void led_update() {

    if (*blink_msg == 0 || !delay_isdone(0)) {
        return;
    }

    switch (blink_state){

        case PHASE1: {
            switch (blink_msg[blink_pos]){

                case ' ': {
                    if ( blink_msg[blink_pos+1] == 0) {
                        blink_pos++;
                    }
                    else {
                        blink_pos = 0;
                    }
                    led_off();
                    delay_set(0, BLANK_SPACE);
                }
                break;

                case '-': {
                    led_on();
                    delay_set(0, DASH);
                    blink_state = PHASE2;
                }
                break;

                case '.': {
                    led_on();
                    delay_set(0, DOT);
                    blink_state = PHASE2;
                }
                break;

                default: {
                    led_off();
                    delay_set(0,OTHER);
                }
            }
        }
        case PHASE2: {

            if (blink_msg[blink_pos+1] !=0){
                blink_pos++;
            }
            else {
                blink_pos = 0;
            }
            led_off();
            delay_set(0,PHASE2_DELAY);
            blink_state = PHASE1;
        }
    }
}
