/*
 * temp.h
 *
 * SER486 Assignment 4
 * Fall 2022
 * Author: Jose Solis Salazar
 * Modified By:
 *
 * This file contains public function declarations for temperature operations on the atmega328p.
 *
 */


#ifndef TEMP_H_INCLUDED
#define TEMP_H_INCLUDED

/*Initialize ADC to measure temp sensor.*/
void temp_init();

/*Checks status of ADC to determine if most recent conversion is ready.*/
int temp_is_data_ready();

/*Start conversion*/
void temp_start();

/*read most recent ADC value*/
signed int temp_get();

#endif // TEMP_H_INCLUDED
