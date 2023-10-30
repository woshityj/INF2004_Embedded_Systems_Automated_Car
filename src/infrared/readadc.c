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
    static float movingAvg_data[10] = {0};
    static int movingAvg_index = 0;
    static int movingAvg_count = 0;
    float movingAvg_sum = 0;
    float movingAvg_result = 0;

    stdio_init_all();
    adc_init();
    adc_gpio_init(IR_PIN_FRONT);
    adc_select_input(ADC_FRONT);
    while(1)
    {
        movingAvg_sum -= movingAvg_data[movingAvg_index];
        movingAvg_data[movingAvg_index] = adc_read();
        movingAvg_sum += movingAvg_data[movingAvg_index];
        movingAvg_index = (movingAvg_index + 1) % 10;

        if (movingAvg_count < 10) movingAvg_count++;

        movingAvg_result = movingAvg_sum/movingAvg_count;
        printf("%f\n", movingAvg_result);
        sleep_ms(100);
    }
    return 0;
}