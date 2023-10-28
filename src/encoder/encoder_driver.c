/** @file encoder_driver.h
*
* @brief Source file for HCMODU0240 Wheel Encoder Sensor Driver.
*
* @par
* COPYRIGHT NOTICE: (c) 2023. This software is placed in the
* public domain and may be used for any purpose. However, this notice must not
* be changed or removed. No warranty is expressed or implied by the publication
* or distribution of this source code.
*/

#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "pico/time.h"

#include "encoder_driver.h"
#include "../motor/motor_driver.c"

// Pins connected to each sensors's OUT pin.
//
#define GPIO_PIN_ENC_LEFT 2
#define GPIO_PIN_ENC_RIGHT 3

#define INTERRUPT_BUF_SIZE 5

// Number of disc slots the Wheel has
//
#define ENCODER_DISC_SLOTS 20

// Diameter of the Wheel
//
#define WHEEL_DIAMETER_CM 6.7

// Circumference of the Wheel
//
#define WHEEL_CIRCUMFERENCE_CM (M_PI * WHEEL_DIAMETER_CM)

volatile int newTime = 0;
volatile int oldTime = 0;
volatile int pulseTime = 0;

// Wheel speed calculation
volatile int encIndex = 0;
volatile int leftInterruptBuffer[INTERRUPT_BUF_SIZE];
volatile int rightInterruptBuffer[INTERRUPT_BUF_SIZE];

/*!
*	@brief	            This callback function is called when the Encoder Sensor is interrupted (low)
*                       or when the Encoder Sensor senses the split from the Wheel passing its IR emitter
*                       and receiver
*   @param[in]  gpio    Contains the GPIO number to identify which Encoder Sensor called the callback function.
*   @param[in]  events  Contains which events caused the callback function to be called
*                       which is an Edge Fall.
*/
void gpio_callback_isr(uint gpio, uint32_t events)
{
    if (gpio == GPIO_PIN_ENC_LEFT)
    {
        leftInterruptBuffer[encIndex]++;
    }

    if (gpio == GPIO_PIN_ENC_RIGHT)
    {
        rightInterruptBuffer[encIndex]++;
    }
}

bool repeating_timer_callback_isr(struct repeating_timer *t)
{
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
    else if (gpio == GPIO_PIN_ENC_RIGHT)
    {
        for (int i = 0; i < INTERRUPT_BUF_SIZE; i++)
        {
            if (i == encIndex)
                continue;
            total += rightInterruptBuffer[i];
        }
    }
    return total;
}

float get_wheel_speed(uint gpio)
{
    int interrupts_per_sec = get_wheel_interrupt_speed(gpio);
    return interrupts_per_sec * CM_PER_SLOT;
}

void encoder_driver_init()
{   
    printf("[Encoder] Init start\n");

    struct repeating_timer timer;

    add_repeating_timer_ms(-250, &repeating_timer_callback_isr, NULL, &timer);

    gpio_set_irq_enabled_with_callback(GPIO_PIN_ENC_LEFT, GPIO_IRQ_EDGE_FALL, true, &gpio_callback_isr);
    gpio_set_irq_enabled_with_callback(GPIO_PIN_ENC_RIGHT, GPIO_IRQ_EDGE_FALL, true, &gpio_callback_isr);
}


int main()
{
    stdio_usb_init();

    encoder_driver_init();
    // motor_driver_init();
    // move_forward();
    // set_speed(100, 1);


    while(true)
    {
        // float speed = get_wheel_speed(GPIO_PIN_ENC_LEFT);
        // printf("Speed is %0.2f cm/s\n", speed);
        printf("semen\n");

        sleep_ms(1000);
    }
}