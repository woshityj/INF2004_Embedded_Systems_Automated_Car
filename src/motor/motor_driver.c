#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"

#define GPIO_PIN_PWM_EN1 0
#define GPIO_PIN_PWM_EN2 1

#define GPIO_PIN_MOTOR_IN1 19
#define GPIO_PIN_MOTOR_IN2 18
#define GPIO_PIN_MOTOR_IN3 17
#define GPIO_PIN_MOTOR_IN4 16

#define MOTOR_LEFT 1
#define MOTOR_RIGHT 2

#define PWM_CYCLE 12500

static uint pwm_slice;

// Saves the duty cycle percentage of the motor
//
static uint motor_left_speed = 0;
static uint motor_right_speed = 0;

void move_forward()
{    
    // Left Wheel Configuration
    //
    gpio_put(GPIO_PIN_MOTOR_IN1, 1);
    gpio_put(GPIO_PIN_MOTOR_IN2, 0);

    // Right Wheel Configuration
    //
    gpio_put(GPIO_PIN_MOTOR_IN3, 1);
    gpio_put(GPIO_PIN_MOTOR_IN4, 0);
}

void move_backward()
{
    // Left Wheel Configuration
    //
    gpio_put(GPIO_PIN_MOTOR_IN1, 0);
    gpio_put(GPIO_PIN_MOTOR_IN2, 1);

    // Right Wheel Configuration
    //
    gpio_put(GPIO_PIN_MOTOR_IN3, 0);
    gpio_put(GPIO_PIN_MOTOR_IN4, 1);
}

void turn_left()
{
    // Set Left Wheel to go Forward
    //
    gpio_put(GPIO_PIN_MOTOR_IN1, 1);
    gpio_put(GPIO_PIN_MOTOR_IN2, 0);

    // Set Right Wheel to go Backward
    //
    gpio_put(GPIO_PIN_MOTOR_IN3, 0);
    gpio_put(GPIO_PIN_MOTOR_IN4, 1);
}

void turn_right()
{
    // Set Left Wheel to go Backward
    //
    gpio_put(GPIO_PIN_MOTOR_IN1, 0);
    gpio_put(GPIO_PIN_MOTOR_IN2, 1);

    // Set Right Wheel to go Forward
    //
    gpio_put(GPIO_PIN_MOTOR_IN3, 1);
    gpio_put(GPIO_PIN_MOTOR_IN4, 0);
}

void set_speed(uint duty_cycle, int motor)
{
    if (duty_cycle < 0)
    {
        duty_cycle = 0;
    } 
    else if (duty_cycle > 100)
    {
        duty_cycle = 100;
    }

    uint16_t pwm_level = duty_cycle / 100.f * PWM_CYCLE;
    if (motor == MOTOR_LEFT)
    {
        pwm_set_chan_level(pwm_slice, PWM_CHAN_A, pwm_level);
        motor_left_speed = duty_cycle; 
    }

    if (motor == MOTOR_RIGHT)
    {
        pwm_set_chan_level(pwm_slice, PWM_CHAN_B, pwm_level);
        motor_right_speed = duty_cycle;
    }

    printf("[Motor] Set speed to %u \n", duty_cycle);
}

void motor_driver_init()
{
    printf("[Motor] Init start \n");

    // Initialize GPIO Pins
    //
    gpio_init(GPIO_PIN_MOTOR_IN1);
    gpio_init(GPIO_PIN_MOTOR_IN2);
    gpio_init(GPIO_PIN_MOTOR_IN3);
    gpio_init(GPIO_PIN_MOTOR_IN4);
    
    gpio_set_dir(GPIO_PIN_MOTOR_IN1, GPIO_OUT);
    gpio_set_dir(GPIO_PIN_MOTOR_IN2, GPIO_OUT);
    gpio_set_dir(GPIO_PIN_MOTOR_IN3, GPIO_OUT);
    gpio_set_dir(GPIO_PIN_MOTOR_IN4, GPIO_OUT);

    // Initialize PWM
    //
    gpio_set_function(GPIO_PIN_PWM_EN1, GPIO_FUNC_PWM);
    gpio_set_function(GPIO_PIN_PWM_EN2, GPIO_FUNC_PWM);

    pwm_slice = pwm_gpio_to_slice_num(GPIO_PIN_PWM_EN1);
    pwm_config pwm_conf = pwm_get_default_config();
    pwm_config_set_clkdiv_int(&pwm_conf, 100);
    pwm_config_set_wrap(&pwm_conf, PWM_CYCLE);

    pwm_init(pwm_slice, &pwm_conf, true);
}


int main()
{
    motor_driver_init();
    move_forward();
    set_speed(50, 1);
}