/*
 * timer1.c
 *
 * SER486 Assignment 4
 * Fall 2022
 * Author: Jose Solis Salazar
 * Modified By:
 *
 * Uses 16bit timer1 of the Atmega 328p to create a custom timer for timer interrupt design.
 *
 */

/* Register Macros */
#define SREG (*((volatile unsigned char*) 0x5F))
#define TCCR1A (*((volatile unsigned char*) 0x80 ))
#define TCCR1B (*((volatile unsigned char*) 0x81 ))
#define TIMSK1 (*((volatile unsigned char*) 0x6F ))
#define OCR1AL (*((volatile unsigned char*) 0x88 ))
#define OCR1AH (*((volatile unsigned char*) 0x89 ))

/* Timer Configuration Macros */
#define COM (0b0000 << 4)
#define WGMA (0b00)
#define INPUT_CAPTURE (0b00 << 6)
#define WGMB (0b01 << 3)
#define CS (0b101)
#define ENABLE_TIMER_INTERRUPT (0b010)

#define TOPH (0x3D)
#define TOPL (0x08)

/* Internal State */
static unsigned long count;

/*
 * timer1_init()
 *
 * Initialize timer1 mode, prescaler and enable timer interrupt.
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
void timer1_init() {
   TCCR1A = (COM | WGMA);
   TCCR1B = (INPUT_CAPTURE | WGMB | CS);
   TIMSK1 = ENABLE_TIMER_INTERRUPT;

   OCR1AH = TOPH;
   OCR1AL = TOPL;

   count = 0;
}

/*
 * timer1_get()
 *
 * Returns ticks elapsed
 *
 * arguments:
 *     None
 *
 * returns:
 *     Returns ticks elapsed
 *
 * Changes:
 *     Nothing
 *
 */
unsigned long timer1_get() {

    /* get global interrupt enable bit state */
    unsigned char sreg_state = SREG;

    /* disable interrupts */
    __builtin_avr_cli();

    /* get the count value */
    unsigned long value = count;

    /* restore the global interrupt state */
    SREG = sreg_state;

    /* return the count value */
    return value;

}

/*
 * timer1_clear()
 *
 * Resets ticks for count
 *
 * arguments:
 *     None
 *
 * returns:
 *     Nothing
 *
 * Changes:
 *     count
 *
 */
void timer1_clear() {

    /* get global interrupt enable state */
    unsigned char sreg_state = SREG;

    /* disable interrupts */
    __builtin_avr_cli();

    /* count = 0 */
    count = 0;

    /* restore global interrupt state */
    SREG = sreg_state;

}

/*
 * __vector_11
 *
 * Our interrupt handler which increments the timer.
 *
 * returns:
 *     Nothing
 *
 * Changes:
 *     count
 *
 */
void __vector_11(void) __attribute__ ((signal,used, externally_visible));
void __vector_11(void) {
   count++;
}
