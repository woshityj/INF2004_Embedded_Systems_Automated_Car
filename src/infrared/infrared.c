#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <float.h>
#include "hardware/adc.h"
#include "../mapping/mapper.h"

#include "pico/stdlib.h"

#define SAMPLE_RATE_MS 10
#define IR_PIN_FRONT 26
#define IR_PIN_LEFT 26
#define TIMING_BUFFERSIZE 10
#define TIMINGDIFFERENCES_BUFFERSIZE 9

struct repeating_timer timer;

static int timings[TIMING_BUFFERSIZE];
static float timing_differences[TIMINGDIFFERENCES_BUFFERSIZE];
static int char_array[TIMINGDIFFERENCES_BUFFERSIZE];

bool IR_getWall(int direction, int currentlyFacing);
bool IR_barcode_scan(struct repeating_timer *t);
void IR_init();
void formChar(float first, float second, float third);
void findTopThree(float arr[], float *first, float *second, float *third);

bool IR_getWall(int direction, int currentlyFacing)
{
    return true;
}

bool IR_barcode_scan(struct repeating_timer *t)
{
    static bool scanReady = true;
    static bool flag = true;
    static int currentIndex = 0;
    adc_select_input(0);

    if (!scanReady)
    {
        scanReady = true;
        flag = true;
        return true;
    }
    
    // Check if bar is black
    //
    if (adc_read() > 1500)
    {
        if (flag)
        {
            flag = false;
            // sleep_us(100);
            timings[currentIndex] = to_us_since_boot(get_absolute_time());
            currentIndex+=1;
        }
    }
    else
    {
        if (!flag)
        {
            flag = true;
            // sleep_us(100);
            timings[currentIndex] = to_us_since_boot(get_absolute_time());
            currentIndex += 1;
        }
    }

    if (currentIndex == TIMING_BUFFERSIZE)
    {
        for (int i = 0; i < TIMINGDIFFERENCES_BUFFERSIZE; i++)
        {
            double first_timing_s = timings[i]/1000000.0;
            double second_timing_s = timings[i+1]/1000000.0;
            timing_differences[i] = second_timing_s - first_timing_s;
            printf("%f\n", timing_differences[i]); 
        }
        float first, second, third;
        findTopThree(timing_differences, &first, &second, &third);
        formChar(first,second,third);
        for (int i = 0; i < TIMINGDIFFERENCES_BUFFERSIZE; i++)
        {
            printf("%d", char_array[i]);
        }
        printf("\n--- End of a set ---\n\n");
        currentIndex = 0;
        scanReady = false;
    }
    
    return true;
    
}

void formChar(float first, float second, float third)
{
    for (int i = 0; i < TIMINGDIFFERENCES_BUFFERSIZE; i++) {
        if (timing_differences[i] == first || timing_differences[i] == second || timing_differences[i] == third)
        {
            char_array[i] = 1;
        } else
        {
            char_array[i] = 0;
        }
    }
}

void findTopThree(float arr[], float *first, float *second, float *third)
{
    *first = FLT_MIN;
    *second = FLT_MIN;
    *third = FLT_MIN;

    for (int i = 0; i < TIMINGDIFFERENCES_BUFFERSIZE; i++)
    {
        if (arr[i] > *first)
        {
            *third = *second;
            *second = *first;
            *first = arr[i];
        } else if (arr[i] > *second)
        {
            *third = *second;
            *second = arr[i];
        } else if (arr[i] > *third)
        {
            *third = arr[i];
        }
    }
}

void IR_init()
{
    adc_init();
    adc_gpio_init(IR_PIN_LEFT);
    adc_select_input(0);
}

int main()
{
    stdio_init_all();
    IR_init();

    add_repeating_timer_ms(SAMPLE_RATE_MS, IR_barcode_scan, NULL, &timer);

    while (1)
    {

    };

    return 0;
}