/* Register Macros */
#define SREG (*(volatile char*) 0x5F )
#define TCCR0A (*(volatile unsigned char*) 0x44 )
#define TCCR0B (*(volatile unsigned char*) 0x45 )
#define TIMSK0 (*(volatile unsigned char*) 0x6E )
#define OCR0A (*(volatile unsigned char*) 0x47 )

/* Timer Configuration Macros */
#define COMA (0x0 << 7 | 0x0 << 6)
#define COMB (0x0 << 5 | 0x0 << 4)
#define WGMA (0x1 << 1 | 0x0)
#define WGMB (0x0 << 3)
#define CS (0x1 << 1 | 0x1)
#define ENABLE_TIMER_INTERRUPT (0x1 << 1)
#define TIMER_LIMIT (0xF9)

/*Internal State*/
static int count[2];
static int limit[2];
static int initialized = 0;

static void delay_init(){
    /* Set timer0 compare to 1ms tick */
    /* Set CTC mode and clock divisor */
    /* Enable interrupts on compare A */
    /* Stop further initialization */

    /* Set normal port operation and ctc mode */
    TCCR0A = COMA | COMB | WGMA
    /* Set normal port operation and select 64 prescaler */
    TCCR0B = WGMB | CS
    /* Enable compare match interrupt */
    TIMSK0 = ENABLE_TIMER_INTERRUPT
    /* Set timer limit to 0d249 */
    OCR0A = TIMER_LIMIT

    count[0] = 0;
    count[1] = 0;
    limit[0] = 0;
    limit[1] = 0;

    initialized = 1;
}

unsigned int delay_get(unsigned int num){
    /* Get global interrupt enable  */
    unsigned char sreg_state = SREG;

    /* Disable interrupts */
    __builtin_avr_cli();

    /* Get count[n] value */
    unsigned int value = count[num];

    /* Restore global interrupt state */
    SGEG = sreg_state;

    /* Return the count value */
    return value;
}

void delay_set(unsigned int num, unsigned int time){
    /* Initialize the delay counter */
    if(initialized == 0){
       delay_init();
    }
    /* Get global interrupt bit state */
    unsigned int sreg_state = SREG;

    /* Disable interrupts */
    __builtin_avr_cli();

    /* Set the time limit for delay[n] and clear the count for delay[n] */
    limit[num] = time;
    count[num] = 0;

    /* Restore the global interrupt state */
    SREG = sreg_state;
}

unsigned int delay_isdone(unsigned int num){
    /* result = 0 */
    unsigned int isdone = 0;

    /* result = 1 */
    if(count[num] == limit[num]){
       isdone = 1;
    }

    /* return result */
    return isdone;
}

void __vector_14(void) __attribute__ ((signal,used, externally_visible));
void __vector_14(){
    /* For each instance of delay, increment its count as long as it is gte than its limit */
    count[0]++;
    count[1]++;
}
