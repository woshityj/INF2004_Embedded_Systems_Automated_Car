#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware_adc/pwm.h"

#include "motor_driver.c"
#include "pid.c"


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

int main()
{

}