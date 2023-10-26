#ifndef MOTOR_H_
#define MOTOR_H_

#include "pid.h"

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"

// GPIO Pin 0 to Control PWM of Left Wheel
//
#define GPIO_PIN_PWM_EN1 0

// GPIO Pin 1 to Control PWM of Right Wheel
//
#define GPIO_PIN_PWM_EN2 1

#define GPIO_PIN_MOTOR_IN1 19
#define GPIO_PIN_MOTOR_IN2 18
#define GPIO_PIN_MOTOR_IN3 17
#define GPIO_PIN_MOTOR_IN4 16

#define MOTOR_LEFT 1
#define MOTOR_RIGHT 2

#define PWM_CYCLE 12500

static uint pwm_slice;

void move_forward();
void move_backward();
void turn_left();
void turn_right();
void set_speed(uint duty_cycle, int motor);
void stop();
void motor_driver_init();

#endif