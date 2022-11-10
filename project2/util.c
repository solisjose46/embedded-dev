/*
* util.c
*
* SER486 Project 2
* Fall 2022
* Author: Jose Solis Salazar
*
*
*/

/*
* update_checksum(char* data, unsigned int size)
*
*
*/
void update_checksum(unsigned char* data, unsigned int size) {

   int sum = 0;

   for (int i = 0; i < (size - 2); i++){
       sum += data[i];
   }

   data[size - 1] = (sum * -1);

}

/**********************************
* int is_checksum_valid(char* data, unsigned int size)
*
*/
int is_checksum_valid(unsigned char* data, unsigned int size) {

   int sum = 0;

   for (int i = 0; i < size; i++){

       sum += data[i];

   }

   return (sum == 0);

}
