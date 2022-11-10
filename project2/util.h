#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

/*
* util.h
*
* SER486 Project 2
* Fall 2022
* Author: Jose Solis Salazar
*
* Function declarations for checksum operations.
*
*/

void update_checksum(unsigned char* data, unsigned int size);

int is_checksum_valid(unsigned char* data, unsigned int size);

#endif // UTIL_H_INCLUDED
