#include <stdio.h>
#include "pico/stdlib.h"
#include <stdbool.h>
#include "hardware/gpio.h"
#include "hardware/timer.h"

#include "ultrasonic.h"

int timeout = 26100;

absolute_time_t startTime;
absolute_time_t endTime;
absolute_time_t pulseLength;

void initializeUltrasonic(uint triggerPin, uint echoPin);
void echo_interrupt(uint gpio, uint32_t events);
int64_t alarm_pulldown_callback(alarm_id_t id, void *user_data);

// Abstracted task returns
/*float getCM()
{
    gpio_put(TRIGGER_PIN, 1);
    add_alarm_in_us(15, &alarm_pulldown_callback, NULL, false); // requires at least 10us for the sonic burst
    sleep_us(15); // act like put into blocked state, will replace with vTaskDelay()
    if(endTime > startTime)
    {
        float centimeters = getPulse();
        printf("The distance is: %.2f\n", centimeters);
    }
    return 
}*/

float getPulse()
{
    pulseLength = absolute_time_diff_us(startTime, endTime); // get the echo pin return wave form length
    float centimeters = pulseLength / 58;
    if(centimeters < 2 || centimeters > 400 || pulseLength >= timeout) // range of the HC-SR04P
    {
        centimeters = 0; // if out of range then just assume that it was a 0
    }
    endTime = 0;
    startTime = 0;
    return centimeters;
}

void initializeUltrasonic(uint triggerPin, uint echoPin)
{
    gpio_init(triggerPin);
    gpio_init(echoPin);

    gpio_set_dir(triggerPin, GPIO_OUT);
    gpio_set_dir(echoPin, GPIO_IN);
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

int main(void)
{
    stdio_init_all();

    // sleep for time to turn on serial monitor
    sleep_ms(3000);
    printf("initializing the sensors\n");

    initializeUltrasonic(TRIGGER_PIN, ECHO_PIN);

    gpio_set_irq_enabled_with_callback(ECHO_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &echo_interrupt);
    
    // Just for testing purposes to show the usage, will be abstracted away for tasks, this while loop simulates tasks that don't have a return
    while(true)
    {
        gpio_put(TRIGGER_PIN, 1);
        add_alarm_in_us(15, &alarm_pulldown_callback, NULL, false); // requires at least 10us for the sonic burst
        sleep_us(15); // act like put into blocked state, will replace with vTaskDelay()
        if(endTime > startTime)
        {
            float centimeters = getPulse();
            printf("The distance is: %.2f\n", centimeters);
        }
        // You can comment the sleep_ms(100) to limit test, but it won't be as accurate because the sound waves can't travel fast enough
        sleep_ms(100); 
        // the sleep here is obviously just for testing, in the implementation it'll be replaced with vTask delays or function calls with delayUntil
    }
}