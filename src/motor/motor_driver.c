#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"

#include "motor_driver.h"

#define GPIO_PIN_PWM_EN1 0
#define GPIO_PIN_PWM_EN2 1

#define GPIO_PIN_MOTOR_IN1 19
#define GPIO_PIN_MOTOR_IN2 18
#define GPIO_PIN_MOTOR_IN3 17
#define GPIO_PIN_MOTOR_IN4 16

#define MOTOR_LEFT 2
#define MOTOR_RIGHT 1

#define PWM_CYCLE 12500

static PID *pid_left;
static PID *pid_right;

/*!
*	@brief      This function initializes the various GPIO Pins and PWM required
*				for the Motor Driver Module to operate
*/
void MOTOR_driver_init(PID *left, PID *right)
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

    pid_left = left;
    pid_right = right;
}

/*!
*	@brief      This function sets the direction of both the Left and Right Motor to
*               to spin in a Clockwise direction so that it will move forward when
*               a speed is set
*/
void MOTOR_move_forward()
{
    // Right Wheel Configuration
    //
    gpio_put(GPIO_PIN_MOTOR_IN1, 0);
    gpio_put(GPIO_PIN_MOTOR_IN2, 1);

    // Left Wheel Configuration
    //
    gpio_put(GPIO_PIN_MOTOR_IN3, 0);
    gpio_put(GPIO_PIN_MOTOR_IN4, 1);
}

/*!
*	@brief      This function sets the direction of both the Left and Right Motor to
*               to spin in a Anti-Clockwise direction so that it will move backwards when
*               a speed is set
*/
void MOTOR_move_backward()
{
    // Right Wheel Configuration
    //
    gpio_put(GPIO_PIN_MOTOR_IN1, 1);
    gpio_put(GPIO_PIN_MOTOR_IN2, 0);

    // Left Wheel Configuration
    //
    gpio_put(GPIO_PIN_MOTOR_IN3, 1);
    gpio_put(GPIO_PIN_MOTOR_IN4, 0);
}

/*!
*	@brief      This function sets the direction of the Left Motor to spin Clockwise and
*               the Right Motor to spin in a Anti-Clockwise direction so that it will turn
*               left when a speed is set
*/
void MOTOR_turn_left()
{
    // Set Right Wheel to go Backward
    //
    gpio_put(GPIO_PIN_MOTOR_IN1, 1);
    gpio_put(GPIO_PIN_MOTOR_IN2, 0);

    // Set Left Wheel to go Forward
    //
    gpio_put(GPIO_PIN_MOTOR_IN3, 0);
    gpio_put(GPIO_PIN_MOTOR_IN4, 1);
}

/*!
*	@brief      This function sets the direction of the Left Motor to spin Anti-Clockwise and
*               the Right Motor to spin in a Clockwise direction so that it will turn
*               right when a speed is set
*/
void MOTOR_turn_right()
{
    // Set Right Wheel to go Forward
    //
    gpio_put(GPIO_PIN_MOTOR_IN1, 0);
    gpio_put(GPIO_PIN_MOTOR_IN2, 1);

    // Set Left Wheel to go Backward
    //
    gpio_put(GPIO_PIN_MOTOR_IN3, 1);
    gpio_put(GPIO_PIN_MOTOR_IN4, 0);
}

/*!
*	@brief	                This function sets the speed for the specified motor via a duty cycle
*   @param[in]  duty_cycle  Duty cycle of the motor (0 - 100)
*   @param[in]  motor       Used in indicating which motor to set the duty cycle for
*/
void MOTOR_set_speed(uint duty_cycle, int motor)
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

    // printf("[Motor] Set speed to %u \n", duty_cycle);
}

/*!
*	@brief	                This function retrieves the speed of the specified motor
*							in terms of its duty cycle in percentage
*   @param[in]  motor       Used in indicating which motor to set the duty cycle for
*	@return					Returns the duty cycle of the specified motor
*/
int MOTOR_get_speed(int motor)
{
   if (motor == MOTOR_LEFT)
   {
		return motor_left_speed;
   }

   if (motor == MOTOR_RIGHT)
   {
		return motor_right_speed;
   }

   printf("[Motor] Invalid Motor Specified for retrieving Speed\n");
   return 0;
}

/*!
*	@brief      This function makes use of the set_speed() function to easily
*               stop the Left and Right Motors from moving
*/
void MOTOR_stop()
{
    MOTOR_set_speed(0, MOTOR_LEFT);
    MOTOR_set_speed(0, MOTOR_RIGHT);
}

void pid_stop()
{
    PID_setpoint(pid_left, 0);
    PID_setpoint(pid_right, 0);
}


bool pid_stop_callback(struct repeating_timer *t)
{
    PID_setpoint(pid_left, 0);
    PID_setpoint(pid_right, 0);

    return false;
}