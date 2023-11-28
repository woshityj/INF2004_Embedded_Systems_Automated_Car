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
#include "../ultrasonic/ultrasonic.h"

// Pins connected to each sensors's OUT pin.
//
#define GPIO_PIN_ENC_LEFT 2
#define GPIO_PIN_ENC_RIGHT 3

// Number of disc slots the Wheel has
//
#define ENCODER_DISC_SLOTS 20

// Diameter of the Wheel
//
#define WHEEL_DIAMETER_CM 6.7

// Circumference of the Wheel
//
#define WHEEL_CIRCUMFERENCE_CM (M_PI * WHEEL_DIAMETER_CM)


// Global Variables for ISR Counter
volatile int enable_isr_counter = 0;
volatile int left_interrupts_isr_counter = 0;
volatile int right_interrupts_isr_counter = 0;
volatile int left_interrupts_isr_target_reached = 0;
volatile int right_interrupts_isr_target_reached = 0;
volatile int isr_target = 0;

struct repeating_timer isr_alert_timer;

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
        if (enable_isr_counter && !left_interrupts_isr_target_reached)
        {
            left_interrupts_isr_counter += 1;

            if (left_interrupts_isr_counter == isr_target)
            {
                left_interrupts_isr_target_reached = 1;
            }
        }
    }

    if (gpio == GPIO_PIN_ENC_RIGHT)
    {
        rightOldTime = rightNewTime;
        rightNewTime = time_us_32();

        right_interrupts += 1;

        if (enable_isr_counter && !right_interrupts_isr_target_reached)
        {
            right_interrupts_isr_counter += 1;

            if (right_interrupts_isr_counter == isr_target)
            {
                right_interrupts_isr_target_reached = 1;
            }
        }
    }

    if (gpio == ECHO_PIN)
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

            // According to the datasheet to get the RPM it is
            // 60000000 / pulseTime, however as there are 20 holes in the 
            // Wheel, it has become (60000000 / 20) / pulseTime
            //
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
            
            // According to the datasheet to get the RPM it is
            // 60000000 / pulseTime, however as there are 20 holes in the 
            // Wheel, it has become (60000000 / 20) / pulseTime
            //
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

/*!
*	@brief      This function initializes GPIO callback functions
*               for both the Left and Right Encoders
*/
void encoder_driver_init()
{
    printf("[Encoder] Init start\n");

    gpio_set_irq_enabled_with_callback(GPIO_PIN_ENC_LEFT, GPIO_IRQ_EDGE_RISE, true, &gpio_callback_isr);
    gpio_set_irq_enabled_with_callback(GPIO_PIN_ENC_RIGHT, GPIO_IRQ_EDGE_RISE, true, &gpio_callback_isr);
    gpio_set_irq_enabled_with_callback(ECHO_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback_isr); //enable interrupts by the echo pin
}

/*!
*	@brief	            This function converts the distance in cm to the number of interrupts or 
*                       slits on the wheel encoder that has to pass to travel the specified distance
*   @param[in]  cm      Distance in cm to convert to the number of interrupts
*	@return             Returns the total number of interrupts to travel the specified distance in cm
*/
int cm_to_interrupts(int cm)
{
    return (cm / CM_PER_SLOT);
}

/*!
*	@brief	            This function calculates the distance that the specified Wheel Encoder Sensor
*                       
*   @param[in]  gpio    Contains the GPIO number to identify which Encoder Sensor to get the distance for
*	@return             Returns the distance in CM for the specified Wheel Encoder Sensor
*/
bool timer_callback_isr_alert(struct repeating_timer *t)
{
    if (left_interrupts_isr_target_reached && right_interrupts_isr_target_reached)
    {
        repeating_timer_callback_t callback = (repeating_timer_callback_t)(t->user_data);
        enable_isr_counter = 0;
        return callback(t); // Stops the timer
    }
    else
    {
        return true;
    }
}

void encoder_alert_after_isr_interrupt(uint target, repeating_timer_callback_t callback)
{
    left_interrupts_isr_counter = 0;
    right_interrupts_isr_counter = 0;
    left_interrupts_isr_target_reached = 0;
    right_interrupts_isr_target_reached = 0;
    isr_target = target;
    enable_isr_counter = 1;

    add_repeating_timer_ms(-2, timer_callback_isr_alert, callback, &isr_alert_timer);
}