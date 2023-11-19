#ifndef MOTOR_H_
#define MOTOR_H_

#include "pid.h"

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"

// GPIO Pin 0 to Control Pulse Width Modulation of Left Motor
//
#define GPIO_PIN_PWM_EN1 0

// GPIO Pin 1 to Control Pulse Width Modulation of Right Motor
//
#define GPIO_PIN_PWM_EN2 1

// GPIO Pins 19 and 18 to control the direction where the Left Motor
// will spin in
//
#define GPIO_PIN_MOTOR_IN1 19
#define GPIO_PIN_MOTOR_IN2 18

// GPIO Pins 17 and 16 to control the direction where the Right Motor
// will spin in
//
#define GPIO_PIN_MOTOR_IN3 17
#define GPIO_PIN_MOTOR_IN4 16

// Preprocessor to easily call upon the Left or Right Motor
//
#define MOTOR_LEFT 2
#define MOTOR_RIGHT 1

// Used in setting the duty cycle of the Left or Right Motor
//
#define PWM_CYCLE 12500

static uint pwm_slice;

// Used to track the speed of the left and right motor
//
static uint motor_left_speed = 0;
static uint motor_right_speed = 0;

// Function to initialize the Motor Driver
//
void MOTOR_driver_init();

// Function to set the direction of the Left and Right Motors
// to move forward
//
void MOTOR_move_forward();

// Function to set the direction of the Left and Right Motors
// to move backwards
//
void MOTOR_move_backward();

// Function to set the direction of the Left and Right Motors
// to turn left
//
void MOTOR_turn_left();

// Function to set the direction of the Left and Right Motors
// to turn right
//
void MOTOR_turn_right();

// Function to set the Left or Right Motor's PWM duty cycle
//
void MOTOR_set_speed(uint duty_cycle, int motor);

// Function to retrieve the speed of the Left or Right Motor in
// terms its duty cycle in percentage
//
int MOTOR_get_speed(int motor);

// Function to stop both Left and Right Motor
//
void MOTOR_stop();

bool pid_stop_callback(struct repeating_timer *t);

#endif