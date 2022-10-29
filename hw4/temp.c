/*
 * temp.c
 *
 * SER486 Assignment 4
 * Fall 2022
 * Author: Jose Solis Salazar
 * Modified By:
 *
 * This file contains public function definitions for temperature operations on the Atmega 328p.
 *
 */

#define ADMUX (* ((volatile char*) 0x7C))
#define ADCSRA ( *((volatile char*) 0x7A))
#define ADCL (* ((volatile char*) 0x78))
#define ADCH (* ((volatile char*) 0x79))
#define VOLTAGE_REF ((unsigned char) 0b11)
#define INPUT_SELECTOR ((unsigned char) 0b1000)
#define DIVISOR_SELECTOR ((unsigned char) 0b110)
#define CONVERSION_SELECTOR ((unsigned char) 0b1)

/*
 * temp_init()
 *
 * Initialize ADC to perform temperature operations
 *
 * arguments:
 *     None
 *
 * returns:
 *     Nothing
 *
 * Changes:
 *     ADMUX and ADCSRA register values
 *
 */
void temp_init()
{
    unsigned char ad_mux_value = ((VOLTAGE_REF << 6) | (INPUT_SELECTOR));
    unsigned char adcsra_value = ((CONVERSION_SELECTOR << 7) | (DIVISOR_SELECTOR));

    ADMUX |= ad_mux_value;
    ADCSRA |= adcsra_value;
}

/*
 * temp_is_data_ready()
 *
 * Check if temperature conversion is complete
 *
 * arguments:
 *     None
 *
 * returns:
 *     1 (true) complete, 0 (false) not complete
 *
 * Changes:
 *     No Changes
 *
 */
int temp_is_data_ready()
{
    unsigned char result = (((ADCSRA) & 0x40) >>6);

    if (result == 0) {
        return 1;
    }

    return 0;
}

/*
 * temp_start()
 *
 * Starts the temperature conversion.
 *
 * arguments:
 *     None
 *
 * returns:
 *     Nothing
 *
 * Changes:
 *     ADCSRA register value
 *
 */
void temp_start()
{
    ADCSRA |= 0x40;
}

/*
 * temp_get()
 *
 * Converts most recent temperature reading into a meaningful result and returns it.
 *
 * arguments:
 *     None
 *
 * returns:
 *     (int)temperature
 *
 * Changes:
 *     No Changes
 *
 */
signed int temp_get()
{
    signed long reading = (ADCL);
    reading |= ((ADCH) << 8);

    signed long degrees = ((reading * 101) / 100) - 273;

    return degrees;
}
