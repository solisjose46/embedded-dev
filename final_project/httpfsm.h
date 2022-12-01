/*
* httpfsm.h
*
* SER486 Final Project
* Fall 2022
* Author: Jose Solis Salazar
*
* Public methods for httpfsm.
*
*/


#ifndef HTTPFSM_H_INCLUDED
#define HTTPFSM_H_INCLUDED

/* Method for updating HTTP FSM */
void httpfsm_update();

/* Initializes state to INITIAL_STATE */
void httpfsm_init();

/* Resets HTTP FSM state to FLUSH */
void httpfsm_reset();


#endif // HTTPFSM_H_INCLUDED
