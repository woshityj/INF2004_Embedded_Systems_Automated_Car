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

#define GRID_SIZE 15        // In terms of cm

struct repeating_timer distance_timer;

bool distance_completed = false;

typedef struct State
{
    uint left_motor_duty_cycle;
    uint right_motor_duty_cycle;

    float left_wheel_speed;
    float right_wheel_speed;

    PID *left_motor_pid;
    PID *right_motor_pid;
    repeating_timer_t pid_timer;
    repeating_timer_t wheel_speed_timer;
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

bool repeating_timer_callback_wheel_speed_isr(struct repeating_timer *t)
{
    ((State *)t->user_data)->left_wheel_speed = get_wheel_speed(GPIO_PIN_ENC_LEFT);
    ((State *)t->user_data)->right_wheel_speed = get_wheel_speed(GPIO_PIN_ENC_RIGHT);

    return true;
}

// Sensor Fusion with Motor and Wheel Encoder
//
bool repeating_timer_callback_isr(struct repeating_timer *t)
{
    if ((left_interrupts >= *(int*)t->user_data) || (right_interrupts >= *(int*)t->user_data))
    {
        distance_completed = true;
    }

    return true;
}

void move_forward_with_distance(int cm, int speed, State *state)
{
    int no_of_interrupts = cm_to_interrupts(cm);

    distance_completed = false;

    left_interrupts = 0;
    right_interrupts = 0;

    move_forward();
    setpoint_pid(state->left_motor_pid, speed);
    setpoint_pid(state->right_motor_pid, speed);
    
    add_repeating_timer_ms(-10, repeating_timer_callback_isr, &no_of_interrupts, &distance_timer);

    while (!distance_completed)
    {
        sleep_ms(10);
    }

    stop();
    setpoint_pid(state->left_motor_pid, 0);
    setpoint_pid(state->right_motor_pid, 0);

    printf("[Motor & Encoder] Moved Forward %dcm\n", cm);

    return;
}

void move_backward_with_distance(int cm, int speed, State *state)
{
    int no_of_interrupts = cm_to_interrupts(cm);

    distance_completed = false;

    left_interrupts = 0;
    right_interrupts = 0;

    move_backward();
    
    setpoint_pid(state->left_motor_pid, speed);
    setpoint_pid(state->right_motor_pid, speed);
    add_repeating_timer_ms(-10, repeating_timer_callback_isr, &no_of_interrupts, &distance_timer);

    while (!distance_completed)
    {
        sleep_ms(10);
    }

    stop();
    setpoint_pid(state->left_motor_pid, 0);
    setpoint_pid(state->right_motor_pid, 0);

    printf("[Motor & Encoder] Moved Backwards %dcm\n", cm);

    return;
}

int main( void )
{
    // Initialization
    stdio_usb_init();

    State state;

    // Mode 1 means mapping mode
    // Mode 2 means barcode scanning mode
    //
    int mode = 1;

    state.left_motor_pid = create_pid(PID_Kp, PID_Ki, PID_Kd, 0);
    state.right_motor_pid = create_pid(PID_Kp, PID_Ki, PID_Kd, 0);

    motor_driver_init();
    encoder_driver_init();

    add_repeating_timer_ms(100, repeating_timer_callback_wheel_speed_isr, &state, &state.wheel_speed_timer);
    add_repeating_timer_ms(-200, repeating_timer_callback_pid_isr, &state, &state.pid_timer);

    while(1)
    {
        switch(mode)
        {
            case 1:
                // If condition to check if front has barcode or wall
                // While there is no wall, move forward a grid
                move_forward_with_distance(GRID_SIZE, 5, &state);
                break;
            
            case 2:
                break;
            
            default:
                break;
        }
    }

    return 0;
}