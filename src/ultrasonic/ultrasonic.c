#include <stdio.h>
#include "pico/stdlib.h"
#include <stdbool.h>
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "ultrasonic.h"

#define SPEED_OF_SOUND 343 / 1000000 // meters per microsecond
#define DIFFERENCE(a, b) (a<b ? (b-a) : (a-b))


int TRIGGER_PIN = 14;
int ECHO_PIN = 15;
int width = 0;

int timeout = 26100;
uint64_t startTime;
uint64_t endTime;

struct repeating_timer ultrasonicTimer;
struct repeating_timer widthTimer;

bool repeatedPullTheTrigger()
{
    printf("pulled trigger\n");
    bool currentState = !gpio_get(TRIGGER_PIN);
    gpio_put(TRIGGER_PIN, currentState); // Send the signal to the ultrasonic sensor to turn it on
    bool pinState = gpio_get(TRIGGER_PIN);
    if (pinState)
    {
        printf("pin state = %d", pinState);
        add_alarm_in_us(10, repeatedPullTheTrigger, NULL, false); // set an alarm to call itself back in 10us to turn the pin off
    }
    printf("leaving trigger\n");
    return true;
}

void echo_interrupt(uint gpio, uint32_t events)
{
    printf("echo interrupts\n");
    if(events & GPIO_IRQ_EDGE_RISE) // Ultrasonic sends the signal back
    {
        if(startTime == 0)
        {
            startTime = time_us_64();    
        }
        width++;
    }

    else if(events & GPIO_IRQ_EDGE_FALL) // Ultrasonic finishes sending signal
    {
        if(width == 8) // counts up to 8 echo pulses on the edge_fall, 
        {
            endTime = time_us_64();
        }
        uint64_t difference = DIFFERENCE(endTime, startTime); // Time given in microseconds
        uint64_t distance = (difference*SPEED_OF_SOUND) / 2;
        width = 0;
        startTime = 0;
        endTime = 0;
        printf("The distance is %llu\n", distance);
    }
}

int main(void)
{
    stdio_init_all();
    sleep_ms(3000);

    gpio_init(TRIGGER_PIN);
    gpio_init(ECHO_PIN);

    gpio_set_dir(TRIGGER_PIN, GPIO_OUT);
    gpio_set_dir(ECHO_PIN, GPIO_IN);

    // just for testing purposes
    while(true)
    {
        repeatedPullTheTrigger();
        sleep_ms(10000);
    }
    gpio_set_irq_enabled_with_callback(ECHO_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &echo_interrupt);

    while(1);
}