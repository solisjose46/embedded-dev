/*
* tempfsm.c
*
* SER486 Project 3
* Fall 2022
* Author: Jose Solis Salazar
*
* Hystersis implementation via fsm to read ATMEGA 328P temperature readings.
*
*/

#include "tempfsm.h"
#include "log.h"
#include "alarm.h"
#include "led.h"

/* FSM State as enum */
enum state {
    NORM_1, 
    NORM_2, 
    NORM_3, 
    WARNING_HI_1,
    WARNING_HI_2, 
    CRITICAL_HI,
    WARNING_LO_1,
    WARNING_LO_2,
    CRITICAL_LO
};

static enum state fsm_state;

/* LED Alert Macros */
#define CRITICAL_LED (".")
#define WARNING_LED ("-")
#define NORMAL_LED (" ")

/*
* void tempfsm_update(int current, int hicrit, int hiwarn, int locrit, int lowarn)
*
* Updates fsm state based on provided temperature arguments.
* Sets LED blink behavior and sends alarms according to fsm state.
*
* arguments:
*   int current, int hicrit, int hiwarn, int locrit, int lowarn
*
* returns:
*   nothing
*/
void tempfsm_update(int current, int hicrit, int hiwarn, int locrit, int lowarn){
    switch(fsm_state){
        case NORM_1: {
           if(current >= hiwarn){
               fsm_state = WARNING_HI_1;
               alarm_send(EVENT_HI_WARN);
               log_add_record(EVENT_HI_WARN);
               led_set_blink(WARNING_LED);

           }
           else if(current <= lowarn){
               fsm_state = WARNING_LO_1;
               alarm_send(EVENT_LO_WARN);
               log_add_record(EVENT_LO_WARN);
               led_set_blink(WARNING_LED);
           }

        }
        break;

        case NORM_2: {
           if(current >= hiwarn){
               fsm_state = WARNING_HI_1;
               alarm_send(EVENT_HI_WARN);
               log_add_record(EVENT_HI_WARN);
               led_set_blink(WARNING_LED);

           }
           else if(current <= lowarn){
               fsm_state = WARNING_LO_1;
               led_set_blink(WARNING_LED);
           }
           else {
               led_set_blink(NORMAL_LED);
           }

        }
        break;

        case NORM_3: {
           if(current <= lowarn){
               fsm_state = WARNING_LO_1;
               alarm_send(EVENT_LO_WARN);
               log_add_record(EVENT_LO_WARN);
               led_set_blink(WARNING_LED);
           }
           else if(current >= hiwarn){
               fsm_state = WARNING_HI_1;
               led_set_blink(WARNING_LED);
           }
           else {
               led_set_blink(NORMAL_LED);
           }

        }
        break;

        case WARNING_HI_1: {
           if(current >= hicrit) {
               fsm_state = CRITICAL_HI;
               alarm_send(EVENT_HI_ALARM);
               log_add_record(EVENT_HI_ALARM);
               led_set_blink(CRITICAL_LED);

           }
           else if(current < hiwarn) {
               fsm_state = NORM_3;
               led_set_blink(NORMAL_LED);
           }
           else {
               led_set_blink(NORMAL_LED);
           }

        }
        break;

        case WARNING_HI_2: {
           if(current >= hicrit) {
               fsm_state = CRITICAL_HI;
               led_set_blink(CRITICAL_LED);
           }
           else if(current < hiwarn) {
               fsm_state = NORM_3;
               led_set_blink(NORMAL_LED);
           }
           else {
               led_set_blink(NORMAL_LED);
           }

        }
        break;

        case CRITICAL_HI: {
            if(current < hicrit) {
               fsm_state = WARNING_HI_2;
               led_set_blink(WARNING_LED);
           }
           else {
               led_set_blink(CRITICAL_LED);
           }

        }
        break;

        case WARNING_LO_1: {
            if(current <= locrit) {
               fsm_state = CRITICAL_LO;
               alarm_send(EVENT_LO_ALARM);
               log_add_record(EVENT_LO_ALARM);
               led_set_blink(CRITICAL_LED);
           }
           else if(current > lowarn) {
               fsm_state = NORM_2;
               led_set_blink(NORMAL_LED);
           }
           else {
               led_set_blink(WARNING_LED);
           }

        }
        break;

        case WARNING_LO_2: {
           if(current < locrit) {
               fsm_state = CRITICAL_LO;
               led_set_blink(CRITICAL_LED);
           }
           else if(current > lowarn) {
               fsm_state = NORM_3;
               led_set_blink(NORMAL_LED);
           }
           else {
               led_set_blink(WARNING_LED);
           }

        }
        break;

        case CRITICAL_LO: {
           if(current > locrit) {
               fsm_state = WARNING_LO_2;
               led_set_blink(WARNING_LED);
           }
           else {
               led_set_blink(CRITICAL_LED);
           }
        }
        break;
    }
}

/*
* void tempfsm_reset()
*
* Sets fsm_state to norm_1.
*
* arguments:
*   none
*
* returns:
*   nothing
*/
void tempfsm_reset(){

    fsm_state = NORM_1;

}

/*
* void tempfsm_init()
*
* Initializes fsm_state to norm_1.
*
* arguments:
*   none
*
* returns:
*   nothing
*/
void tempfsm_init(){

    fsm_state = NORM_1;

}
