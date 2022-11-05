/*
 * delay.c
 *
 * SER486 Assignment 4
 * Fall 2022
 * Author: Jose Solis Salazar
 * Modified By:
 *
 * Uses 8 bit timer0 of the Atmega 328p to create custom timers for timer interrupt design.
 *
 */

/* Register Macros */
#define SREG (*((volatile char*) 0x5F ))
#define TCCR0A (*((volatile unsigned char*) 0x44 ))
#define TCCR0B (*((volatile unsigned char*) 0x45 ))
#define TIMSK0 (*((volatile unsigned char*) 0x6E ))
#define OCR0A (*((volatile unsigned char*) 0x47 ))

/* Timer Configuration Macros */
#define COM (0b0000 << 4)
#define WGMA (0b10)
#define WGMB (0b0 << 3)
#define CS (0b011)
#define ENABLE_TIMER_INTERRUPT (0b010)

/* 0d249 limit */
#define TOP (0xF9)


/* Internal State */
static volatile unsigned int count[2];
static unsigned int limit[2];
static unsigned char initialized = 0;


/*
 * init()
 *
 * Initialize timer0 mode, prescaler and enable timer interrupt.
 *
 * arguments:
 *     None
 *
 * returns:
 *     Nothing
 *
 * Changes:
 *     TCCR0A, TCCR0B, TIMSK0, OCR0A
 *
 */
static void init() {
    TCCR0A = (COM | WGMA);
    TCCR0B = (WGMB | CS);
    TIMSK0 = ENABLE_TIMER_INTERRUPT;
    OCR0A = TOP;

    count[0] = 0;
    count[1] = 0;
    limit[0] = 0;
    limit[1] = 0;

    initialized = 1;
}

/*
 * delay_get(unsigned num)
 *
 * Returns ticks elapsed for count[num]
 *
 * arguments:
 *     num
 *
 * returns:
 *     Returns ticks elapsed for count[num]
 *
 * Changes:
 *     Nothing
 *
 */
unsigned delay_get(unsigned num) {

    /* get global interrupt enable bit state */
    unsigned char sreg_state = SREG;

    /* disable interrupts */
    __builtin_avr_cli();

    /* get the count[n] value */
    unsigned int value = count[num];

    /* restore global interrupt state */
    SREG = sreg_state;

    /* return the count value */
    return value;

}


/*
 * delay_set(unsigned int num, unsigned int time)
 *
 * Resets ticks for count[num] and tick limit
 *
 * arguments:
 *     num, time
 *
 * returns:
 *     Nothing
 *
 * Changes:
 *     count[num], limit[num]
 *
 */
void delay_set(unsigned int num, unsigned int time){

    if (initialized != 1) {
        init();
    }

    /* get global interrupt enable bit state */
    unsigned char sreg_state = SREG;

    /* disable interrupts */
    __builtin_avr_cli();

    /* set the limit for delay[n] and clear the count for delay[n] */
    limit[num] = time;
    count[num] = 0;

    /* restore global interrupt state */
    SREG = sreg_state;

}

/*
 * delay_set(unsigned int num)
 *
 * Checks if count[num] has reached its tick limit
 *
 * arguments:
 *     num
 *
 * returns:
 *     1 = if limit reached, 0 if not yet reached limit
 *
 * Changes:
 *     Nothing
 *
 */
unsigned char delay_isdone(unsigned int num) {

    unsigned char value = 0;

    if (count[num] == limit[num]) {
            value = 1;
    }

    return value;

}

/*
 * __vector_14
 *
 * Our interrupt handler which increments our timers.
 *
 *
 * returns:
 *     Nothing
 *
 * Changes:
 *     count[0], count[1]
 *
 */
void __vector_14(void) __attribute__ ((signal,used, externally_visible));
void __vector_14(void) {

    count[0]++;
    count[1]++;

 }
