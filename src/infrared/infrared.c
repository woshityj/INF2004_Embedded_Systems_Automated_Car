#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "pico/stdlib.h"

#define IR_PIN 15
#define MAX_TIMINGS 10
#define MAX_TIMINGDIFFERENCES 9

void IR_scan();
void IR_init();

void IR_scan()
{
    static bool flag = true;
    static int timings[MAX_TIMINGS];
    static float timing_differences[MAX_TIMINGDIFFERENCES];
    static int currentIndex = 0;

    if (gpio_get(IR_PIN) == 1)
    {
        if (flag)
        {
            flag = false;
            sleep_us(100);
            timings[currentIndex] = to_us_since_boot(get_absolute_time());
            currentIndex+=1;
        }
        
    }
    else
    {
        if (!flag)
        {
            flag = true;
            sleep_us(100);
            timings[currentIndex] = to_us_since_boot(get_absolute_time());
            currentIndex += 1;
        }
        
    }

    if (currentIndex == 11)
    {
        for (int i = 0; i < MAX_TIMINGDIFFERENCES; i++)
        {
            double first_timing_s = timings[i]/1000000.0;
            double second_timing_s = timings[i+1]/1000000.0;
            timing_differences[i] = second_timing_s - first_timing_s;
            printf("%f\n", timing_differences[i]); 
        }
        currentIndex = 0;
    }
    
    
}

void IR_init()
{
    gpio_init(IR_PIN);
    gpio_set_dir(IR_PIN, GPIO_IN);
}

int main()
{
    stdio_init_all();
    IR_init();

    while (true) {
        IR_scan();
        sleep_ms(10);
    }

    return 0;
}