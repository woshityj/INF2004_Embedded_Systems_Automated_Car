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

struct repeating_timer timer;

volatile uint32_t leftNewTime = 0;
volatile uint32_t leftOldTime = 0;
volatile uint32_t leftPulseTime = 0;

volatile uint32_t rightNewTime = 0;
volatile uint32_t rightOldTime = 0;
volatile uint32_t rightPulseTime = 0;

volatile uint32_t left_interrupts = 0;
volatile uint32_t right_interrupts = 0;

/*!
*	@brief	            This callback function is called when the Encoder Sensor is high (Edge Rise)
*                       or when the Encoder Sensor senses the split from the Wheel passing its IR emitter
*                       and receiver and adds to a counter that counts the number of splits
*   @param[in]  gpio    Contains the GPIO number to identify which Encoder Sensor called the callback function.
*   @param[in]  events  Contains which events caused the callback function to be called
*                       which is an Edge Rise.
*/
void gpio_callback_isr(uint gpio, uint32_t events)
{
    if (gpio == GPIO_PIN_ENC_LEFT)
    {
        leftOldTime = leftNewTime;
        leftNewTime = time_us_32();

        left_interrupts += 1;
    }

    if (gpio == GPIO_PIN_ENC_RIGHT)
    {
        rightOldTime = rightNewTime;
        rightNewTime = time_us_32();

        right_interrupts += 1;
    }
}

/*!
*	@brief	            This function calculates the number of rotations (360 degress) that the specified
*                       Wheel Encoder Sensor detects based on the amount of time the IR emitter and
*                       receiver was blocked for which resulted in a low
*   @param[in]  gpio    Contains the GPIO number to identify which Encoder Sensor to get the RPM for
*	@return             Returns number of rotations per minute for the specified Wheel Encoder Sensor
*/
int get_wheel_rpm(uint gpio)
{
    if (gpio == GPIO_PIN_ENC_LEFT)
    {
        if (leftNewTime != 0)
        {
            leftPulseTime = leftNewTime - leftOldTime;

            if (leftPulseTime < (time_us_32() - leftNewTime))
            {
                leftPulseTime = time_us_32() - leftNewTime;
            }

            int rpm = 30000000 / leftPulseTime;

            return rpm;
        }
    }

    if (gpio == GPIO_PIN_ENC_RIGHT)
    {
        if (rightNewTime != 0)
        {
            rightPulseTime = rightNewTime - rightOldTime;

            if (rightPulseTime < (time_us_32() - rightNewTime))
            {
                rightPulseTime = time_us_32() - rightNewTime;
            }

            int rpm = 30000000 / rightPulseTime;

            return rpm;
        }
    }

    return 0;
}

/*!
*	@brief	            This function calculates the speed of the specified Wheel Encoder Sensor based
*                       on the rotations per minute
*   @param[in]  gpio    Contains the GPIO number to identify which Encoder Sensor to get the speed for
*	@return             Returns the speed in KM/H for the specified Wheel Encoder Sensor
*/
int get_wheel_speed(uint gpio)
{
    int rpm = get_wheel_rpm(gpio);

    int speed = (rpm * WHEEL_CIRCUMFERENCE_CM) * 60;
    speed = speed / 100000;

    return speed;
}

/*!
*	@brief	            This function calculates the distance that the specified Wheel Encoder Sensor
*                       travelled for
*   @param[in]  gpio    Contains the GPIO number to identify which Encoder Sensor to get the distance for
*	@return             Returns the distance in CM for the specified Wheel Encoder Sensor
*/
float get_wheel_distance(uint gpio)
{
    if (gpio == GPIO_PIN_ENC_LEFT)
    {
        float distance = left_interrupts * CM_PER_SLOT;
        printf("[Testing] Left interrupts :%d\n", left_interrupts);

        return distance;       
    }

    if (gpio == GPIO_PIN_ENC_RIGHT)
    {
        float distance = right_interrupts * CM_PER_SLOT;

        return distance;
    }

    return 0;
}


void encoder_driver_init()
{
    printf("[Encoder] Init start\n");

    gpio_set_irq_enabled_with_callback(GPIO_PIN_ENC_LEFT, GPIO_IRQ_EDGE_RISE, true, &gpio_callback_isr);
    gpio_set_irq_enabled_with_callback(GPIO_PIN_ENC_RIGHT, GPIO_IRQ_EDGE_RISE, true, &gpio_callback_isr);
}


int main()
{
    stdio_usb_init();
    motor_driver_init();
    encoder_driver_init();
    move_forward();
    set_speed(100, 1);

    while(true)
    {
        int speed = get_wheel_speed(2);
        printf("Speed is %d km/h\n", speed);


        sleep_ms(5000);

        float distance = get_wheel_distance(2);
        printf("Distance travelled is: %0.2f cm\n", distance);
    }
}