/*
* util.c
*
* SER486 Project 2
* Fall 2022
* Author: Jose Solis Salazar
*
* This file provides function definitions checksum operations.
*/

/*
* update_checksum(unsigned char* data, unsigned int size)
*
* update the checksum (last byte) of the specifed data so that the sum of all the bytes will be zero.
*
* arguments:
*   unsigned char* data, unsigned int size
*
* returns:
*   Nothing
*
*/
void update_checksum(unsigned char* data, unsigned int size) {
    int sum = 0;
    for (int i = 0; i < (size - 2); i++){
        sum += data[i];
    }
    data[size - 1] = (sum * -1);

}

/*
*
* is_checksum_valid(unsigned char* data, unsigned int size)
*
* returns 1 if the sum of all the bytes in the data structure is 0.
* Otherwise, returns 0.
*
* arguments:
*   unsigned char* data, unsigned int size
*
* returns:
*   1 if bytes sum to 0 else 0
*
* changes:
*   EEPROM
*
*/
int is_checksum_valid(unsigned char* data, unsigned int size) {
    int sum = 0;
    for (int i = 0; i < size; i++){
        sum += data[i];
    }
    return (sum == 0);

}
