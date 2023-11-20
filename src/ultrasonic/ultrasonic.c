#include <stdio.h>
#include "pico/stdlib.h"
#include <stdbool.h>
#include "hardware/gpio.h"
#include "hardware/timer.h"

#include "ultrasonic.h"

struct repeating_timer ultrasonic_timer;
// Abstracted task returns
// to be done

bool getObstacle()
{
    short numOfZeros = 0;
    short numOfValids = 0;
    int totalDistanceInMM = 0;
    unsigned short millimeters = 0;
    for(int i = 0; i < SAMPLING_RATE; i++)
    {
        millimeters = getMM();
        // Filtering out the "too high" and "too low" values
        if(millimeters == 0)
        {
            numOfZeros++;
            if(numOfZeros >= ZERO_THRESHOLD)
            {
                return false;
            }
        }

        else
        {
            totalDistanceInMM += millimeters;
            numOfValids++;
        }
        // This sleep simulates the vTaskDelay() of 50ms, this simulates it being a non-blocking delay when using FreeRTOS-Kernel
        vTaskDelay(pdMS_TO_TICKS(30));
    }

    short average_millimeters = totalDistanceInMM / numOfValids;

    // The obstacle is too far to care
    if(average_millimeters >= DISTANCE_THRESHOLD)
    {
        return false;
    }

    return true;
}

unsigned short getMM()
{
    pulseLength = DIFFERENCE(startTime, endTime); // get the echo pin return wave form length
    unsigned short milliimeters = pulseLength / 6;
    if(milliimeters < 20 || milliimeters > 4000 || pulseLength >= TIMEOUT) // range of the HC-SR04P
    {
        milliimeters = 0; // if out of range then just assume that it was a 0
    }
    // reset the parameters for the next echo_interrupt
    endTime = 0;
    startTime = 0;
    return milliimeters;
}

void initializeUltrasonic()
{
    // pin initializings
    gpio_init(TRIGGER_PIN);
    gpio_init(ECHO_PIN);

    gpio_set_dir(TRIGGER_PIN, GPIO_OUT);
    gpio_set_dir(ECHO_PIN, GPIO_IN);

    // gpio_set_irq_enabled_with_callback(ECHO_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &echo_interrupt); //enable interrupts by the echo pin
    add_repeating_timer_ms(30, pullTrigger, NULL, &ultrasonic_timer); // Happens to be near 30Hz by setting it to be -30
}

int64_t alarm_pulldown_callback(alarm_id_t id, void *user_data)
{
    gpio_put(TRIGGER_PIN, 0); // Pull the trigger pin down to signify the sonic burst start   
    return 0;
}

void echo_interrupt(uint gpio, uint32_t events)
{
    if(events & GPIO_IRQ_EDGE_RISE) // Ultrasonic sends the signal back
    {
        if(startTime == 0)
        {
            startTime = time_us_64();    
        }
    }

    else if(events & GPIO_IRQ_EDGE_FALL) // Ultrasonic finishes sending signal
    {
        if(endTime == 0)
        {
            endTime = time_us_64();
        }
    }
}

bool pullTrigger(struct repeating_timer *t) // repeating timer to periodically call the 
{
    gpio_put(TRIGGER_PIN, 1);

     // requires AT LEAST 10us for the sonic burst
    add_alarm_in_us(15, &alarm_pulldown_callback, NULL, false);

    return true;
}