#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <float.h>

#include "hardware/adc.h"
#include "infrared.h"
#include "../mapping/mapper.h"

#include "pico/stdlib.h"

int main()
{
    stdio_init_all();
    adc_init();
    adc_gpio_init(IR_PIN_FRONT);
    adc_select_input(ADC_FRONT);
    while(1)
    {
        printf("%d\n", adc_read());
        sleep_ms(100);
    }
    return 0;
}