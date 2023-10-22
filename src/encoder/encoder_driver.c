#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "pico/time.h"

#include "encoder_driver.h"

#define GPIO_PIN_ENC_LEFT 2
#define GPIO_PIN_ENC_RIGHT 3

#define INTERRUPT_BUF_SIZE 5

#define ENCODER_DISC_SLOTS 20
#define WHEEL_DIAMETER_CM 6.7
#define WHEEL_CIRCUMFERENCE_CM (M_PI * WHEEL_DIAMETER_CM)

const float CM_PER_SLOT = WHEEL_CIRCUMFERENCE_CM / ENCODER_DISC_SLOTS;

volatile int newTime = 0;
volatile int oldTime = 0;
volatile int pulseTime = 0;

// Wheel speed calculation
volatile int encIndex = 0;
volatile int leftInterruptBuffer[INTERRUPT_BUF_SIZE];
volatile int rightInterruptBuffer[INTERRUPT_BUF_SIZE];

volatile int encoderISRLeftCounter = 0;

// int get_rpm()
// {
//     if (newTime != 0)
//     {
//         pulseTime = newTime - oldTime;

//         if (pulseTime < ((time_us_32() / 1000) - newTime)) 
//         {
//             pulseTime = (time_us_32() / 1000) - newTime;
//         }

//         return 60000000 / pulseTime;
//     }
//     return 0;
// }

void gpio_callback_isr(uint gpio, uint32_t events)
{
    if (gpio == GPIO_PIN_ENC_LEFT)
    {
        // oldTime = newTime;
        // newTime = (time_us_32() / 1000);

        leftInterruptBuffer[encIndex]++;
    }
}

bool repeating_timer_callback_isr(struct repeating_timer *t)
{
    // int rpm = get_rpm();
    // printf("RPM is %d\n", rpm);
    // return true;

    encIndex++;
    if (encIndex == INTERRUPT_BUF_SIZE)
        encIndex = 0;
    leftInterruptBuffer[encIndex] = 0;
    rightInterruptBuffer[encIndex] = 0;
    return true;
}

int get_wheel_interrupt_speed(uint gpio)
{
    int total = 0;
    if (gpio == GPIO_PIN_ENC_LEFT)
    {
        for (int i = 0; i < INTERRUPT_BUF_SIZE; i++)
        {
            if (i == encIndex)
                continue;
            total += leftInterruptBuffer[i];
        }
    }
    return total;
}

float get_wheel_speed(uint gpio)
{
    int interrupts_per_sec = get_wheel_interrupt_speed(gpio);
    return interrupts_per_sec * CM_PER_SLOT;
}


int main()
{
    stdio_usb_init();
    printf("[Encoder] Init start \n");

    struct repeating_timer timer;

    add_repeating_timer_ms(-250, repeating_timer_callback_isr, NULL, &timer);

    gpio_set_irq_enabled_with_callback(GPIO_PIN_ENC_LEFT, GPIO_IRQ_EDGE_FALL, true, &gpio_callback_isr);
    while(true)
    {
        float speed = get_wheel_speed(GPIO_PIN_ENC_LEFT);
        printf("Speed is %0.2f cm/s\n", speed);

        sleep_ms(1000);
    }
}