#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"

#include "motor_driver.c"
#include "../encoder/encoder_driver.c"
#include "pid.c"

#define PID_Kp 2.f
#define PID_Ki 2.f
#define PID_Kd 0.f

void test_motor_move_forward()
{
    motor_driver_init();

    move_forward();
    set_speed(100, MOTOR_LEFT);
    set_speed(100, MOTOR_RIGHT);
    printf("[Motor] Test Case Motor Move Forward is Successful!");

    sleep_ms(3000);

    stop();
}

void test_motor_move_backward()
{
    motor_driver_init();

    move_backward();
    set_speed(100, MOTOR_LEFT);
    set_speed(100, MOTOR_RIGHT);
    printf("[Motor] Test Case Motor Move Backward is Successful!");

    sleep_ms(3000);

    stop();
}

void test_motor_turn_left()
{
    motor_driver_init();

    turn_left();
    set_speed(100, MOTOR_LEFT);
    set_speed(100, MOTOR_RIGHT);

    printf("[Motor] Test Case Motor Turn Left is Successful!");

    sleep_ms(3000);

    stop();
}

void test_motor_turn_right()
{
    motor_driver_init();

    turn_right();
    set_speed(100, MOTOR_LEFT);
    set_speed(100, MOTOR_RIGHT);

    printf("[Motor] Test Case Motor Turn Right is Successful!");

    sleep_ms(3000);

    stop();
}

void test_motor_set_speed()
{
    motor_driver_init();

    move_forward();
    set_speed(100, MOTOR_LEFT);
    set_speed(100, MOTOR_RIGHT);
    printf("[Motor] Test Case Motor Set Speed to 100 is Successful!");

    sleep_ms(3000);

    set_speed(50, MOTOR_LEFT);
    set_speed(50, MOTOR_RIGHT);
    printf("[Motor] Test Case Motor Set Speed to 50 is Successful!");

    sleep_ms(3000);

    set_speed(30, MOTOR_LEFT);
    set_speed(30, MOTOR_RIGHT);
    printf("[Motor] Test Case Motor Set Speed to 30 is Successful!");

    sleep_ms(3000);

    stop();
}

int main()
{
    motor_driver_init();

    move_forward_with_distance(100, 70);
}