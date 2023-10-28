#include <stdio.h>
#include "pico/stdlib.h"

#include "encoder_driver.c"
#include "../motor/motor_driver.c"


int main()
{
    stdio_usb_init();

    motor_driver_init();
    encoder_driver_init();

    move_forward();
    set_speed(100, 1);

    while (true)
    {
        int speed = get_wheel_speed(2);

        printf("Speed is %d km/h\n", speed);


        sleep_ms(5000);

        float distance = get_wheel_distance(2);
        printf("Distance travelled is: %0.2f cm\n", distance);
    }

    return 1;
}