#include <stdio.h>
#include "pico/stdlib.h"
#include <stdbool.h>
#include "hardware/gpio.h"
#include "hardware/timer.h"

#include "ultrasonic.h"

struct repeating_timer timer;
// Abstracted task returns
// to be done

unsigned short getMM()
{
    pulseLength = absolute_time_diff_us(startTime, endTime); // get the echo pin return wave form length
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

void initializeUltrasonic(unsigned char triggerPin, unsigned char echoPin)
{
    // pin initializings
    gpio_init(triggerPin);
    gpio_init(echoPin);

    gpio_set_dir(triggerPin, GPIO_OUT);
    gpio_set_dir(echoPin, GPIO_IN);

    gpio_set_irq_enabled_with_callback(ECHO_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &echo_interrupt); //enable interrupts by the echo pin
    add_repeating_timer_ms(SAMPLING_RATE, pullTrigger, NULL, &timer); // Happens to be near 30Hz by setting it to be -30
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
            startTime = get_absolute_time();    
        }
    }

    else if(events & GPIO_IRQ_EDGE_FALL) // Ultrasonic finishes sending signal
    {
        if(endTime == 0)
        {
            endTime = get_absolute_time();
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