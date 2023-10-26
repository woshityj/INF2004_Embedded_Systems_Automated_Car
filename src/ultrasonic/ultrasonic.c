#include <stdio.h>
#include "pico/stdlib.h"
#include <stdbool.h>
#include "hardware/gpio.h"
#include "hardware/timer.h"
// #include "ultrasonic.h"

// #define SPEED_OF_SOUND 343 / 1000000 // meters per microsecond
// #define DIFFERENCE(a, b) (a<b ? (b-a) : (a-b))
#define TRIGGER_PIN 14
#define ECHO_PIN 15


int timeout = 26100;
float centimeters = 0.0f;

absolute_time_t startTime;
absolute_time_t endTime;

void initializeUltrasonic(uint triggerPin, uint echoPin);
void echo_interrupt(uint gpio, uint32_t events);
int64_t alarm_pulldown_callback(alarm_id_t id, void *user_data);

// Auxillary function that could be used in the future

/*float getCM(uint triggerPin, uint, echoPin)
{
    float pulseLength = getPulse(triggerPin, echoPin) / 58;
    printf("measured:\t%.2f\n", pulseLength);
    return pulseLength;
}*/

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
    //printf("echo interrupts\n");
    if(events & GPIO_IRQ_EDGE_RISE) // Ultrasonic sends the signal back
    {
        if(startTime == 0)
        {
            startTime = get_absolute_time();    
        }
    }

    else if(events & GPIO_IRQ_EDGE_FALL) // Ultrasonic finishes sending signal
    {

        endTime = get_absolute_time();
        
        absolute_time_t pulseLength = absolute_time_diff_us(startTime, endTime); // get the echo pin return wave form length
        // printf("difference is %llu\n", pulseLength); debugging purposes
        centimeters = pulseLength / 58; // convert to centimeters and assign it as a float value
        
        if(pulseLength >= timeout)
        {
            printf("The distance is 0cm\n");
        }
        else
        {
            printf("The distance is: %.2fcm\n", centimeters);
        }

        // reset the parameters
        startTime = 0;
        endTime = 0;
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
    
    // just for testing purposes to show the usage
    while(true)
    {
        gpio_put(TRIGGER_PIN, 1);
        add_alarm_in_us(15, &alarm_pulldown_callback, NULL, false); // requires at least 10us for the sonic burst
        sleep_ms(1000);
        // the sleep here is obviously just for testing, in the implementation it'll be replaced with vTask delays or function calls with delayUntil
    }
}