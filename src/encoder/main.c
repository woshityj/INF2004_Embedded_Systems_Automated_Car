#include <stdio.h>
#include "pico/stdlib.h"

#include "encoder_driver.c"
#include "../motor/motor_driver.c"


int main()
{
    stdio_usb_init();

    // MOTOR_driver_init();
    encoder_driver_init();

    // MOTOR_move_forward();
    // set_speed(100, MOTOR_LEFT);
    // set_speed(50, MOTOR_RIGHT);

    while (true)
    {
        float speed = get_wheel_speed(2);
        float right_speed = get_wheel_speed(3);

        printf("Left Speed is %.02f cm/sec\n", speed);
        printf("Right Speed is %.02f cm/sec\n", right_speed);


        sleep_ms(2000);

        // float distance = get_wheel_distance(2);
        // printf("Distance travelled is: %0.2f cm\n", distance);
    }

    return 1;
}