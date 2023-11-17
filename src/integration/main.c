#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

#include "../encoder/encoder_driver.c"
#include "../motor/motor_driver.c"
#include "../motor/pid.c"

#define PID_Kp 2.f
#define PID_Ki 2.f
#define PID_Kd 0.f

#define MIN_SPEED 5         // In terms of KM/h
#define MAX_SPEED 15        // In terms of KM/h


typedef struct State
{
    uint left_motor_duty_cycle;
    uint right_motor_duty_cycle;

    float left_wheel_speed;
    float right_wheel_speed;

    PID *left_motor_pid;
    PID *right_motor_pid;
} State;

bool repeating_timer_callback_pid_isr(struct repeating_timer *t)
{
    State state = *((State *)t->user_data);
    
    state.left_motor_duty_cycle = compute_pid(state.left_motor_pid, state.left_wheel_speed);
    state.right_motor_duty_cycle = compute_pid(state.right_motor_pid, state.right_wheel_speed);

    if (state.left_motor_duty_cycle != get_speed(MOTOR_LEFT))
    {
        set_speed(state.left_motor_duty_cycle, MOTOR_LEFT);
    }

    if (state.right_motor_duty_cycle != get_speed(MOTOR_RIGHT))
    {
        set_speed(state.right_motor_duty_cycle, MOTOR_RIGHT);
    }

    printf("SP: %.2f | SPD L: %.2f | DUTY L: %u | [P]%.2f [I]%.2f [D]%.2f (Err: %.2f) \n", state.left_motor_pid->setpoint, state.left_wheel_speed, state.left_motor_duty_cycle, state.left_motor_pid->p, state.left_motor_pid->i, state.left_motor_pid->d, state.left_motor_pid->prev_error);
    printf("SP: %.2f | SPD R: %.2f | DUTY R: %u | [P]%.2f [I]%.2f [D]%.2f (Err: %.2f) \n", state.right_motor_pid->setpoint, state.right_wheel_speed, state.right_motor_duty_cycle, state.right_motor_pid->p, state.right_motor_pid->i, state.right_motor_pid->d, state.right_motor_pid->prev_error);

    return true;
}

int main( void )
{
    // Initialization
    stdio_usb_init();

    State state;

    struct repeating_timer timer;

    state.left_motor_pid = create_pid(PID_Kp, PID_Ki, PID_Kd, 0);
    state.right_motor_pid = create_pid(PID_Kp, PID_Ki, PID_Kd, 0);

    motor_driver_init();
    encoder_driver_init();

    add_repeating_timer_ms(-200, repeating_timer_callback_pid_isr, &state, &timer);
    setpoint_pid(state.left_motor_pid, 5);
    setpoint_pid(state.right_motor_pid, 5);

    while(1)
    {
        move_forward();

        state.left_wheel_speed = get_wheel_speed(GPIO_PIN_ENC_LEFT);
        state.right_wheel_speed = get_wheel_speed(GPIO_PIN_ENC_RIGHT);

        sleep_ms(100);
    }

    return 0;
}