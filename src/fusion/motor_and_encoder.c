#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"

#include "../motor/motor_driver.c"
#include "../encoder/encoder_driver.c"

#define MOTOR_LEFT 2
#define MOTOR_RIGHT 1

bool distance_completed = false;

// TODO: Create a repeating timer that triggers every 10ms to
// check if the distance travelled is completed

bool repeating_timer_callback_isr(struct repeating_timer *t)
{
    // Check if number of interrupts is reached for either wheel
    //
    if ((left_interrupts >= *(int*)t->user_data) || (right_interrupts >= *(int*)t->user_data))
    {
        distance_completed = true;
    }

    return true;
}

void move_forward_with_distance(int cm)
{
    int no_of_interrupts = cm_to_interrupts(cm);

    int speed = 80;

    struct repeating_timer timer;
    
    distance_completed = false;

    // Reset total distance travelled for each wheel
    left_interrupts = 0;
    right_interrupts = 0;

    // Set the direction of the wheels to move forward
    //
    move_forward();

    set_speed(speed, MOTOR_LEFT);
    set_speed(speed, MOTOR_RIGHT);

    add_repeating_timer_ms(-10, repeating_timer_callback_isr, &no_of_interrupts, &timer);

    while (!distance_completed)
    {
        sleep_ms(10);
    }

    stop();

    printf("[Motor & Encoder] Moved Forward %dcm", cm);

    return;
}

void move_backward_with_distance(int cm)
{
    int no_of_interrupts = cm_to_interrupts(cm);

    int speed = 80;

    struct repeating_timer timer;

    distance_completed = false;

    left_interrupts = 0;
    right_interrupts = 0;

    move_backward();

    set_speed(speed, MOTOR_LEFT);
    set_speed(speed, MOTOR_RIGHT);

    add_repeating_timer_ms(-10, repeating_timer_callback_isr, &no_of_interrupts, &timer);

    while (!distance_completed)
    {
        sleep_ms(10);
    }

    stop();

    printf("[Motor & Encoder] Moved Backwards %dcm", cm);

    return;
}

int main()
{
    stdio_usb_init();

    motor_driver_init();
    encoder_driver_init();

    move_forward_with_distance(50);

    return 1;
}