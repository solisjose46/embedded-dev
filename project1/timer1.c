/* Register Macros */
#define SREG (*(volatile unsigned char*) 0x5F)
#define TCCR1A (*(volatile unsigned char*) 0x80 )
#define TCCR1B (*(volatile unsigned char*) 0x81 )
#define TIMSK1 (*(volatile unsigned char*) 0x6F )
#define OCR1AL (*(volatile unsigned char*) 0x88 )
#define OCR1AH (*(volatile unsigned char*) 0x89 )

/* Timer Configuration Macros */
#define COM (0x0 << 4)
#define WGMA (0x0 << 1 | 0x0)
#define INPUT_CAPTURE (0x0 << 6)
#define WGMB (0x1 << 3)
#define CS (0x1 << 2 | 0x0 << 1 | 0x1) /* sets prescaler to 1024 */
#define TIMER_INTERRUPT (0x1 << 1) /* enables timer interrupt */

#define TOPH (0x3D) /* together with TOPL, set hardware timer limit of 0d15,624 */
#define TOPL (0x08)

void timer1_init(){
    /* Set timer1 compare for 1s tick */
    /* Set CTC mode */
    /* Set Clock divisor */
    /* Enable interrupts on compare A */
}

unsigned long timer1_get(){
    /* Get global interrupt enable bit state */
    /* Disable interrupts */
    /* Get the count value */
    /* Restore global interrupt state */
    /* Return the count value */
}

void timer1_clear(){
    /* Get global interrupt enable bit state */
    /* Disable interrupts */
    /* count = 0 */
    /* Restore global interrupt state */
}

void __vector_14(){
    /* count++ */
}
