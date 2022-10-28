#define ADMUX (* ((volatile char*) 0x7C))
#define ADCSRA ( *((volatile char*) 0x7A))
#define ADCL (* ((volatile char*) 0x78))
#define ADCH (* ((volatile char*) 0x79))
#define VOLTAGE_REF ((unsigned char) 0b11)
#define INPUT_SELECTOR ((unsigned char) 0b1000)
#define DIVISOR_SELECTOR ((unsigned char) 0b110)
#define CONVERSION_SELECTOR ((unsigned char) 0b1)

void temp_init()
{
    unsigned char adMuxConfig = ((VOLTAGE_REF << 6) | (INPUT_SELECTOR));
    unsigned char adcsraConfig = ((CONVERSION_SELECTOR << 7) | (DIVISOR_SELECTOR));

    ADMUX |= adMuxConfig;
    ADCSRA |= adcsraConfig;
}

int temp_is_data_ready()
{
    unsigned char result = (((ADCSRA) & 0x40) >>6);

    if (result == 0) {
        return 1;
    }

    return 0;
}

void temp_start()
{
    ADCSRA |= 0x40;
}

signed int temp_get()
{
    signed long reading = (ADCL);
    reading |= ((ADCH) << 8);

    signed long degrees = ((reading * 101) / 100) - 273;

    return degrees;
}
