#include "uart.h"
#include "simpleled.h"
#include "simpledelay.h"
#include "temp.h"

int main(void)
{

    // Insert code
    uart_init();
    led_init();
    temp_init();

    uart_writestr("SER486 HW4 Jose Solis Salazar\n\r");

    while(1){
        temp_start();

        while(!temp_is_data_ready()){
            // do nothing but wait
        }
        signed int temperature = temp_get();
        uart_writedec32(temperature);
        uart_writestr("\n\r");
        delay(1000);
    }

    return 0;
}
